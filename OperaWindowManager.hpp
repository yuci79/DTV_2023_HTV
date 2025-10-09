/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Handles management of the windows and key handling
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOWMANAGER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOWMANAGER_HPP_INCLUDED

#include "OperaEngine.hpp"
#include "OperaWindow.hpp"
#include "vewd_integration/core/WindowManagerActiveList.hpp"
#include <omi/Browser.h>
#include "../OperaWindowHandle.hpp"
#include "vewd_common/AnyWebBrowserNumberKeyHandler.hpp"
#include "vewd_common/AnyWebBrowserWindowManager.hpp"
#include "utilities_public/String.hpp"
#include <atomic>
#include <vector>
#include "../jspp_plugins/NativeOperaWindowManager.h"

#include "nebula/browser_interface/AppResolutionType.hpp"

static const unsigned int priority_max_value = 100;

class OperaWindow;
typedef std::vector<OperaWindow *> VectorOfOperaWindowPtr;
class OperaHbbTVWindow;
class OperaOpAppWindow;
class OperaCspWindow;
class OperaBrowserUIWindow;
class OperaBrowserWindow;
class OperaTVStoreWindow;
class OperaOipfWindow;
#ifdef USE_MEDIA_PLAYER_WINDOW
class OperaMediaPlayerWindow;
#else
class OperaDashPlayerWindow;
using OperaMediaPlayerWindow = OperaDashPlayerWindow;
#endif
class OperaLauncherWindow;

namespace omi
{
    namespace evt
    {
        class WindowFocusChanged;
    }
}

