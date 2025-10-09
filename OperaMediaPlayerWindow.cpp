/*
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaMediaPlayerWindow class.
 */
#include "OperaMediaPlayerWindow.hpp"

#include "vewd_integration/helper/OperaSdk4Utilities.hpp"

#include <omi/messages/CreateMediaPlayerWindow.h>
#include <omi/messages/MediaPlayerLoad.h>
#include <omi/messages/MediaPlayerPlay.h>

#include <omi/messages/DestroyWindow.h>
#include <omi/messages/StackAtBottom.h>

#include "OperaEngine.hpp"

#include "utilities_public/String.hpp"
#include "utilities_public/UString.hpp"

#include "utilities_debug/trace.h"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

TRACE_IMPLEMENT("opera/mediaplayerwindow");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

OperaMediaPlayerWindow::OperaMediaPlayerWindow(
        OperaEngine& opera_engine, int event_dispatch_priority, int resource_id)
    : OperaLinearIPServiceWindow(opera_engine, OperaWindow::window_name_media_player,
                                 event_dispatch_priority)
    , m_resource_id(resource_id)
{
    clearKeyset();
}

OperaMediaPlayerWindow::~OperaMediaPlayerWindow()
{
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle));
}

bool
OperaMediaPlayerWindow::loadUrl(cabot::String const& url, RenderingResolution resolution)
{
    CABOT_UNUSED_PARAM(resolution);

    TRACE_INFO(("%s(url=%s)\n", __func__, url.charArray()));
    bool ok = false;

    if (!m_window_handle.IsValid())
    {
        using omi::msg::CreateMediaPlayerWindow;
        const bool visible = false;
        const bool focus = false;
        const auto bounds = omi::Rect();
        const auto media_resource_user_mode = omi::common::MediaResourceUserMode::IPTV;
        const auto result = m_opera_engine.post(
            CreateMediaPlayerWindow(visible, focus, bounds,
                                    media_resource_user_mode)
        ).get();

        m_window_handle = getWindowHandle(result);

        setRenderingResolution(rendering_resolution_1280x720);
        setDefaultUserAgentString();

        setInitialWindowPosition();
    }

    m_current_url = url;

    using omi::msg::MediaPlayerLoad;
    const std::string mime_type;
    const double start_position = 0;
    const int iptv_resource_id = m_resource_id;
    const auto result = m_opera_engine.post(
        MediaPlayerLoad(m_window_handle, url.c_str(), 
                        mime_type, start_position, iptv_resource_id)
    ).get();

    if (checkResult(result))
    {
        using omi::msg::MediaPlayerPlay;
        // If play fails, it is assumed that an omi::evt::MediaPlayerError will be generated,
        // as MediaPlayerPlay::Result does not contain any error information.
        m_opera_engine.post(MediaPlayerPlay(m_window_handle)).get();

        ok = true;
    }

    using omi::msg::StackAtBottom;
    m_opera_engine.post(StackAtBottom(m_window_handle)).get();
    
    return ok;
}

void
OperaMediaPlayerWindow::terminateCurrentApp()
{
    if (m_window_handle.IsValid())
    {
        const auto url = m_current_url;
        const auto terminated_window = m_window_handle;
        destroy();
        const bool app_creation_error = false;
        m_opera_engine.onAppTerminated(m_window_handle, terminated_window, url,
                                       app_creation_error, WebBrowserAppType::media_player);
    }
}

OperaWindow::WindowType
OperaMediaPlayerWindow::type() const
{
    return OperaWindow::WindowType::window_media_player;
}

void
OperaMediaPlayerWindow::destroy()
{
    TRACE_INFO(("%s - post destroy window begin (handle = '%s')\n", 
        __func__,
        m_window_handle.str().c_str()));
    m_opera_engine.post(omi::msg::DestroyWindow(m_window_handle)).get();
    TRACE_INFO(("%s - post destroy window end (handle = '%s')\n", 
        __func__,
        m_window_handle.str().c_str()));
        
    m_window_handle = OperaWindowHandle(-1);
    m_current_url.clear();
}
