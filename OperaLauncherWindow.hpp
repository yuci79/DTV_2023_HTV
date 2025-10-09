/*
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Handles launcher window specific functionalities
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERALAUNCHERWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERALAUNCHERWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"

class OperaEngine;

class OperaLauncherWindow : public OperaWindow
{
public:    
    OperaLauncherWindow(OperaEngine& opera_engine,
                    int event_dispatch_priority);
    
    virtual ~OperaLauncherWindow();
    
    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();

    virtual OperaWindow::WindowType type() const;

    /**
     * @brief Interact with the browser to create the window handle.
     */
    void createWindow();
    
    /**
     * @brief Reload browser launcher.
     */
    void reloadCurrentApp();

private:
    OperaWindow::WindowType m_window_type;
    unsigned int m_application_id;
};
#endif
