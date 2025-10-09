/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOW_HPP_INCLUDED

#include "vewd_integration/core/AnyOperaWindow.hpp"
#include "OperaWindowTypes.hpp"  
#include <omi/Browser.h>
#include "../OperaWindowHandle.hpp"
#include "vewd_common/KeySetUtilities.hpp"
#include "nebula/adaptation_layer/nebula_av_control.h"
#include "nebula/browser_interface/NebulaKeyCodes.hpp"
#include <vector>

class OperaEngine;
class UString;

namespace cabot
{
    class String;
}

class OperaWindow : public AnyOperaWindow
{
public:
    typedef OperaWindowType WindowType;
    
    enum RenderingResolution
    {
        rendering_resolution_default,
        rendering_resolution_1280x720,
        rendering_resolution_1920x1080
    };

    static const char* window_name_browser_ui;
    static const char* window_name_csp;
    static const char* window_name_hbbtv;
    static const char* window_name_tv_store;
    static const char* window_name_ui;
    static const char* window_name_opapp;
    static const char* window_name_oipf;
    static const char* window_name_launcher;
    static const char* window_name_overlay;
    static const char* window_name_media_player;

    OperaWindow(OperaEngine& opera_engine, cabot::String window_name, int event_dispatch_priority);

    virtual ~OperaWindow();
    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default) = 0;

    virtual void terminateCurrentApp() = 0;
    virtual WindowType type() const = 0;

    OperaWindowHandle handle() const;
    void setEventDispatchPriority(int event_dispatch_priority);
    int getEventDispatchPriority() const;
    void setKeySet(int keyset_value, const std::vector<int>& other_keys);
    void getKeySet(int* keyset_value, std::vector<int>* other_keys) const;
    void setVisible(bool visible);
    void setActive(bool active);

    /**
     * @brief Indicates whether the window has focus.
     * @param focus true if the window has focus.
     */
    void setFocus(bool focus);

    /**
     * @brief Control whether a window can be given focus.
     * @param window OperaWindow handle.
     * @param focusable true to allow the window to have focus,
     *                  false to prevent the window from having focus.
     */
    void setFocusable(bool focusable);

    bool sendToBack();
    bool bringToFront();
    bool moveAbove(OperaWindowHandle const& reference_window_handle) const;
    bool moveBelow(OperaWindowHandle const& reference_window_handle) const;
    bool setPreferenceValue(UString const& key, UString const& value);
    bool getPreferenceValue(UString const& key, UString& value);
    bool moveInHistory(int steps);
    int getHistoryCount();
    int getCurrentHistoryIndex();
    bool isVisible() const;
    bool isActive() const;
    void setRenderingResolution(RenderingResolution resolution);
    cabot::String const& getCurrentPageUrl() const;
    void setCurrentPageUrl(cabot::String const& url);
    virtual void updateOperaInputMethod();

    /**
     * @brief Resets the window to a default state.
     * @param clear_keyset A flag to indicate whether to set the keyset to default.
     */
    virtual void reset(bool clear_keyset);

    /**
     * @brief Marks whether this window is currently running a Teletext app.
     * @param teletext_app True if running a teletext app, false if not.
     */
    void setTeletextAppRunning(bool teletext_app);

    /**
     * @return Returns whether this window is currently running a Teletext app.
     */
    bool isTeletextAppRunning() const;

    /**
     * @brief Updates the Window's useragent preference.
     */
    void updateUserAgentPreference();

    /**
     * @brief Gets the rectangular region of the window.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the window has a rectangular region set, false otherwise.
     */
    bool getPosition(double& x,
                     double& y,
                     double& width,
                     double& height) const;

    /**
     * @brief Sets the rectangular region of the window.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the window rectangular region is set, false otherwise.
     */
    bool setPosition(double x,
                     double y,
                     double width,
                     double height);

protected:
    void setRenderingResolution(int width, int height);

public:
    // Implementing AnyOperaWindow
    bool isFocusable() const override;
    bool isFocused() const override;
    bool isValid() const override;
    cabot::String name() const override;
    bool shouldHandleKey(nebula::KeyCode key_code,
                         nebula::KeyPressHandlingReason keypress_handling_reason) const override;
    bool broadbandMediaAllowed() const override;

protected:
    static const int lowest_event_dispatch_priority_value = 0; // Lowest value means highest priority
    static const int highest_event_dispatch_priority_value = 100; // Highest value means lowest priority
    static const unsigned int default_broadcast_related_keyset = 0; // per OIPF DAE 7.2.5
    static const unsigned int default_broadcast_independent_keyset = keyset_red
        | keyset_green | keyset_yellow | keyset_blue | keyset_navigation
        | keyset_vcr | keyset_scroll | keyset_info | keyset_numeric
        | keyset_alpha; // per OIPF DAE 7.2.5
    static const unsigned int all_keyset = default_broadcast_independent_keyset | keyset_other;

protected:
    virtual void clearKeyset();
    void setDefaultUserAgentString();
    void setTikTokUserAgentString();
    bool isKeyInKeySet(nebula::KeyCode key_code) const;

    /**
     * @brief Get the information from the file.
     *
     * @param file path.
     * @param content information from the file returned here.
     *
     * @return true is successful.
     */
    bool getFileContent(const char* file_path, char *content);

protected:
    OperaEngine& m_opera_engine;
    OperaWindowHandle m_window_handle;
    cabot::String m_current_url;
    cabot::String m_window_name;
    bool m_active;
    bool m_visible;
    bool m_focusable;
    int m_event_dispatch_priority;
    bool m_is_txt_app_running;

    struct KeySet
    {
        int keyset_value;
        std::vector<int> other_keys;
    } m_keyset;

private:
    bool m_focused;

    NEBULA_DisplayWindow m_nebula_window_rect;
    bool m_nebula_window_rect_set;
};
#endif
