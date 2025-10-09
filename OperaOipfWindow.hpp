/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Handles oipf window specific functionalities
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAOIPFWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAOIPFWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"

class OperaEngine;

class OperaOipfWindow : public OperaWindow
{
public:    
    OperaOipfWindow(OperaEngine& opera_engine,
                       cabot::String const& window_name,
                       OperaWindow::WindowType window_type,
                       int event_dispatch_priority);
    
    virtual ~OperaOipfWindow();
    
    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
private:
    OperaWindow::WindowType m_window_type;
    unsigned int m_application_id;
};
#endif
