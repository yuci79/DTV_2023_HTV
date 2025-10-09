/*
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaDashPlayerWindow class.
 */
#include "OperaDashPlayerWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"

#include "3rd_party_wrapper/curl/include/CurlUtils.hpp"
#include "3rd_party_wrapper/curl/include/EasyWrapper.hpp"
#include <omi/messages/CreateWindow.h>
#include <omi/messages/LoadURL.h>

#include <omi/messages/DestroyWindow.h>
#include <omi/messages/StackAtBottom.h>

#include "OperaEngine.hpp"

#include "utilities_public/String.hpp"
#include "utilities_public/UString.hpp"

#include "utilities_debug/trace.h"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

TRACE_IMPLEMENT("opera/dashplayerwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaDashPlayerWindow::OperaDashPlayerWindow(
    OperaEngine& opera_engine, int event_dispatch_priority, int resource_id)
    : OperaLinearIPServiceWindow(opera_engine, OperaWindow::window_name_media_player,
                                 event_dispatch_priority)
    , m_window_type(OperaWindow::WindowType::window_media_player)
{
    CABOT_UNUSED_PARAM(resource_id);

    clearKeyset();
}

OperaDashPlayerWindow::~OperaDashPlayerWindow()
{
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
}

bool
OperaDashPlayerWindow::generateDataUrl(cabot::String const mpd_url,
                                       cabot::String& data_url,
                                       bool check_mpd_valid) const
{
    Curl::EasyWrapper easy_wrapper;
    if (easy_wrapper.create())
    {
        if (check_mpd_valid)
        {
            // time out arbitrary, all of this code will be removed once VEWD supply SDK423
            const int timeout_in_seconds = 5;
            if (!Curl::CurlUtils::urlIsValid(
                easy_wrapper,
                mpd_url,
                timeout_in_seconds,
                Curl::CurlUtils::RequestType::body))
            {
                TRACE_ERROR(("Unable to connect to url = '%s' therefore will not generate data url\n",
                    mpd_url.charArray()));
                return false;
            }
        }
        
        // See: http://rdwiki/confluence/display/SWBINT/2021/03/15/MPD+playback
        cabot::String data_url_payload;
        data_url_payload += "<!DOCTYPE html>\n";
        data_url_payload += "<html>\n";

        data_url_payload += "<body style=\"margin: 0px\">\n";
        data_url_payload += "<video autoplay=\"autoplay\" src=" + mpd_url;
        data_url_payload += " style=\"height: 100%; width: 100%; margin: auto\"></video>\n";
        data_url_payload += "</body>\n";
        data_url_payload += "</html>\n";    
        
        // Example of including video events start
        data_url_payload += "<script>\n";
        data_url_payload += "const video = document.querySelector('video');\n";
        data_url_payload += "video.onended = (event) => {\n";
        data_url_payload += "console.log('Video stopped, would call middleware here');\n";
        data_url_payload += "}\n";
        data_url_payload += "video.onerror = (event) => {\n";
        data_url_payload += "console.log('Video error, would call middleware here');\n";
        data_url_payload += "}\n";
        data_url_payload += "</script>\n";
        // Example of including video events end
        
        cabot::String encoded_url;
        encoded_url = Curl::CurlUtils::escapeUrl(easy_wrapper, data_url_payload);
        data_url = "data:text/html,";
        data_url += encoded_url;
        return true;
    }
    else
    {
        SHOULD_NOT_BE_HERE();
    }
    return false;
}

bool
OperaDashPlayerWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    TRACE_INFO(("loadUrl() url %s\n", url.charArray()));
    bool ok = false;
    cabot::String data_url;
    if (generateDataUrl(url, data_url, true))
    {
        TRACE_INFO(("data_url = '%s'\n", data_url.charArray()));

        clearKeyset();
        if (!m_window_handle.IsValid())
        {
            using omi::msg::CreateWindow;
            const auto result = m_opera_engine.post(CreateWindow(false)).get();
            m_window_handle = getWindowHandle(result);

            updateOperaInputMethod();

            setPreferenceValue("transparent", "true");
            setPreferenceValue("default_background_color", "transparent");
            setPreferenceValue("default_cert_error_page_enabled", "false");
            setPreferenceValue("notify_scripts_closing_windows", "true");
            setPreferenceValue("block_error_pages", "true");
            setPreferenceValue("block_mixed_xhr_content", "false");

            setRenderingResolution(rendering_resolution_1280x720);
            
            setDefaultUserAgentString();

            setInitialWindowPosition();
        }

        using omi::msg::LoadURL;
        const auto result = m_opera_engine.post(
            LoadURL(m_window_handle, data_url.charArray())
        ).get();

        if (checkResult(result))
        {
            ok = true;
            m_current_url = url;
            m_unencoded_url = url;
        }

        using omi::msg::StackAtBottom;
        (void) m_opera_engine.post(StackAtBottom(m_window_handle)).get();
    }
    return ok;
}

void
OperaDashPlayerWindow::terminateCurrentApp()
{
    if (m_window_handle.IsValid())
    {
        cabot::String url = m_current_url;
        cabot::String data_url;

        if (generateDataUrl(m_unencoded_url, data_url, false) && 
            data_url == m_current_url)
        {
            url = m_unencoded_url;
            TRACE_INFO(("terminateCurrentApp() match to unencoded url found, using unencoded : '%s'\n", 
                url.charArray()));
        }
        const OperaWindowHandle terminated_window = m_window_handle;
        stop();
        m_opera_engine.onAppTerminated(m_window_handle,
                                       terminated_window,
                                       url,
                                       false,
                                       WebBrowserAppType::media_player);
    }
}

bool 
OperaDashPlayerWindow::checkUnEncodedUrl(
    cabot::String const encoded_url, 
    cabot::String& unencoded_url) const
{
    cabot::String data_url;
    if (!m_unencoded_url.empty() && 
        generateDataUrl(m_unencoded_url, data_url, false) &&
        data_url == encoded_url)
    {
        unencoded_url = m_unencoded_url;
        return true;
    }
    unencoded_url.clear();
    return false;
}

OperaWindow::WindowType
OperaDashPlayerWindow::type() const
{
    return m_window_type;
}

void
OperaDashPlayerWindow::stop()
{
    clearKeyset();
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
    m_window_handle = OperaWindowHandle(-1);
    m_current_url.clear();
    m_unencoded_url.clear();
}
