/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaOipfWindow class.
 */

#include "OperaOipfWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include <omi/messages/CreateOIPFWindow.h>
#include <omi/messages/CreateOIPFApplication.h>
#include <omi/messages/TerminateOIPFApplication.h>
#include <omi/messages/DestroyWindow.h>
#include "OperaEngine.hpp"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/oipfwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaOipfWindow::OperaOipfWindow(OperaEngine& opera_engine,
                            cabot::String const& window_name,
                            OperaWindow::WindowType window_type,
                            int event_dispatch_priority)
    : OperaWindow(opera_engine, window_name, event_dispatch_priority)
    , m_window_type(window_type)
    , m_application_id(0)
{

}

OperaOipfWindow::~OperaOipfWindow()
{
    TRACE_INFO(("%s()\n", __func__));

    if(m_window_handle.IsValid())
    {
        using omi::msg::DestroyWindow;
        //DestroyWindow::Result result = m_opera_engine.post(DestroyWindow(m_window_handle)).get();
        m_opera_engine.post(DestroyWindow(m_window_handle));
        //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
    }
}

bool
OperaOipfWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    bool ok = false;
    TRACE_INFO((" loadUrl() [%s], window: %s\n", url.charArray(), m_window_handle.str().c_str()));
    m_keyset.keyset_value = default_broadcast_independent_keyset;

    if (!m_window_handle.IsValid())
    {
        auto create_result = m_opera_engine.post(omi::msg::CreateOIPFWindow("", omi::msg::CreateOIPFWindow::OIPF_KEY_ALL)).get();
        m_window_handle = getWindowHandle(create_result);
        updateOperaInputMethod();

        setRenderingResolution(rendering_resolution_1280x720);
        setDefaultUserAgentString();
        setPreferenceValue("block_mixed_xhr_content", "false");

        using omi::msg::CreateOIPFApplication;
        CreateOIPFApplication::Result result = m_opera_engine.post(
                                                        CreateOIPFApplication(m_window_handle, 0,url.charArray())).get();

        if (checkResult(result))
        {
            ok = true;
            m_current_url = url;
            m_application_id = result.application_id;
        }
        TRACE_INFO(("loadUrl() [%s], create OIPF application result: %s, app_id: %d\n",
                url.charArray(), result.errorString().c_str(), m_application_id));
    }
    TRACE_INFO((" loadUrl() [%s], window: %s, ok: %d \n", url.charArray(), m_window_handle.str().c_str(), ok));

    return ok;
}

void
OperaOipfWindow::terminateCurrentApp()
{
    TRACE_INFO((" terminateCurrentApp() \n"));
    if (m_window_handle.IsValid())
    {
        using omi::msg::TerminateOIPFApplication;
        //TerminateOIPFApplication::Result terminate_result = m_opera_engine.post(
        //        TerminateOIPFApplication(m_window_handle, m_application_id)).get();
        m_opera_engine.post(TerminateOIPFApplication(m_window_handle, m_application_id));
        
        //CABOT_ASSERT(checkResult(terminate_result) || checkBrowserShutdown(terminate_result));

        using omi::msg::DestroyWindow;
        //DestroyWindow::Result result = m_opera_engine.post(DestroyWindow(m_window_handle)).get();
        //CABOT_ASSERT(checkResult(result) || checkBrowserShutdown(result));
        m_opera_engine.post(DestroyWindow(m_window_handle));

        clearKeyset();
        m_application_id = 0;
        const OperaWindowHandle terminated_window = m_window_handle;
        m_window_handle = OperaWindowHandle(-1);
        m_current_url.clear();
        m_opera_engine.onAppTerminated(m_window_handle,
                                       terminated_window,
                                       cabot::String(),
                                       false,
                                       WebBrowserAppType::user_app);
    }
}

OperaWindow::WindowType
OperaOipfWindow::type() const
{
    return m_window_type;
}
