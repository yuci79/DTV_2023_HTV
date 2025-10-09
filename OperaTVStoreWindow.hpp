/*
 * Copyright 2015 Vestel.  All rights reserved.
 *
 * @file
 * @brief
 *
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERATVSTOREWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERATVSTOREWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"
class OperaEngine;

class OperaTVStoreWindow : public OperaWindow
{
public:
    OperaTVStoreWindow(OperaEngine& opera_engine,
                   int event_dispatch_priority);

    virtual ~OperaTVStoreWindow();

    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
    void updateOperaInputMethod();
    void stop();

private:
    OperaWindow::WindowType m_window_type;
};
#endif
