/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaBrowserCore class.
 */

#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include "OperaHbbTVWindow.hpp"
#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include <omi/messages/CreateHbbTVWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/LoadAITApplication.h>
#include <omi/messages/LoadURLApplication.h>
#include <omi/messages/SetBroadbandStatus.h>
#include <omi/messages/TerminateApplication.h>
#include <omi/messages/SetCICAMStatus.h>
#include "OperaEngine.hpp"
#include "utilities_public/UnderlyingType.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/hbbtvwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaHbbTVWindow::OperaHbbTVWindow(OperaEngine& opera_engine,
                                   int event_dispatch_priority)
    : OperaLinearIPServiceWindow(opera_engine, window_name_hbbtv, event_dispatch_priority)
    , m_restart_after_termination(true)
    , m_broadcast_related(false)
    , m_focusable_override(false)
    , m_focusable_override_value(false)
{
    m_keyset.keyset_value = default_broadcast_related_keyset;

    createWindow();
}

void
OperaHbbTVWindow::createWindow()
{
    using omi::msg::CreateHbbTVWindow;
    std::vector<CreateHbbTVWindow::PlatformProfile> hbbtv_platform_profiles;
    CreateHbbTVWindow::PlatformProfile platform_profile;
    int hbbtv_profile = m_opera_engine.getHbbTvProfile();
    switch (static_cast<CreateHbbTVWindow::PlatformProfile>(hbbtv_profile))
    {
#if defined USING_SDK423
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2023:
#endif
#if defined USING_SDK422 || defined USING_SDK423
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2022:
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2021:
#endif
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2020:
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2019:
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2018:
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2017:
#if defined USING_SDK422 || defined USING_SDK423
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_3:
#endif
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_2:
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_1:
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0:
        case CreateHbbTVWindow::PlatformProfile::TDT_HYBRIDA:
        case CreateHbbTVWindow::PlatformProfile::HBBTV_1_5:
            platform_profile = static_cast<omi::PlatformProfile>(hbbtv_profile);
            break;
        default:
            TRACE_ERROR(("Unknown profile %d - defaulting to HbbTV 1.5\n", hbbtv_profile));
            platform_profile = CreateHbbTVWindow::PlatformProfile::HBBTV_1_5;
            break;
    }
    switch (platform_profile)
    {
#if defined USING_SDK423
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2023:  // 1.6.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2023);
            // FALL-THROUGH
#endif            
#if defined USING_SDK422 || defined USING_SDK423
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2022:  // 1.6.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2022);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_3:    // 1.6.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_3);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2021:  // 1.5.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2021);
            // FALL-THROUGH
#endif
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2020:  // 1.5.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2020);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2019:  // 1.5.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2019);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_2:    // 1.5.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_2);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2018:  // 1.4.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2018);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_1:    // 1.4.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::HBBTV_2_0_1);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::FREEVIEW_2017:  // 1.3.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::FREEVIEW_2017);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::HBBTV_2_0:      // 1.3.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::HBBTV_2_0);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::TDT_HYBRIDA:    // 1.2.1+
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::TDT_HYBRIDA);
            // FALL-THROUGH
        case CreateHbbTVWindow::PlatformProfile::HBBTV_1_5:      // 1.2.1
            hbbtv_platform_profiles.push_back(CreateHbbTVWindow::PlatformProfile::HBBTV_1_5);
            break;
        default:
            break;
    }
    auto result = m_opera_engine.opera()->post(
            CreateHbbTVWindow(hbbtv_platform_profiles,
                              "",                               // user_agent_suffix
                              CreateHbbTVWindow::HBBTV_KEY_ALL, // key_set_mask
                              true,                             // visible
                              true,                             // focused
                              omi::Rect(),                      // bounds
                              true,                             // handle_oom_internally
                              true,                             // ait_bound
                              false,                            // is_hbbtv_only_receiver (*)
                              true                              // is_internal_autoselector_enabled
            )).get();

    // SUNSTONE-2146: SI_UK_DTG: AIT012d::Connection requirement displays the incorrect HbbTV Application when disconnected.
    // Note (*): Changed is_hbbtv_only_receiver default from true to pass AIT012d

    m_window_handle = getWindowHandle(result);

    updateOperaInputMethod();
    TRACE_INFO(("%s() %s [%s]\n", __func__,
                m_window_name.c_str(), m_window_handle.str().c_str()));

    /* do not modify or remove this we do not have control over hbbtv window res.

    If rendering resolution is to change the devicePixelRatio value in pixelratio.user.js
    will need to be updated. See comment in pixelratio.user.js. */
    setRenderingResolution(rendering_resolution_1280x720);

    setPreferenceValue("media_backend_prefs",  "{\"delay_play_until_video_show\":\"true\"}");

    setPreferenceValue("block_mixed_xhr_content", "false");

    setDefaultUserAgentString();
}

OperaHbbTVWindow::~OperaHbbTVWindow()
{   
    clearKeyset();

    using omi::msg::DestroyWindow;
    //DestroyWindow::Result result = m_opera_engine.post(DestroyWindow(m_window_handle)).get();
    //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
    m_opera_engine.post(DestroyWindow(m_window_handle));
}

