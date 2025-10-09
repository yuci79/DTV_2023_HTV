/*
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the OperaMediaPlayerWindow class.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAMEDIAPLAYERWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAMEDIAPLAYERWINDOW_HPP_INCLUDED

#include "OperaLinearIPServiceWindow.hpp"

class OperaEngine;

/**
 * @brief A window to launch media content (e.g. mpd, m3u8, mp3).
 */
class OperaMediaPlayerWindow : public OperaLinearIPServiceWindow
{
public:
    /**
     * @brief Construct a new Opera Media Player Window object.
     * 
     * @param opera_engine Used to post messages / signal events.
     * @param event_dispatch_priority Priority of the window.
     * @param resource_id A unique id that represents the resource to use for playing media.
     */
    OperaMediaPlayerWindow(OperaEngine& opera_engine, int event_dispatch_priority,
                           int resource_id);

    virtual ~OperaMediaPlayerWindow();

public: // OperaWindow.
    bool loadUrl(cabot::String const& url,
                 RenderingResolution resolution = rendering_resolution_default) override;

    void terminateCurrentApp() override;
    OperaWindow::WindowType type() const override;

private:
    void destroy();

private:
    const int m_resource_id;
};
#endif
