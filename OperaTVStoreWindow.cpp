/*
 * Copyright 2015 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaTVStoreWindow class.
 */

#include "OperaTVStoreWindow.hpp"
#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include <omi/messages/CreateTVStoreWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/KeyInput.h>
#include <omi/messages/LoadURL.h>
#include "OperaEngine.hpp"
#include "OperaWindowManager.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"
#include "../helper/OperaKeyHelper.hpp"

TRACE_IMPLEMENT("opera/tvs_window");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaTVStoreWindow::OperaTVStoreWindow(OperaEngine& opera_engine,
                               int event_dispatch_priority)
    : OperaWindow(opera_engine, OperaWindow::window_name_tv_store, event_dispatch_priority)
    , m_window_type(OperaWindow::WindowType::window_tv_store)
{
    clearKeyset();
}

OperaTVStoreWindow::~OperaTVStoreWindow()
{
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
}

bool
OperaTVStoreWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    bool ok = false;
    using omi::msg::LoadURL;

    UString tvstore_url = url.c_str();

    if (url.find("appstore://open?") != cabot::String::npos)
    {
        tvstore_url = tvstore_url.getSubstring(16, tvstore_url.length() - 16);
    }

    clearKeyset();
    m_keyset.keyset_value = all_keyset;
    m_keyset.other_keys.push_back(operaKeyToNebulaKey(omi::common::Key::OMI_KEY_SUBTITLE));

    if (!m_window_handle.IsValid())
    {
        using omi::msg::CreateTVStoreWindow;
        auto result = m_opera_engine.post(CreateTVStoreWindow(tvstore_url.charArray(),
                                                              "Model/Vestel-VSTVB-MB100")).get();
        m_window_handle = getWindowHandle(result);
        updateOperaInputMethod();

        // Set background color as black to avoid blinking effect.
        setPreferenceValue("default_background_color", "#000000");
        setPreferenceValue("notify_scripts_closing_windows", "true");
        setRenderingResolution(rendering_resolution_1280x720);
    }

    LoadURL::Result result = m_opera_engine.post(LoadURL(m_window_handle, tvstore_url.charArray())).get();
    if (checkResult(result))
    {
        ok = true;
        m_current_url = tvstore_url.charArray();
    }
    return ok;
}

void
OperaTVStoreWindow::terminateCurrentApp()
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
OperaTVStoreWindow::type() const
{
    return m_window_type;
}

void
OperaTVStoreWindow::stop()
{
    clearKeyset();
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
    m_window_handle = OperaWindowHandle(-1);
    m_current_url.clear();
}

void OperaTVStoreWindow::updateOperaInputMethod()
{
    bool is_usb_keyboard_plugged = m_opera_engine.getWindowManager()->isUsbKeyboardPlugged();

    UString input_method;
    if (!is_usb_keyboard_plugged)
    {
        input_method = "virtual_keyboard";
        TRACE_INFO(("Setting Opera Keyboard Input Method to Virtual Keyboard.\n"));
    }
    else
    {
        input_method = "multitap";
        TRACE_INFO(("Setting Opera Keyboard Input Method to Multitap.\n"));
    }
    setPreferenceValue("input_method", input_method);
}