bool
OperaHbbTVWindow::loadApp(unsigned int org_id, unsigned short app_id)
{
    bool ok = false;

    using omi::msg::LoadAITApplication;
    //LoadAITApplication::Result result = m_opera_engine.post(
    //    LoadAITApplication(m_window_handle, app_id, org_id)).get();
    //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
    
    m_opera_engine.post(LoadAITApplication(m_window_handle, app_id, org_id));

    //if (result.error == LoadAITApplication::NO_ERR)
    {
        ok = true;
    }

    return ok;
}

bool
OperaHbbTVWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    bool ok = false;
    using omi::msg::LoadURLApplication;
    //LoadURLApplication::Result result = m_opera_engine.post(
    //        LoadURLApplication(m_window_handle, url.charArray())).get();
    m_opera_engine.post(LoadURLApplication(m_window_handle, url.charArray()));
    
    //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));

    //if (result.error == LoadURLApplication::NO_ERR)
    {
        ok = true;
    }

    return ok;
}

bool 
OperaHbbTVWindow::terminateCurrentAppWithResult()
{
    if(((m_opera_engine.getCurrentAppType() == WebBrowserAppType::hbbtv_app  ||
         m_opera_engine.getCurrentAppType() == WebBrowserAppType::hbbtv_txt_app) &&
         m_opera_engine.isAppRunning()) || m_opera_engine.isHbbTvAppStarting())
    {
        const bool restart_after_termination = m_restart_after_termination;

        TRACE_INFO(("on terminate Linear IP Service = '%s' restart after termination = '%s'\n",
            (isLinearIPService() ? "true" : "false"),
            (restart_after_termination ? "true" : "false")));

        using omi::msg::TerminateApplication;
#ifdef USING_SDK423
        // Workaround for browser hang using get() sometimes on channel change.
        // TODO: Revert when fixed by Vewd - STEALTH-1800.
        m_opera_engine.post(
                TerminateApplication(m_window_handle, restart_after_termination));
#else
        TerminateApplication::Result terminate_result = m_opera_engine.post(
                TerminateApplication(m_window_handle, restart_after_termination)).get();
        CABOT_ASSERT(checkResult(terminate_result) || checkBrowserShutdown(terminate_result));
#endif
       return true;
    }
    else
    {
        TRACE_INFO(("terminateCurrentApp ignored app type = '%d' is running = '%s' is starting = '%s' Linear IP Service = '%s'\n",
            convertToUnderlyingType(m_opera_engine.getCurrentAppType()),
            (m_opera_engine.isAppRunning() ? "true" : "false"),
            (m_opera_engine.isHbbTvAppStarting() ? "true" : "false"),
            (isLinearIPService() ? "true" : "false")));
    }
    return false;
}

void
OperaHbbTVWindow::terminateCurrentApp()
{
    (void)terminateCurrentAppWithResult();
}

void
OperaHbbTVWindow::setRestartAfterTermination(bool restart_flag)
{
    m_restart_after_termination = restart_flag;
}

void
OperaHbbTVWindow::onConnectionStatusChange(bool connected)
{
    using omi::msg::SetBroadbandStatus;
    SetBroadbandStatus::Status status = SetBroadbandStatus::AUTODETECT;
    if (connected)
    {
        status = SetBroadbandStatus::AVAILABLE;
    }
    else
    {
        status = SetBroadbandStatus::NOT_AVAILABLE;
    }

    m_opera_engine.opera()->post(SetBroadbandStatus(handle(), status));

    //SetBroadbandStatus::Result result = m_opera_engine.opera()->post(
    //        SetBroadbandStatus(handle(), status)).get();
    //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
}

void
OperaHbbTVWindow::onCicamStatusChange(bool available)
{
    using omi::msg::SetCICAMStatus;

    //SetCICAMStatus::Result result = m_opera_engine.post(
    //SetCICAMStatus(handle(), available)).get();
    
    m_opera_engine.post(SetCICAMStatus(handle(), available));
    //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));

}

OperaWindow::WindowType
OperaHbbTVWindow::type() const
{
    return OperaWindow::WindowType::window_hbbtv;
}

bool
OperaHbbTVWindow::isFocusable() const
{
    return m_focusable_override ? m_focusable_override_value
                                : OperaWindow::isFocusable();
}

void
OperaHbbTVWindow::setFocusableOverride(bool override, bool focusable)
{
    m_focusable_override = override;
    m_focusable_override_value = focusable;
}

void
OperaHbbTVWindow::setBroadcastRelated(bool broadcast_related)
{
    m_broadcast_related = broadcast_related;
}

bool
OperaHbbTVWindow::broadcastRelated() const
{
    return m_broadcast_related;
}

cabot::String const&
OperaHbbTVWindow::getAutoStartUrl()
{
    return m_auto_start_url;
}

void
OperaHbbTVWindow::setAutoStartUrl(cabot::String const& url)
{
    m_auto_start_url = url;
}

void
OperaHbbTVWindow::reset(bool clear_keyset)
{
    OperaWindow::reset(clear_keyset);

    m_restart_after_termination = true;
    m_broadcast_related = false;
    m_auto_start_url.clear();
}

bool
OperaHbbTVWindow::shouldHandleKey(nebula::KeyCode key_code,
                        nebula::KeyPressHandlingReason keypress_handling_reason) const
{
    bool result = false;

    //if (isFocused())
    {
        result = OperaWindow::shouldHandleKey(key_code, keypress_handling_reason);
    }

    return result;
}
