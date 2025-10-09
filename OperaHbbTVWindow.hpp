/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief
 *
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVWINDOW_HPP_INCLUDED

#include "OperaLinearIPServiceWindow.hpp"

#include <omi/messages/CreateHbbTVWindow.h>

class OperaHbbTVWindow : public OperaLinearIPServiceWindow
{
public:
    OperaHbbTVWindow(OperaEngine& opera_engine, int event_dispatch_priority);

    virtual ~OperaHbbTVWindow();

    /**
     * @brief Loads an hbbtv application.
     * @param org_id The organisation that the application belongs to.
     * @param app_id The application to start.
     * @return true if the request was successfully made to load the application.
     */
    bool loadApp(unsigned int org_id, unsigned short app_id);
    void onConnectionStatusChange(bool connected);
    void onCicamStatusChange(bool available);

    void setRestartAfterTermination(bool restart_flag);
    void setBroadcastRelated(bool broadcast_related);
    bool broadcastRelated() const;
    cabot::String const& getAutoStartUrl();
    void setAutoStartUrl(cabot::String const& url);

    /**
     * @brief Overrides the focus of the window.
     * @param override Whether to override the focus.
     * @param focusable The value to override to.
     */
    void setFocusableOverride(bool override, bool focusable);

    /** Implementing OperaWindow interface */
    void reset(bool clear_keyset) override;

    bool loadUrl(cabot::String const& url,
                 RenderingResolution resolution = rendering_resolution_default) override;

    /**
     * @brief return boolean to be propagated out, 
     * @return whether terminate was actually called. 
     */
    bool terminateCurrentAppWithResult();

    void terminateCurrentApp() override;
    OperaWindow::WindowType type() const override;
    bool isFocusable() const override;
    bool shouldHandleKey(nebula::KeyCode key_code,
                         nebula::KeyPressHandlingReason keypress_handling_reason) const override;

    /**
     * @brief Interact with the browser to create the window handle.
     */
    void createWindow();

private:
    bool m_restart_after_termination;
    bool m_broadcast_related;
    cabot::String m_auto_start_url;
    bool m_focusable_override;
    bool m_focusable_override_value;
};
#endif
