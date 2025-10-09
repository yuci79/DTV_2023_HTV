/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief
 *
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERUIWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERUIWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"

class OperaBrowserUIWindow : public OperaWindow
{
public:
    OperaBrowserUIWindow(OperaEngine& opera_engine,
                       cabot::String const& start_url,
                       int event_dispatch_priority,
                       unsigned int plane_height);

    virtual ~OperaBrowserUIWindow();

    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
    void stop();
};
#endif
