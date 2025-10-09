/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines CSP browser windows
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERACSPWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERACSPWINDOW_HPP_INCLUDED

#include "OperaLinearIPServiceWindow.hpp"

class OperaEngine;

class OperaCspWindow : public OperaLinearIPServiceWindow
{
public:
    OperaCspWindow(OperaEngine& opera_engine,
                   int event_dispatch_priority);

    virtual ~OperaCspWindow();

    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
    void stop();

private:
    OperaWindow::WindowType m_window_type;
};
#endif
