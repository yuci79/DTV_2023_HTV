/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaBrowserWindow class.
 */
#include "OperaBrowserWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include <omi/messages/CreateWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/LoadURL.h>
#include "OperaEngine.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/browserwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaBrowserWindow::OperaBrowserWindow(OperaEngine& opera_engine,
                            cabot::String const& window_name,
                            OperaWindow::WindowType window_type,
                            int event_dispatch_priority)
    : OperaWindow(opera_engine, window_name, event_dispatch_priority)
    , m_window_type(window_type)
{
    clearKeyset();
}

OperaBrowserWindow::~OperaBrowserWindow()
{
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
}

bool
OperaBrowserWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    TRACE_ALWAYS(("loadUrl() url %s\n", url.charArray()));

    bool ok = false;

    clearKeyset();
    m_keyset.keyset_value = all_keyset;

    if (!m_window_handle.IsValid())
    {
        using omi::msg::CreateWindow;
        auto result = m_opera_engine.post(CreateWindow(false)).get();
        m_window_handle = getWindowHandle(result);
        updateOperaInputMethod();

        if (m_window_type == OperaWindow::WindowType::window_csp)
        {
            resolution = rendering_resolution_1280x720;
        }

        setDefaultUserAgentString();
        setPreferenceValue("transparent", "true");
    }

    if (resolution != rendering_resolution_default)
    {
        setRenderingResolution(resolution);
    }

    using omi::msg::LoadURL;
    m_opera_engine.post(LoadURL(m_window_handle, url.charArray()));
    //LoadURL::Result result = m_opera_engine.post(LoadURL(m_window_handle, url.charArray())).get();
    //if (checkResult(result))
    {
        ok = true;
        m_current_url = url;
    }
    return ok;
}

void
OperaBrowserWindow::terminateCurrentApp()
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
OperaBrowserWindow::type() const
{
    return m_window_type;
}

void
OperaBrowserWindow::stop()
{
    clearKeyset();
    //m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
    m_window_handle = OperaWindowHandle(-1);
    m_current_url.clear();
}