class OperaWindowManager : public AnyWebBrowserNumberKeyHandler
                         , public AnyWebBrowserWindowManager
{
public:
    /**
     * @brief OperaDialogWindowManager constructor.
     */
    OperaWindowManager(OperaEngine& opera_engine);

    /**
     * @brief OperaDialogWindowManager destructor.
     */
    virtual ~OperaWindowManager();

    /**
     * @brief Get the one and only OperaWindowManager.
     * @return OperaWindowManager object pointer
     */
    static OperaWindowManager* getWindowManager();

    /**
     * @brief Initialise resources.
     * 
     * @param media_resource_id A unique id that represents the resource to use for playing media.
     */
    void initialise(int media_resource_id);

    /**
      * @brief This method is called to notify of a browser process state change.
      * @param state The state of the browser process
      */
    void notifyBrowserProcessStateChange(BrowserProcessState state);
    
    /**
     * @brief Creates an Oipf window.
     * @return OperaWindow object pointer
     * @see OperaWindow
     */
    OperaWindow* createOipfWindow(cabot::String const& window_name = "GENERIC_OIPF", int event_dispatch_priority = priority_max_value);

    /**
     * @brief Creates a full-screen basic browser window.
     * @param window_name The name of the window
     * @param event_dispatch_priority Order for delivering events to an application. Lower values mean higher priority
     *        range[0...100]
     * @return OperaWindow object pointer
     * @see OperaWindow
     */
    OperaWindow* createGenericWindow(cabot::String const& window_name = "GENERIC_CSP", int event_dispatch_priority = priority_max_value);

    /**
     * @brief Creates a full-screen HbbTV window.
     * @return OperaWindow object pointer
     * @see OperaWindow
     */
    OperaHbbTVWindow& hbbTVWindow();

    /**
     * @brief Provides access to the HbbTV Operator Applications window.
     * @return The OpApp window.
     * @see OperaWindow
     */
    OperaOpAppWindow& opAppWindow() const;

    OperaCspWindow& cspWindow();

    /**
     * @brief Gets window dedicated to Opera TV Store application
     * @return OperaTVStoreWindow object
     * @see OperaWindow
     */
    OperaTVStoreWindow& tvStoreWindow();

    /**
     * @brief Gets window dedicated to UI application
     * @return OperaWindow object pointer
     * @see OperaWindow
     */
    OperaWindow& uiWindow();

    /**
     * @brief Gets window dedicated to OIPF application
     * @return OipfWindow object pointer
     * @see OperaWindow
     */
    OperaOipfWindow& oipfWindow();

    /**
     * @brief Gets window dedicated to launcher application
     * @return OipfWindow object pointer
     * @see OperaWindow
     */
    OperaLauncherWindow& launcherWindow();

    /**
     * @brief Gets window dedicated to playing media content (e.g. mpd).
     * @return OperaMediaPlayerWindow object pointer.
     * @see OperaWindow.
     */
    OperaMediaPlayerWindow& mediaPlayerWindow();

    /**
     * @brief Creates a full-screen Browser UI window.
     * @return OperaWindow object pointer
     * @see OperaWindow
     */
    OperaBrowserUIWindow& getBrowserUIWindow(cabot::String const& start_url);

    /**
     * @brief Destroys a specified window and releases any reserved resources.
     * @param window OperaWindow handle
     */
    void destroyWindow(OperaWindow* window);

    /**
     * @brief Send key to active OperaWindow. If not consumed, key is dispatched
     *        to other applications sorted according to the event_dispatch_pripority.
     * @param key_code The key to send
     * @param keypress_handling_reason The reason for handling this key press.
     * @return true, if the key is successfully sent to the browser,
     *         false, if the key is not being sent or handled
     */
    bool handleKeyInput(nebula::KeyCode key_code,
                        nebula::KeyPressHandlingReason keypress_handling_reason);


    /**
     * @brief Handle key press with new implementation of down/up keys.
     *
     * @param key the key code to submit to the browser core.     
     * @param key_released false if the key is just pressed and down key is simulated and 
     * true if the key is released and up key is simulated
     * @return true if key was accepted by the browser core and no further
     * processing of this key is required. Otherwise false if the key
     * was not accepted by the browser core - in this case the key may then be
     * passed on for further handling (for example by the UI).
     */    
    bool handleKeyInputWithDownUpKeys(nebula::KeyCode key_code,
                        nebula::KeyPressHandlingReason keypress_handling_reason,
                        bool key_released);

    /*
     * @brief Set specified window's keyset. Keysets are used for determining if window will
     *        handle a key or not.
     * @param window OperaWindow handle
     * @param keyset_value The value of the keyset which this DAE application will receive.
     * @param other_keys If the OTHER bit in the value property is set then this indicates those key events
     *        which are available to the browser which are not included in one of the constants defined
     *        in this class, If the OTHER bit in the value property is not set then this property is meaningless.
     */
    void setKeySet(OperaWindow& window, int keyset_value, std::vector<int> other_keys);

    /*
     * @brief Get specified window's keyset. Keysets are used for determining if window will
     *        handle a key or not.
     * @param window OperaWindow handle
     * @param keyset_value The value of the keyset which this DAE application will receive.
     * @param other_keys this indicates those key events which are available to the browser
     *        which are not included in one of the constants defined in OIPF.
     */
    void getKeySet(OperaWindow& window, int* keyset_value, std::vector<int>* other_keys) const;

    /*
     * @brief Set specified window's event dispatch priority. Priorities are used for determining which window will
     *        handle a key first if there are multiple windows able to handle that key.
     *        Lower values mean higher priority: A window with event capture priority of 0, will always get the events first
     *        if there is not any active window.
     * @param window OperaWindow handle
     * @param event_dispatch_priority The value of the event dispatch priority. Range should be between [0-100]
     */
    void setEventDispatchPriority(OperaWindow& window, int event_dispatch_priority);

    /*
     * @brief Get specified window's event dispatch priority.
     * @param window OperaWindow handle
     * @return The value of the event dispatch priority
     * */
    int getEventDispatchPriority(OperaWindow& window) const;

    /**
     * @brief Make the specified window or view visible on the display,
     *        or hide according to the 'visible' parameter
     * @param window OperaWindow handle
     * @param visible show window if true; otherwise hide the window.
     */
    void setVisible(OperaWindow& window, bool visible);

    /**
     * @brief Set the specified window active/inactive
     * @param window OperaWindow handle
     * @param active set active on the window if true; otherwise blur the window.
     */
    void setActive(OperaWindow& window, bool active);

    /**
     * @brief Reset an OperaWindow to a default state.
     * @param window OperaWindow handle.
     * @param clear_keyset A flag to indicate whether to set the keyset to default. Always clear
     * the keyset by default.
     */
    void reset(OperaWindow& window, bool clear_keyset = true);

    /**
     * @brief Load a webpage or application from the specified URL
     * @param window OperaWindow handle
     * @param url The URL of the webpage/application to load.
     * @return true if request is handled successfully; false, otherwise
     */
    bool loadUrl(OperaWindow& window, cabot::String const& url);

    /**
     * @brief Sets the named preference to given value.
     * @param window OperaWindow handle
     * @param key The key to set.
     * @param value The value to set for given key.
     * @return true if request is handled successfully; false, otherwise
     */
    bool setPreferenceValue(OperaWindow& window, UString const& key, UString const& value);

    /**
     * @brief Gets the named preference value
     * @param window OperaWindow handle
     * @param key The key to get.
     * @param value The value to get for given key.
     * @return true if request is handled successfully; false, otherwise
     */
    bool getPreferenceValue(OperaWindow& window, UString const& key, UString& value);

    /**
     * @brief Navigate the page-load history of the given window.
     * @param window OperaWindow handle
     * @param steps Number of history entries to navigate in history. Note that direction is
     *              decided according to the sign value of the parameter. Use negative value
     *              to go back.
     * @return true if request is handled successfully; false, otherwise
     */
    bool moveInHistory(OperaWindow& window, int steps);

    /**
     * @brief Returns the number of the entries present in the browsing history for the given window
     * @param window OperaWindow handle
     * @return count History count
     */
    int getHistoryCount(OperaWindow& window);

    /**
     * @brief Returns History index in the given window
     * @param window OperaWindow handle
     * @return index History index
     */
    int getCurrentHistoryIndex(OperaWindow& window);

    /**
     * @brief Check whether the window is visible or not
     * @param window OperaWindow handle
     * @return true if window is visible; otherwise, returns false.
     */
    bool isVisible(OperaWindow& window) const;

    /**
     * @brief Check whether the window is acive or not
     * @param window OperaWindow handle
     * @return true if window is active; otherwise, returns false.
     */
    bool isActive(OperaWindow& window) const;

    /**
     * @brief Gets the name of the window.
     * @param window OperaWindow handle
     * @param name Window name
     */
    void getName(OperaWindow& window, cabot::String& name) const;

    /**
     * @brief Gets the url of the window.
     * @param window OperaWindow handle
     * @param url The url loaded in the window
     */
    void getUrl(OperaWindow& window, cabot::String& url) const;

    /**
     * @brief prints the browser list and the active window list
     */
    void getWindowListAsName();

    /**
     * @brief Get list of windows
     * @return list of OperaWindow pointers
     */
    VectorOfOperaWindowPtr const& getWindows() const;

    /**
     * @brief Get active window
     * @return pointer to active OperaWindow handle.
     * @note If there is not any window created via WindowManager,
     *       null pointer will returned.
     */
    OperaWindow* getActiveWindow() const;

    /**
     * @brief Get active windows
     * @return vector of pointers to active OperaWindow handle.
     * @note If there is not any window created via WindowManager an empty list will be returned.
     */
    VectorOfOperaWindowPtr getActiveWindows() const;

    /**
     * @brief Get active window for media components.
     * @return pointer to active OperaWindow handle for media components.
     */
    OperaWindow* getActiveWindowForComponents() const;

    /**
     * @brief Get active window of specified name
     * @param name Name of the active window
     * @return pointer to OperaWindow handle.
     * @note If there is not any window created via WindowManager with the provided name,
     *       null pointer will returned.
     */
    OperaWindow* getActiveWindowByName(cabot::String const& name) const;

    /**
     * @brief Get window of specified name
     * @param name Name of the window
     * @return pointer to OperaWindow handle.
     * @note If there is not any window created via WindowManager with the provided name,
     *       null pointer will returned.
     */
    OperaWindow* getWindowByName(cabot::String const& name) const;

    /**
     * @brief Get window that corresponds to specified handle.
     * @param handle Handle of the window
     * @return pointer to OperaWindow handle.
     * @note If there is not any window created via WindowManager with the provided name,
     *       null pointer will returned.
     */
    OperaWindow* getWindowByHandle(OperaWindowHandle const& handle) const;

    /**
     * @brief Get window identifier of the specified window
     * @param window OperaWindow handle
     * @return OperaWindowHandle window identifier
     */
    OperaWindowHandle getWindowHandle(OperaWindow& window);

    /**
     * @brief Make the hbbTV,csp,tvStore and ui windows view visible on the display,
     *        or hide according to the 'visible' parameter
     * @param visible show window if true; otherwise hide the window.
     */
    void setVisibleAllWindows(bool visible);

    /**
     * @brief Terminate apps except UI and other default apps.
     */
    void terminateUserApps();

    /**
     * @brief Terminate by app type
     * 
     * @param app_type to terminate
     * @return whether successful
     */
    bool terminateApp(WebBrowserAppType const app_type);

    /**
     * @brief Start Opera TV Store application
     * @param tv_store_url Url of application
     *
     * @return true if application is successfully loaded, false otherwise
     */
    bool startTVStoreApp(cabot::String const& tv_store_url);

    bool startCspApp(cabot::String const& csp_url);

    void startBrowserUi(cabot::String const& start_url);

    void stopBrowserUi();

    void getUiApp(cabot::String& application_url);

    bool startUiApp(cabot::String const& application_url,
                    cabot::String const& overlay_application_url,
                    ApplicationResolution application_resolution
                    = application_resolution_default);

    bool reloadUiApp();

    void stopUiApp();

    void setVisibleUiApp(bool visible);

    /**
     * @brief Give focus to operator application so that it will receive key input events.
     *
     * @param focus If true, give focus to operator application, false, otherwise
     */
    void setFocusToOpApp(bool focus);

    /**
     * @brief Check if Opapp is focused and is in front
     *
     * @return true if OpApp is focused and is in front, false otherwise.
     */
    bool isOpAppFocused();    

    /**
     * @brief Starts an hbbtv application.
     * @param org_id The organisation that the application belongs to.
     * @param app_id The application to start.
     * @return true if the request to start the application was made.
     */
    bool startHbbTVApp(unsigned int org_id, unsigned short app_id);

    /**
     * @brief Starts an hbbtv application
     * @param application_url of the application to start
     * @return whether it was successful
     */
    bool startHbbTVApp(cabot::String const& application_url);

    /**
     * @brief Stops an hbbtv application.
     * @param launch_autostart_app If true, after terminating the current application, the
     * application indicated as autostart in the last obtained AIT table is launched. Always
     * attempt to launch the auto start application by default.
     */
    void stopHbbTvApp(bool launch_autostart_app = true);

    /**
     * @brief Check whether oipf window is supported or not
     * @return true if supported; false, otherwise.
     */
    bool isOipfWindowSupported();

    /**
     * @brief Starts an oipf application
     * @param application_url of the application to start
     * @return whether it was successful
     */
    bool startOipfApp(cabot::String const& application_url);

    /**
     * @brief Stops an oipf application
     */
    void stopOipfApp();

    /**
     * @brief Move window to the back of the z-order
     * @param window OperaWindow handle*
     * @return true if action is completed successfully; false, otherwise.
     */
    bool sendToBack(OperaWindow& window);

    /**
     * @brief Move window to the front of the z-order
     * @param window OperaWindow handle
     * @return true if action is completed successfully; false, otherwise.
     */
    bool bringToFront(OperaWindow& window);

    /*
     * @brief Places a window above another window in the window stack.
     * Modifies the z-order so that the window specified by |window| ends up
     * just above the window specified by |reference_window|.
     * @param window The handle of the window to place above the reference window.
     * @param reference_window The handle of the reference window.
     * @return true if action is completed successfully; false, otherwise.
     */
    bool moveAbove(OperaWindow& window, OperaWindow& reference_window);

    /*
     * @brief Places a window below another window in the window stack.
     * Modifies the z-order so that the window specified by |window| ends up
     * just below the window specified by |reference_window|.
     * @param window The handle of the window to place above the reference window.
     * @param reference_window The handle of the reference window.
     * @return true if action is completed successfully; false, otherwise.
     */
    bool moveBelow(OperaWindow& window, OperaWindow& reference_window);

    /*
     * @brief Set callback to watch application state change
     * @param callback pointer to ApplicationStateChangeCallback
     */
    void setApplicationStateChangeCallback(ApplicationStateChangeCallback* callback);

    /*
     * @brief Send application state change notification to the listener who sets
     * ApplicationStateChangeCallback by using setApplicationStateChangeCallback
     * @param handle Window handle
     * @param app_state Current application state
     */
    void notifyAppStateChange(OperaWindowHandle const& handle, ApplicationState app_state);

    /**
     * @brief Check whether any app/page is active
     * @return true, if there is an active application
     *         false, if no active application
     */
    bool isAppRunning() const;

    /**
     * @brief Check whether any app/page is active with the specified app type.
     * @param app_type the application type
     * @return true, if there is an active application
     *         false, if no active application
     */
    bool isAppRunning(WebBrowserAppType const app_type) const;

    /**
     * @brief Check active window for any running application
     *
     * @param active_win the active window
     *
     * @return true if an application is running in the specified window.
     */
    bool isActiveWindowAppRunning(OperaWindow *active_win) const;

    /**
     * @brief Get all running application types.
     *
     * @param first_match_only Return only the first found running application.
     *
     * @return Vector of running application types.
     */
    std::vector<WebBrowserAppType> runningAppTypes(bool first_match_only = false) const;
    
    /*
     * @brief Check whether number key is sent from remote controller or not.
     * @return true if number key is sent from remote controller; false, otherwise.
     */
    bool isNumberSentFromRC();

    /**
     * @brief Sets if keyboard is plugged or not
     * @param Keyboard attached value
     */
    void setUsbKeyboardStateAttached(bool attached);

    /**
     * @brief Check whether usb keyboard plugged
     * @return true if usb keyboard plugged
     */
    bool isUsbKeyboardPlugged() const;

    /**
     * @brief Control whether a window can be given focus.
     * @param window OperaWindow handle.
     * @param focusable true to allow the window to have focus,
     *                  false to prevent the window from having focus.
     */
    void setFocusable(OperaWindow& window, bool focusable);

    void setCurrentUrl(cabot::String url);

    void setApmStarted(bool apm_started);
    bool getApmStarted();

    /**
     * @brief Sets start url is opened
     * @param value : the new value
     */
    void setStarturlOpened(bool value);

    /**
     * @brief Checks start url is opened
     * @return true if start url is opened, false otherwise
     */
    bool isStarturlOpened() const;

    /**
     * @brief sets given windows visibility as visible parameter
     *
     * @param window string is page name
     * @param visible boolean value, True when making window visible, false otherwise
     */
    void setAppVisibilityWithGivenName(cabot::String const& window, bool visible);

    /**
     * @brief Update the list of active windows.
     *
     * External changes to windows may mean that the active window list needs
     * to be re-evaluated and possibly re-ordered.  This may also affect focus.
     */
    void updateActiveWindows();

    /**
     * @brief Updates the User-Agent preference of windows as appropriate.
     */
    void updateUserAgentPreference();

    /**
     * @brief Send the key to the window.
     *
     * @param window The window to send the key to.
     * @param key_code The key to send.
     */
    void sendKeyInput(const OperaWindow& window, nebula::KeyCode key_code);

    /**
     * @brief Send the key to the window with down key.(pressed)
     *
     * @param window The window to send the key to.
     * @param key_code The key to send.
     */
    void sendKeyDown(const OperaWindow& window, nebula::KeyCode key_code);

    /**
     * @brief Send the key to the window with up key (released).
     *
     * @param window The window to send the key to.
     * @param key_code The key to send.
     */    
    void sendKeyUp(const OperaWindow& window, nebula::KeyCode key_code);

    /**
     * @brief Starts an csp application to show advertisement on browser
     * @param ad_url of the advertisement to start
     * @return whether it was successful
     */
    bool startAdvertisementBrowser(cabot::String const& ad_url);

    /**
     * @brief Stops the advertisement browser
     */
    void stopAdvertisementBrowser();

    /**
     * @brief Launch Browser launcher.
     *
     * @param url URL of the launcher app that will be launched.
     *
     * @return true if URL is launch request is done, false otherwise.
     */
    bool startBrowserLauncher(cabot::String const& application_url);

    /**
     * @brief Terminates previously loaded Browser launcher.
     */
    void stopBrowserLauncher();

    /**
     * @brief Reload browser launcher.
     */
    void reloadBrowserLauncher();

    /**
     * @brief Change the visibility of Browser launcher.
     *
     * @param visible Make launcher visible if true, hide otherwise.
     */
    void setBrowserLauncherVisible(bool visible);

    /**
     * @brief Checks the focus of Browser launcher.
     *
     * @return true if Browser launcher is focus, false otherwise
     */
    bool isBrowserLauncherFocused();

    /**
     * @brief Checks the visibility of Browser launcher.
     *
     * @return true if Browser launcher is visible, false otherwise
     */
    bool isBrowserLauncherVisible();

    /**
     * @brief Give focus to the launcher window so that it will receive key input events.
     *
     * @param If true, give focus to the launcher, if false take focus from it.
     *
     * @return true on success, false otherwise.
     */
    bool setFocusToBrowserLauncher(bool focus);

    /**
     * @brief Launch a window hosting media content (e.g. mpd).
     *
     * @param url The url of the media to play.
     * @return true if the request to launch the media is successful, false otherwise.
     */
    bool startMediaPlayer(cabot::String const& url);

    /**
     * @brief Reset the media player back to a default state, i.e. before startMediaPlayer()
     * was called.
     */
    void resetMediaPlayer();

    /**
     * @brief Checks the current active window's type matches specified type.
     * @param window_type
     * @return true if the current active window type matches window_type
     */
    bool isCurrentActiveWindowType(OperaWindow::WindowType window_type) const;

    /**
     * @brief Gets the current active window's app type.
     * @return the app type
     */
    WebBrowserAppType getCurrentActiveWindowAppType() const;

    static WebBrowserAppType getWebBrowserAppType(OperaWindow* active_win);

public:
    // Implementing AnyWebBrowserWindowManager
    WindowType windowType(WindowId window_id) const override;

    bool isApplicationBroadcastRelated(WindowId window_id) const override;

    cabot::String getCurrentUrl() const override;

    cabot::String pageUrl(WindowId window_id) const override;

private:
    VectorOfOperaWindowPtr getActiveWindowsLocked() const;

    OperaWindow* getWindowByHandleLocked(OperaWindowHandle const& handle) const;
    OperaWindow* getActiveWindowLocked() const;
    void updateActiveWindowsLocked();
    void insertToListLocked(OperaWindow* window,
                            VectorOfOperaWindowPtr& window_list,
                            bool sort_by_priority_after_insertion = true);
    void insertToList(OperaWindow* window,
                      VectorOfOperaWindowPtr& window_list,
                      bool sort_by_priority_after_insertion = true);
    bool removeFromListLocked(OperaWindow* window,
                              VectorOfOperaWindowPtr& window_list);
    bool removeFromList(OperaWindow* window,
                        VectorOfOperaWindowPtr& window_list);
    void sortListByPriority();
    void updateFocus();
    bool isInWindowList(OperaWindow const* window);
    void stopCspApp();
    void stopTVStoreApp();
    const OperaWindow* findWindowToHandleKeyInput(nebula::KeyCode key_code,
                                        nebula::KeyPressHandlingReason keypress_handling_reason);

    void onWindowFocusChanged(omi::evt::WindowFocusChanged window_focus_changed);
    void destroyWindowList();
    void recreateWindows();

private: // Copy and assign not allowed.
    OperaWindowManager(OperaWindowManager const &);
    OperaWindowManager& operator=(OperaWindowManager const &);

private: // data members
    OperaWindow* m_ui_window;
    cabot::String m_ui_window_url;
    OperaEngine& m_opera_engine;
    bool m_is_oipf_window_supported;
    bool m_is_apm_started;
    typedef VectorOfOperaWindowPtr::iterator WindowIterator;
    VectorOfOperaWindowPtr m_window_list;
    WindowManagerActiveList m_active_window_list;
    std::atomic<ApplicationStateChangeCallback*> m_app_state_change_callback;
    bool m_is_number_key_sent_from_rc;
    bool m_usb_keyboard_plugged;
    cabot::String m_current_url;
    bool m_is_start_url_opened;
    bool m_is_repeat_key_active;

    //mutable FrostMutex m_list_mutex;

    static OperaWindowManager* m_the_window_manager;
};

#endif
