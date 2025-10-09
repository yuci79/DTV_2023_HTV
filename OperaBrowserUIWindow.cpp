/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaBrowserUIWindow class.
 */


#include "OperaBrowserUIWindow.hpp"
#include "vewd_integration/helper/OperaSdk4Utilities.hpp"

#include <omi/messages/CreateBrowserUIWindow.h>
#include <omi/messages/DestroyWindow.h>
#include "OperaEngine.hpp"
#include "utilities_debug/unused.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/browseruiwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaBrowserUIWindow::OperaBrowserUIWindow(OperaEngine& opera_engine,
                                           cabot::String const& start_url,
                                           int event_dispatch_priority,
                                           unsigned int plane_height)
    : OperaWindow(opera_engine, window_name_browser_ui, event_dispatch_priority)
{
    using omi::msg::CreateBrowserUIWindow;

    m_current_url = start_url;
    m_keyset.keyset_value = default_broadcast_independent_keyset;
    auto result = m_opera_engine.post(CreateBrowserUIWindow(start_url.charArray(), false)).get();
    m_window_handle = getWindowHandle(result);
    setPreferenceValue("transparent", "true");
    setVisible(true);
    updateOperaInputMethod();

    if (plane_height == 1080)
    {
        setRenderingResolution(rendering_resolution_1920x1080);
    }
    else
    {
        setRenderingResolution(rendering_resolution_1280x720);
    }
    setDefaultUserAgentString();
    setPreferenceValue("spatial_navigation_enabled.override", "false");
}

OperaBrowserUIWindow::~OperaBrowserUIWindow()
{
    stop();
}

bool
OperaBrowserUIWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(url);
    CABOT_UNUSED_PARAM(resolution);
    return true;
}

void OperaBrowserUIWindow::terminateCurrentApp()
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
OperaBrowserUIWindow::type() const
{
    return OperaWindow::WindowType::window_browser_ui;
}

void
OperaBrowserUIWindow::stop()
{

    if (m_window_handle.IsValid())
    {
        clearKeyset();
        //m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
        m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
        m_window_handle = OperaWindowHandle(-1);
        m_current_url.clear();
    }
}
