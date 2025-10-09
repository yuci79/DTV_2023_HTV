/*
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaLauncherWindow class.
 */

#include "OperaLauncherWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include <omi/messages/CreateOIPFWindow.h>
#include <omi/messages/CreateOIPFApplication.h>
#include <omi/messages/TerminateOIPFApplication.h>
#include <omi/messages/RestartOIPFApplication.h>
#include <omi/messages/DestroyWindow.h>

#include "OperaEngine.hpp"

#include "profile_helper/ProfileHelper.h"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/launcherwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaLauncherWindow::OperaLauncherWindow(OperaEngine& opera_engine,
                            int event_dispatch_priority)
    : OperaWindow(opera_engine, OperaWindow::window_name_launcher, event_dispatch_priority)
    , m_window_type(OperaWindow::WindowType::window_launcher)
    , m_application_id(0)
{
    createWindow();
}

OperaLauncherWindow::~OperaLauncherWindow()
{
    TRACE_INFO(("%s()\n", __func__));

    if (m_window_handle.IsValid())
    {
        using omi::msg::DestroyWindow;
        DestroyWindow::Result result = m_opera_engine.post(DestroyWindow(m_window_handle)).get();
        CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
    }
}

void
OperaLauncherWindow::createWindow()
{
    TRACE_ALWAYS(("%s()\n", __func__));
    m_application_id = 0;
    auto create_result = m_opera_engine.post(omi::msg::CreateOIPFWindow("", omi::msg::CreateOIPFWindow::OIPF_KEY_ALL)).get();
    m_window_handle = getWindowHandle(create_result);
    updateOperaInputMethod();

    if (Profile_GetLauncherWindowRenderingResolution()
                   == window_rendering_resolution_1920x1080)
    {
        setRenderingResolution(rendering_resolution_1920x1080);
    }
    else
    {
        setRenderingResolution(rendering_resolution_1280x720);
    }
    setDefaultUserAgentString();
    setPreferenceValue("block_mixed_xhr_content", "false");
}


bool
OperaLauncherWindow::loadUrl(cabot::String const& url,
                     RenderingResolution resolution)
{
    bool ok = false;

    //Different from default_broadcast_independent_keyset for numeric keys to be not handled by Launcher
    m_keyset.keyset_value = keyset_red
        | keyset_green | keyset_yellow | keyset_blue | keyset_navigation
        | keyset_vcr | keyset_scroll | keyset_info | keyset_alpha;

    if (m_window_handle.IsValid())
    {
        if (m_application_id == 0)
        {
            using omi::msg::CreateOIPFApplication;
            CreateOIPFApplication::Result result = m_opera_engine.post(
                         CreateOIPFApplication(m_window_handle, 0,url.charArray())).get();
            if (checkResult(result))
            {
                ok = true;
                m_current_url = url;
                m_application_id = result.application_id;

                setPreferenceValue("spatial_navigation_enabled.default", "false");
                setPreferenceValue("spatial_navigation_enabled.override", "false");
            }
            TRACE_ALWAYS(("loadUrl(): [%s], create launcher application result: %s, app_id: %d\n",
                    url.charArray(), result.errorString().c_str(), m_application_id));
        }
        else
        {
            // App is already loaded so, just return ok for launcher case.
            TRACE_ALWAYS(("loadUrl(): [%s], launcher application is already running, app_id: %d\n",
                    url.charArray(), m_application_id));
            ok = true;
        }
    }
    else
    {
        TRACE_ERROR(("loadUrl(): No valid window handle \n"));
    }

    return ok;
}

void
OperaLauncherWindow::terminateCurrentApp()
{
    TRACE_ALWAYS(("terminateCurrentApp() app_id: %d\n", m_application_id));
    if (m_window_handle.IsValid())
    {
        using omi::msg::TerminateOIPFApplication;
        TerminateOIPFApplication::Result terminate_result = m_opera_engine.post(
                TerminateOIPFApplication(m_window_handle, m_application_id)).get();
        CABOT_ASSERT(checkResult(terminate_result) || checkBrowserShutdown(terminate_result));
        m_application_id = 0;
        clearKeyset();
        m_current_url.clear();
    }
}

OperaWindow::WindowType
OperaLauncherWindow::type() const
{
    return m_window_type;
}

void
OperaLauncherWindow::reloadCurrentApp()
{
    TRACE_ALWAYS(("reloadCurrentApp()\n"));

    if (m_window_handle.IsValid() && (m_application_id != 0))
    {
        TRACE_ALWAYS(("Reloading Current App\n"));
        using omi::msg::RestartOIPFApplication;
        RestartOIPFApplication::Result restart_result = m_opera_engine.post(
                RestartOIPFApplication(m_window_handle, m_application_id)).get();
        CABOT_ASSERT(checkResult(restart_result) || checkBrowserShutdown(restart_result));
    }
}

