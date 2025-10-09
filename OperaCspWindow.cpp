/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaCspWindow class.
 */

#include "OperaCspWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include <omi/messages/CreateWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/LoadURL.h>
#include "OperaEngine.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_debug/trace.h"
#include "profile_helper/ProfileHelper.h"
#include "utilities_debug/unused.h"

TRACE_IMPLEMENT("opera/cspwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaCspWindow::OperaCspWindow(OperaEngine& opera_engine,
                               int event_dispatch_priority)
    : OperaLinearIPServiceWindow(opera_engine, OperaWindow::window_name_csp, event_dispatch_priority)
    , m_window_type(OperaWindow::WindowType::window_csp)
{
    clearKeyset();
}

OperaCspWindow::~OperaCspWindow()
{
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
}

bool
OperaCspWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    TRACE_ALWAYS(("loadUrl() url %s\n", url.charArray()));

    bool ok = false;
    using omi::msg::LoadURL;

    cabot::String load_url = url;

    clearKeyset();
    m_keyset.keyset_value = default_broadcast_independent_keyset;
    if (!m_window_handle.IsValid())
    {
        using omi::msg::CreateWindow;
        auto result = m_opera_engine.post(CreateWindow(false)).get();
        m_window_handle = getWindowHandle(result);
        updateOperaInputMethod();

        setPreferenceValue("transparent", "true");
        setPreferenceValue("default_background_color", "transparent");
        setPreferenceValue("default_cert_error_page_enabled", "false");
        setPreferenceValue("notify_scripts_closing_windows", "true");
        setPreferenceValue("block_error_pages", "true");
        setPreferenceValue("block_mixed_xhr_content", "false");

        int width;
        int height;
        bool fhd_plane_needed = false;
        NEBULA_SystemInformationProviderGetPanelInformation(width, height);
        char board_name[32] = {0};
        Profile_GetBoardName(board_name, 32);
        cabot::String board_name_str(board_name);

        if ((width > 2 * 1280) &&
           (height > 2 * 720))  // YT 2017 does not allow devicePixelRatio to be bigger than 2.
        {
            fhd_plane_needed = true;
        }
        bool fhd_needed_for_youtube = ((load_url.find("youtube.com") != cabot::String::npos) ||
                (load_url.find("qual-e.appspot.com") != cabot::String::npos)) &&
                fhd_plane_needed;

        
        if (fhd_needed_for_youtube ||
            load_url.find("vstl_res=fhd") != cabot::String::npos) // For FHD needed apps
        {
            TRACE_INFO((" FHD Resolution is Needed for  %s\n", load_url.charArray()));
            setRenderingResolution(rendering_resolution_1920x1080);

            //Revert custom link configuration query string before load
            load_url.replaceAll("vstl_res=fhd&","");
            load_url.replaceAll("&vstl_res=fhd","");
            load_url.replaceAll("?vstl_res=fhd/","");
            load_url.replaceAll("?vstl_res=fhd","");
        }
        else
        {
            setRenderingResolution(rendering_resolution_1280x720);
        }
        
        if (load_url.find("tv.tiktok") != std::string::npos)
        {
            setTikTokUserAgentString();
        }
        else
        {
            setDefaultUserAgentString();
        }
    }
    //LoadURL::Result result = m_opera_engine.post(LoadURL(m_window_handle, load_url.charArray())).get();
    m_opera_engine.post(LoadURL(m_window_handle, load_url.charArray()));
    //if (checkResult(result))
    {
        ok = true;
        m_current_url = load_url;
    }
    return ok;
}

void
OperaCspWindow::terminateCurrentApp()
{
    if (m_window_handle.IsValid())
    {
        cabot::String url = m_current_url;
        const OperaWindowHandle terminated_window = m_window_handle;
        stop();
        m_opera_engine.onAppTerminated(m_window_handle,
                                       terminated_window,
                                       url,
                                       false,
                                       WebBrowserAppType::user_app);
    }
}

OperaWindow::WindowType
OperaCspWindow::type() const
{
    return m_window_type;
}

void
OperaCspWindow::stop()
{
    clearKeyset();
    //m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
    m_window_handle = OperaWindowHandle(-1);
    m_current_url.clear();
}
