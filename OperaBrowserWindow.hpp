/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief
 *
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"
class OperaEngine;

class OperaBrowserWindow : public OperaWindow
{
public:
    OperaBrowserWindow(OperaEngine& opera_engine,
                       cabot::String const& window_name,
                       OperaWindow::WindowType window_type,
                       int event_dispatch_priority);

    virtual ~OperaBrowserWindow();

    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
    void stop();

private:
    OperaWindow::WindowType m_window_type;
};
#endif
