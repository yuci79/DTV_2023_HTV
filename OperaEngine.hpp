/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief The entry point for the Opera SDK. Handles initialisation, start
 *        and stop of the engine, handles events.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAENGINE_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAENGINE_HPP_INCLUDED

#include "config/include_opapp_support.h"
#include "config/include_hoteltv.h"

#include "config/using_sdk420.h"
#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include "config/novatek_platform.h"

#include "config/vewd_integration/enable_broadcast_resource_management.h"
#include "config/vewd_integration/use_media_player_window.h"

#include "eclipse_src/core/Active.hpp"
#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "nebula/adaptation_layer/nebula_channel_info.h"
#include "nebula/adaptation_layer/nebula_async_message_callback.h"
#include "nebula/adaptation_layer/AnyAvComponentController.hpp"
#include "nebula/browser_interface/AppType.hpp"
#include "nebula/browser_interface/AppResolutionType.hpp"
#include "nebula/browser_interface/ConnectionType.hpp"
#include "nebula/browser_interface/NebulaKeyCodes.hpp"
#include "nebula/browser_interface/AnyWebBrowserAitHandler.hpp"
#include "nebula/browser_interface/OperatorApplicationConfiguration.hpp"
#include "nebula/browser_interface/WebBrowserCookie.hpp"
#include "nebula/core/ait_hub/AitSection.hpp"

#include "OperaEventProcessor.hpp"
#include <omi/events/AITUpdated.h>
#include <omi/events/BrowserMemoryEvent.h>
#if defined USING_SDK422 || defined USING_SDK423
#include <omi/events/BrowserServiceFatalError.h>
#endif
#include <omi/events/AITUpdated.h>
#include <omi/events/NewWindowBlocked.h>
#include <omi/events/EGLContextLost.h>
#include "omi/common/MediaObject.h"
#include "omi/common/TrackType.h"
#include "vewd_common/KeySetUtilities.hpp"
#include "omi/messages/SetMediaComponentsPreferences.h"
#include "vewd_integration/helper/CursorPosition.hpp"
#if defined USING_SDK422 || defined USING_SDK423
#include "vewd_integration/restart/OperaBrowserRestart.hpp"
#include "vewd_integration/restart/OperaBrowserRestartTestTool.hpp"
#endif
#ifndef NOVATEK_PLATFORM
#ifdef USING_SDK423
#include <omi/events/WebAudioCreated.h>
#include <omi/events/WebAudioDeleted.h>
#endif
#endif

#include <omi/Browser.h>

#include "utilities_public/ExplodingBoolean.hpp"
#include "utilities_public/FrostMutex.hpp"
#include "utilities_public/FrostReadWriteMutex.hpp"
#include "utilities_private/cabot_timer.h"
#include "../OperaWindowHandle.hpp"
#include "BrowserEventPublisher.hpp"
#include "OperaHbbTVParentalRatingChecker.hpp"
#include "OperaHbbTVAppKeyset.hpp"
#include "OperaHbbTVUIDialogHelper.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class AnyWebBrowserEventListener;
class AnyWebBrowserProgressIndicator;
class AnyWebBrowserUserAgentProvider;
class BrowserUiConfiguration;
struct MouseEventData;
class OperaBrowserCore;
class OperaDialogWindowManager;
class OperaWindowManager;
class OperaImeManager;
class OperaHbbTVBrowserEventPublisher;
class OperaMediaPlayerEventPublisher;
class UString;
class OperaHandsetConfiguration;
class OperaHandsetIntegration;

class OpAppFileProvider;
class OperaOpAppBrowserEventPublisher;
class OperaMediaResourceManager;
class OperaOipfGlobalDrmClient;
class OperaOipfBrowserEventPublisher;

namespace omi
{
class Browser;
class Handle;
namespace evt
{
class AITParsingStatus;
class BeforeURLRequest;
class CertificateError;
class HbbTVApplicationVisibilityChanged;
class HbbTVApplicationStarted;
class HbbTVApplicationNotStarted;
class HbbTVApplicationStopped;
class HbbTVAITAppLoadRequest;
class HbbTVAppURLLoadRequest;
class HbbTVBroadcastRelatedStatusChanged;
class HbbTVKeysetChanged;
class LoadingStarted;
class LoadingFinished;
class UnhandledKeyInput;
class VirtualKeyboardVisibilityChanged;

class OIPFApplicationStarted;
class OIPFApplicationStopped;
class OIPFApplicationStartFailed;
class OIPFApplicationActivated;
class OIPFApplicationDeactivated;
class RequestQuotaPermission;
class FullscreenModeRequest;
class HbbTVApplicationLoadTimedOut;
class ParentalControlQueryEvent;
class TVStoreWindowClosed;
class ScriptRequestCloseWindow;
class WindowDestroyed;
class CICAMFileRequested;
class URLChanged;
class MediaObjectChanged;
class BrowserGPUMemoryEvent;
class RendererProcessDied;

#ifdef INCLUDE_OPAPP_SUPPORT
class OpAppApplicationStartResult;
class OpAppApplicationStopped;
class OpAppStateChanged;
class OpAppKeySetChanged;
#ifdef USING_SDK420
class OpAppApplicationsDiscovered;
class OpAppApplicationsNotDiscovered;
class OpAppApplicationInstallRequest;
#endif
#ifdef USING_SDK423
class OpAppDiscoveryServiceBroadbandDiscoveryStarted;
class OpAppDiscoveryServiceBroadbandDiscoveryPartialResult;
class OpAppDiscoveryServiceInstallRequest;
class OpAppDiscoveryServiceAvailableApplications;
#endif
class OpAppReplaceUiElements;
#endif

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
class BroadcastResourceGranted;
class BroadcastResourceReleaseRequested;
#endif

#ifdef USE_MEDIA_PLAYER_WINDOW
class MediaPlayerError;
class MediaPlayerLoadFinished;
class MediaPlayerPlaybackRevoked;
#endif
}
}

namespace vewd_integ
{
    class HbbTVApplicationStopped;
}

class OperaEngine : private Active,
                    public AnyAvComponentController
{
public:
    OperaEngine();
    ~OperaEngine();

    /**
     * @brief Initialise the browser engine.
     *        This method should be called before any other method can be called.
     *        Failure to comply will result in undefined behavior (possibly
     *        a crash)
     * @param hbbtv_event_publisher focuses on publishing all hbbtv related events and therefore
     * carries app and org id.
     * @param media_player_event_publisher publishes Media Player events.
     * @param opapp_event_publisher Publishes operator application-related events.
     * @param opapp_configuration_provider Supplies operator application configuration.
     * @param ait_handler utilised within AitUpdated.
     * @param hbbtv_application_profile a bit mask representing the application profile supported
     * (see the CreateHbbTvWindow ApplicationProfile enum).
     * @param hbbtv_profile integer representing the HbbTV Profile supported (See the
     * omi PlatformProfile enum).
     * @return true if the initialisation is successful, false otherwise
     * 
     * @note Must call terminate() before destroying.
     */
    ExplodingBoolean initialise(
                        AnyWebBrowserUserAgentProvider* user_agent_provider,
                        BrowserEventPublisher*  event_publisher,
                        OperaHbbTVBrowserEventPublisher* hbbtv_event_publisher,
                        OperaMediaPlayerEventPublisher* media_player_event_publisher,
                        OperaOpAppBrowserEventPublisher* opapp_event_publisher,
                        AnyWebBrowserOpAppConfigurationProvider const* opapp_configuration_provider,
                        AnyWebBrowserAitHandler* ait_handler,
                        std::uint16_t hbbtv_application_profile,
                        int hbbtv_profile,
                        OperaOipfBrowserEventPublisher* oipf_event_publisher);

    bool initialisationCompleted() const;
    bool initialisationCompletedLocked() const;

    /**
     * @brief Terminate the engine.
     * 
     * The engine cannot be used after this method has been called.
     * 
     * @note It is important that this method is called from the same thread as initialise().
     *       (The SDK 4.22 browser creates a new process when omi::Browser is created.  If the
     *       thread that creates this object terminates before it is destroyed, then the browser
     *       will shut down abnormally.)
     */
    void terminate();
    
    /**
     * @brief Return a handle to Opera internal object
     * @return The Opera internal instance
     */
    omi::Browser* opera();

    /**
     * @brief Start the engine
     * @return true if the engine is started successfully, false otherwise
     */
    bool start();

    /**
     * @brief Stop the engine
     */
    void stop();

    /**
     * @brief Open a page in a standard window
     * @param url The URL of the page to open
     * @param parse_ait_xml true if the URL will return AIT XML which the browser must parse
     * @param broadcast_dependent whether the channel information should be associated with request
     * @param hbbtv_application whether to launch in a hbbtv application (this will be checked after parse_ait_xml)
     * @param hbbtv_service inform the browser this application relates to an HbbTV service
     * @return true if successful, false otherwise
     */
    bool openUrl(cabot::String const& url,
                bool parse_ait_xml,
                bool broadcast_dependent,
                bool hbbtv_application,
                bool hbbtv_service,
                bool allow_auto_transit = true);

    /**
     * @brief Open a page in the HbbTV window
     *
     * @param xml_ait_url Url of the XML AIT to parse.
     *
     * @return true if successful, false otherwise
     */
    bool openDvbIXmlAit(cabot::String const& xml_ait_url);

    /**
     * @brief Create a page as a portal application
     * @param url The URL of the page to open
     * @param hbbtv_service inform the browser this application relates to an HbbTV service
     * @return true if successful, false otherwise
     */
    bool createHbbTVApp(cabot::String const& url, bool hbbtv_service);

    /**
     * @brief Access the operator application browser configuration.
     * 
     * @param configuration The returned configuration.
     * @return true if there is a valid configuration, false otherwise.
     */
    bool getOpAppConfiguration(
                nebula::OperatorApplicationConfiguration & configuration) const;
    
    /**
     * @brief Controls whether the browser is responsible for the discovery
     *        of operator applications.
     *
     * @param enabled true to enable browser support, where possible.
     * @return true if browser discovery is enabled, false otherwise.
     */
    bool setOpAppDiscoveryEnabled(bool enabled);

    /**
     * @return true if the browser is responsible for the discovery
     *         of operator applications, false otherwise.
     */
    bool isOpAppDiscoveryEnabled() const;

    /**
     * @brief Start the discovery process for operator applications.
     *
     * @param configuration Information for control of the discovery process.
     * @return true if the discovery process was started successfully,
     *         false if an error occurred.
     */
    bool startOpAppDiscovery(
                nebula::OperatorApplicationDiscoveryProperties const & configuration);


    /**
     * @brief Stop the discovery process for operator applications.
     *
     * @return true if the discovery process was stopped successfully, false if
     *         an error occurred.
     */
    bool stopOpAppDiscovery();

    /**
     * @brief Uninstall OpApp.
     * 
     * @param application_id Identifies the application. 
     * @param organisation_id Identifies the organisation the application
     *                        belongs to.
     * 
     * @return true if the request to uninstall the OpApp was submitted
     *         successfully.
     */
    virtual bool uninstallOpApp(
                std::uint16_t application_id, std::uint32_t organisation_id);

    /**
     * @brief Providing the NIT/BAT FQDN list to the browser.
     *
     * @param uris The list of discovery uris.
     * @return true if the stored list was updated.
     */
    void setOperatorApplicationDiscoveryUris(
                std::vector<cabot::String> const & uris);

    /**
     * @brief Start an operator application.
     *
     * @param application Properties of the operator application to start.
     * @return true if the request to start the application was submitted
     *         successfully.
     */
    bool startOpApp(nebula::OperatorApplicationProperties const & application);

    /**
     * @brief Stop the running operator application.
     *
     * @return true if the request to stop the running application was submitted
     *         successfully, false if there is no running application.
     */
    bool stopOpApp();

    /**
     * @brief Suspend currently running operator application.
     */
    void suspendOpApp();

    /**
     * @brief Resume previously suspended operator application.
     *        When resumed, application should continue from the state before it is suspended.
     */
    void resumeOpApp();

    /**
     * @brief Query if operator application is suspended.
     *
     * @return true if suspended, false otherwise.
     */
    bool isOpAppSuspended() const;

    /**
     * @brief Change the state of the running operator application.
     *
     * @param new_state The new operator application state.
     * @return true if the request to change the application state was submitted
     *         successfully, false if the transition failed or there is no
     *         running application.
     */
    bool setOpAppState(nebula::OperatorApplicationState new_state);

    /*
     * @brief Change the context of the running operator application.
     *
     * @param query_url Full URL or a query string, with or without a leading
     *                  '?'. Only queries for sloc and lloc are relevant.
     * @return true if the request to change the application context was
     *         submitted successfully, otherwise false.
     */
    bool setOpAppContext(const cabot::String& query_url);

    /**
     * @brief Send a key to the running operator application.
     *
     * @param key_code The key to send.
     * @param keypress_handling_reason The reason for sending the key.
     *
     * @return True if the key was sent to the operator application, false otherwise.
     */
    bool handleOpAppKeyInput(nebula::KeyCode key_code,
                             nebula::KeyPressHandlingReason keypress_handling_reason);

    /**
     * @brief Clear files created during OpApp installation by the browser.
     */
    void clearOpAppFiles();

    /**
     * @brief checks if any digital teletext type application exists in AIT subtable
     * @return true if AIT subtable includes teletext type HbbTV Application
     * false otherwise.
     */
    virtual bool isTeletextAvailable();

    /**
     * @brief checks if any digital teletext type application is started
     * @return true if teletext type HbbTV Application is started
     * false otherwise.
     */
    virtual bool isTeletextApplicationStarted() const;

    /**
     * @brief starts HbbTV Application with usage_type USAGE_DIGITAL_TELETEXT_APP
     * @return true if successful, false otherwise.
     */
    virtual bool startTeletext();

    /**
     * @brief stops HbbTV Application with usage_type USAGE_DIGITAL_TELETEXT_APP.
     *
     * @param launch_autostart_app If true, after terminating the digital teletext application,
     * the application indicated as autostart in the last obtained AIT table is launched.
     *
     * @return true if successful, false otherwise.
     */
    virtual bool stopTeletext(bool launch_autostart_app);

    /**
     * @brief Gets URL of UI application
     * @param application_url String buffer will be populated with URL of UI application
     */
    void getUIApp(cabot::String& application_url);

    /**
     * @brief Open an independent application page.
     * @param application_url This url will be opened as the main application
     * @param overlay_application_url This url will be opened as overlay application.
     * @param application_resolution Screen resolution supported by main application.
     *
     * @return true on success, false otherwise.
     */
    bool createUIApp(cabot::String const & application_url,
                     cabot::String const & overlay_application_url,
                     ApplicationResolution application_resolution
                     = application_resolution_default);

    /**
     * @brief Start the Opera's browser UI
     * @param start_url The start page
     * @return true if successful, false otherwise
     */
    bool startTvBrowser(cabot::String const& start_url);

    /**
     * @brief Stop the Opera's browser UI
     * @return true if successful, false otherwise
     */
    bool stopTvBrowser();

    /**
     * @brief Reset the Opera's browser UI current tab
     */
    void resetTvBrowserPage();

   /*
     * @brief Query current open internet url
     *
     * @param url current open internet url
     * @return true on success, otherwise false
     */
    bool tvBrowserUrl(cabot::String & url);

    /**
     * @brief Get the running status of the browser UI
     * @return true if successful, false otherwise
     */
    bool isTvBrowserRunning() const;

    /**
     * @brief Called upon exit request from browser UI
     */
    void onTvBrowserExitRequested();

    /**
     * @brief Checks whether exit requested from browser UI
     * @return true if exit requested, false otherwise
     */
    bool isTvBrowserExitRequested() const;

    /*
     * @brief Returns true if any special app being started.
     */
    bool isSpecialAppStarting() const;

    /*
    * @brief Sets a special app starting
    * @param status true if starting, false otherwise
    */
    void setSpecialAppStarting(bool status);

    /*
     * @brief Returns true if any  external source is selected.
     */
    bool isExternalSourceSelected() const;

    /*
    * @brief Selects an external source
    * @param selected true if external source is selected, false otherwise
    */
    void setExternalSourceSelected(bool selected);    

    /**
     * @brief Get the running status of the Opera TV Store application
     * @return true if successful, false otherwise
     */
    bool isTvStoreAppRunning() const;

    /**
     * @brief Returns true if Opera TV Store application being restarted
     */
    bool isTvStoreAppRestarting();

    /**
     * @brief Send key to browser
     * @param key_code The key to send
     * @return true, if the key is successfully sent to the browser,
     *         false, if the key is not being sent or handled
     */
    bool handleKeyInput(nebula::KeyCode key_code);

    /**
     * @brief Handle key press with new implementation of down/up keys.
     *
     * @param key_code the key code to submit to the browser core.     
     * @param key_released false if the key is just pressed and down key is simulated and 
     * true if the key is released and up key is simulated
     * @return true if key was accepted by the browser core and no further
     * processing of this key is required. Otherwise false if the key
     * was not accepted by the browser core 
     */
    bool handleKeyInputWithDownUpKeys(nebula::KeyCode key_code, bool key_released);    

    /**
     * @brief Terminate the currently running page
     */
    void terminateCurrentApp(bool reselect_service = true, bool invalidate_ait = false);

    /**
     * @brief Terminate the specified app_type.
     * @param app_type The app type to terminate. Supports only hbbtv and media_player.
     * @param invalidate_ait Whether to invalidate AIT. Only relevant for types hbbtv_app and
     * hbbtv_txt_app.
     * @return Whether the call is successful.
     */
    bool terminateCurrentApp(
        WebBrowserAppType const app_type,
        bool const invalidate_ait);

    /**
     * @brief starts apm application event
     */
    void setApmStarted(bool apm_started);

    /**
     * @brief checks if a recently closed application started again.
     *
     * @return true if started; false otherwise.
     */
    bool isAnyAppStartingClosedURL();

    /**
     * @brief Destroy and recreate the UI application previous created by createUIApp()
     *
     * @return true on success, false otherwise.
     */
    bool recreateUIApp();

    /**
     * @brief Destroy the UI application previous created by createUIApp()
     */
    void destroyUIApp();

    /**
     * @brief Set visibility of the UI application
     *
     * @param visible visibility of the UI application
     *
     */
    void setVisibleUIApp(bool visible);

    /**
     * @brief Get visibility of the UI application
     *
     * @return true if the UI application is visible
     *
     */
    bool isUIAppVisible();

    /**
     * @brief Set visibility of hbbTV,csp,tvStore and ui windows
     *
     * @param visible show window if true; otherwise hide the window.
     *
     */
    void setVisibleAllWindows(bool visible);

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
    bool isOpAppFocused() const;

    /**
     * @brief Move in the browser's built-in history
     * @param steps The steps to move. Negative values mean go back in time
     */
    void moveInHistory(int steps);

    /**
     * @brief Returns the number of the entries present in the browsing history for the active window
     *
     * @return count History count
     */
    int getHistoryCount();

    /**
     * @brief Returns History index in the active window
     *
     * @return index History index
     */
    int getCurrentHistoryIndex();

    /**
     * @brief Update the Http Accept Lang header
     * @param accept_lang_iso The language code to set
     */
    void updateHttpAcceptLang(char const* accept_lang_iso);

    /**
     * @brief Update the Do No Track header
     * @param do_not_track The DoNotTrack option to set
     */
    void updateDoNotTrack(UString const& do_not_track);

    /**
     * @brief Update the app_locale header
     * @param app_locale_iso The language code to set
     */
    void updateAppLocale(char const* app_locale_iso);

    /**
     * @brief Check whether any app/page is active
     * @return true, if there is an active application
     *         false, if no active application
     */
    bool isAppRunning() const;

    /**
     * @brief Get all running application types.
     *
     * @return Vector of running application types.
     */
    std::vector<WebBrowserAppType> runningAppTypes() const;
    
    /**
     * @brief Check whether any app/page is active of the type specified, support multiple active apps
     * @param app_type to check for
     * @return true, if there is an active application
     *         false, if no active application
     */
    bool isAppRunningByType(WebBrowserAppType const app_type) const;

    /**
     * @brief Check whether html5 media is playing
     * @return true, if there is a media with playing status
     *         false, otherwise
     */
    bool isMediaPlaying();

    /**
     * @brief Check whether any app/page in the starting phase
     * @return true, if there is an application in the start phase
     *         false, if no application in the start phase
     */
    bool isHbbTvAppStarting() const;

    /**
     * @brief Check whether digital ib close requested on the browser
     * @return true, if close is requested
     *         false, otherwise
     */
    bool isDigitalIbCloseRequested();

    /**
     * @brief Resets digital ib close request to false
     */
    void resetDigitalIbCloseRequest();

#ifdef INCLUDE_HOTELTV
    /**
     * @brief Make the active window or view visible on the display,
     *        or hide according to the 'visible' parameter
     * @param visible show window if true; otherwise hide the window.
     */
    void setVisible(bool visible);
#endif

    /**
     * @brief Get the current application type
     * @return The type of the active application, if any
     */
    WebBrowserAppType getCurrentAppType() const;

    /**
     * @brief Clear all data except cache
     */
    void clearPrivateBrowserData();

    /**
     * @brief Clear data stored by the browser (history, cookies, caches, ...)
     * @param clear_cookies Requests for deleting cookies
     */
    void clearPrivateData(bool clear_cookies = true);

    /**
     * @brief Clear cookies stored by the browser
     */
    void clearCookies();

    /**
     * @brief Disable browser disk caching
     */
    void disableDiskCache();

    /**
     * @brief Disable cookies
     */
    void disableCookies();

    /**
     * @brief Enable cookies
     */
    void enableCookies();

    /**
     * @brief delete the cookie from the vector by index
     *
     * @param index number of the cookie to be deleted
     *
     * @return true if success false otherwise
     */
    virtual bool deleteCookieByIndex(unsigned int);

    /**
     * @brief get all cookies as a vector
     *
     * @return vector of WebBrowserCookies
     */
    virtual std::vector<WebBrowserCookie> getCookies();

    /**
     * @brief Set the progress indicator to use
     * @param progress_indicator The progress indicator instance
     */
    void setProgressIndicator(AnyWebBrowserProgressIndicator* progress_indicator);

    /**
     * @brief Set the parental rating checker
     * @param parental_rating_checker The parental rating checker to set
     */
    void setParentalRatingChecker(OperaHbbTVParentalRatingChecker* parental_rating_checker);

    /**
     * @brief Set the ui dialog helper
     * @param ui_dialog_helper The ui dialog helper to set
     */
    void setUIDialogHelper(OperaHbbTVUIDialogHelper* ui_dialog_helper);

    /**
     * @brief An event handler that is called when a new app is created
     * @param handle The window handle
     * @param app_url  The URL of the app that is created
     * @param app_type The type of the app
     * @param linear_ip_service Inform the browser this application relates to an Linear IP Service
     */
    void onAppCreated(OperaWindowHandle const& handle,
                      cabot::String const& app_url,
                      WebBrowserAppType app_type,
                      bool linear_ip_service);

    /**
     * @brief An event handler that is called when the current app is terminated.
     * @param current_handle The current window handle
     * @param terminated_handle The terminated window handle
     * @param app_creation_error The flag indicating creation error
     * @param app_type The type of the application that is terminated
     */
    void onAppTerminated(OperaWindowHandle const& current_handle,
                         OperaWindowHandle const& terminated_handle,
                         cabot::String const& app_url,
                         bool app_creation_error,
                         WebBrowserAppType app_type);

    /**
     * @brief An event handler that is called when loading is finished
     * @param handle The window handle
     * @param url The URL of the page that is loaded
     * @param http_status_code The HTTP protocol status code returned by the server, if relevant.
     */
    void onAppDocDone(OperaWindowHandle const& handle, 
                      cabot::String const& url,
                      int const http_status_code);

    /**
     * @brief An event handler that is called when a new page is being loaded
     * @param handle The window handle
     * @param url The URl of the new page
     */
    void onAppHostChanged(OperaWindowHandle const& handle, cabot::String const& url);

    /**
     * @brief An event handler that is called in case of content errors
     * @param handle The window handle
     * @param error_code The error code
     * @param url URL failed to open
     * @param is_memory_low Variable that shows if content error occured because memory is low
     * @param is_video_memory_low Variable that shows if video memory is low
     */
    void onAppContentError(OperaWindowHandle const& handle,
                           int error_code,
                           cabot::String const& url,
                           bool is_memory_low = false,
                           bool is_video_memory_low = false);

    /**
     * @brief An event handler that is called when the initialisation of the browser is complete
     */
    void onBrowserInitComplete();

    /**
     * @brief An event handler for mouse events
     * @param event The mouse event
     */
    void onMouseEvent(MouseEventData event);

    /**
     * @brief Sets if keyboard is plugged or not
     * @param Keyboard attached value
     */
    void setUsbKeyboardStateAttached(bool attached);

    /**
     * @brief Send click mouse event
     * @param event The mouse event
     */
    void sendMouseClick();

    /**
     * @brief Sets bounds of the mouse cursor
     * @param width Maximum x value for cursor position
     * @param height Maximum y value for cursor position
     */
    void setMouseMaxBounds(int width, int height);

    /**
     * @brief Gets cursor position
     */
    bool getCursorPosition(cabot::String& cursor_position) const;

    /**
     * @brief Clear any AIT information held. Ensuring that any subsequent calls to onAitUpdate()
     * will be actioned accordingly.
     */
    void clearAit();

    /**
     * @brief An event handler for AIT updates
     * @param ait_section The AIT section instance
     * @param pid_changed bool to indicate if the pid has changed
     * @param empty_pid whether the empty PMT case occurs
     * @return true if the AIT was sent to the browser.
     */
    bool onAitUpdate(AitSectionPtr ait_section, bool pid_changed, bool empty_pmt);

    /**
     * @brief An event handler for service selection events
     * @param channel_id The channel is for the new service
     * @param ts_source_id The tuner ID for the new service
     * @param channel_change_result The result of the select operation
     */
    void onServiceSelect(NEBULA_Channel_ID channel_id,
            NEBULA_Tuner_ID ts_source_id,
            NEBULA_ChannelChangeResult channel_change_result);

    /**
     * @brief An event handler for connection status change events
     * @param connected The flag indicating the connection availability
     * @param connection_type The type of the connection
     */
    void onConnectionStatusChange(bool connected, ConnectionType connection_type, cabot::String interface_name);

    /**
     * @brief An event handler for CICAM status change events.
     * @param available true if a CICAM is available.
     */
    void onCicamStatusChange(bool available);

    /**
     * @brief An event handler to process Aux FS content.
     * @param transaction_id identifies this transaction to allow this response
     *                       to be matched with the request.
     * @param data The file contents.
     * @param data_length The length of the file content.
     */
    void onAuxfsContentReady(unsigned int transaction_id, const unsigned char* data, unsigned int data_length);

    /**
     * @brief The event handler function for internal Opera events
     * @param handler The handler function
     * @param receiver The intended receiver of the event
     */
    template <typename TEvent, typename TReceiver>
    void addEventHandler(void (TReceiver::*handler)(TEvent), TReceiver* receiver);

    /**
     * @brief An event handler for Opera engine loading started events
     * @param loading_started The data for this event
     */
    void onLoadingStarted(omi::evt::LoadingStarted loading_started);

    /**
     * @brief An event handler for Opera engine loading finished events
     * @param loading_finished The data for this event
     */
    void onLoadingFinished(omi::evt::LoadingFinished loading_finished);

    /**
     * @brief An event handler for Opera engine un-handled key input events. Notifies the integration about un-handled key input.
     * @param unhandled_key The data for this event
     */
    void onUnhandledKeyInput(omi::evt::UnhandledKeyInput unhandled_key);

    /**
     * @brief An event handler for Opera engine window block events
     * @param block_msg The data for this event (src target url- popup or new window - block reason)
     */
    void OnNewWindowBlocked(omi::evt::NewWindowBlocked block_msg);

    /**
     * @brief An event handler for Opera engine virtual keyboard visibility changed events
     * @param virtual_keyboard_visibility_changed The data for this event
     */
    void onVirtualKeyboardVisibilityChanged(
            omi::evt::VirtualKeyboardVisibilityChanged virtual_keyboard_visibility_changed);

    /**
     * @brief An event handler for Opera engine HbbTV app load timed out events
     * @param hbbtv_app_load_timed_out The data for this event
     */
    void onHbbTvApplicationLoadTimedOut(omi::evt::HbbTVApplicationLoadTimedOut hbbtv_app_load_timed_out);

    /**
     * @brief An event handler for Opera engine HbbTV app visibility changed events.
     * @param hbbtv_app_visibility The data for this event.
     */
    void onHbbTvApplicationVisibilityChanged(
            omi::evt::HbbTVApplicationVisibilityChanged hbbtv_app_visibility);

    /**
     * @brief An event handler for Opera engine HbbTV app started events
     * @param hbbtv_app_started The data for this event
     */
    void onHbbTvApplicationStarted(omi::evt::HbbTVApplicationStarted hbbtv_app_started);

    /**
     * @brief An event handler for Opera engine HbbTV app not started events, for example incorrect URL
     * @param hbbtv_app_not_started The data for this event
     */
    void onHbbTvApplicationNotStarted(omi::evt::HbbTVApplicationNotStarted hbbtv_app_not_started);

    /**
     * @brief An event handler for Opera engine HbbTV app stopped events
     * @param hbbtv_app_stopped The data for this event
     */
    void onHbbTvApplicationStopped(omi::evt::HbbTVApplicationStopped hbbtv_app_stopped);

    /**
     * @brief An event handler for Opera engine HbbTV ait app load events
     * @param hbbtv_app_event The data for this event
     */
    void onHbbTVAITAppLoadRequest(omi::evt::HbbTVAITAppLoadRequest hbbtv_app_event);

    /**
     * @brief An event handler for Opera engine when 
     * application launch has been requested by another HbbTV application by 
     * calling createApplication with a URL as an argument
     * @param app_url_load_request The data for this event
     */
    void onHbbTVAppURLLoadRequest (omi::evt::HbbTVAppURLLoadRequest app_url_load_request);

    /**
     * @brief An event handler for Opera engine Oipf app started event
     * @param app_started The data for this event
     */
    void onOipfApplicationStarted(omi::evt::OIPFApplicationStarted app_started);

    /**
     * @brief An event handler for Opera engine Oipf app stopped event
     * @param app_stopped The data for this event
     */
    void onOipfApplicationStopped(omi::evt::OIPFApplicationStopped app_stopped);

    /**
     * @brief An event handler for Opera engine Oipf app start failure event
     * @param app_fail The data for this event
     */
    void onOipfApplicationFailed(omi::evt::OIPFApplicationStartFailed app_fail);

    /**
     * @brief An event handler for Opera engine Oipf app activated event
     * @param app_activated The data for this event
     */
    void onOipfApplicationActivated(omi::evt::OIPFApplicationActivated app_activated);

    /**
     * @brief An event handler for Opera engine Oipf app deactivated event
     * @param app_deactivated The data for this event
     */
    void onOipfApplicationDeactivated(omi::evt::OIPFApplicationDeactivated app_deactivated);

    /**
     * @brief An event handler for Opera engine URL changed events
     * @param url_changed The data for this event
     */
    void onUrlChanged(omi::evt::URLChanged url_changed);

    /**
     * @brief An event handler for keyset changed events
     * @param keyset_changed The data for the event
     */
    void onHbbTvKeysetChanged(omi::evt::HbbTVKeysetChanged keyset_changed);

    /**
     * @brief An event handler for AIT parsing status event
     * @param ait_parsing_status The data for the event
     */
    void onAitParsingStatus(omi::evt::AITParsingStatus ait_parsing_status);

    /**
     * @brief An event handler for AIT updated events. Opera fires this
     *        when it received AIT table and successfully parsed it.
     * @param ait_updated The data for the event
     */
    void onAitUpdated(omi::evt::AITUpdated ait_updated);

    /**
     * @brief An event handler for Script Request Close Window event. Opera fires this
     *        when a script calls window.close() within a window.
     * @param close_window_request The data for the event
     */
    void onScriptRequestCloseWindow(omi::evt::ScriptRequestCloseWindow close_window_request);

    /**
     * @brief An event handler for Window Destoyed event. Opera fires this
     *        when a script calls window.close() within a window.
     * @param window_destroyed The data for the event
     */
    void onWindowDestroyed(omi::evt::WindowDestroyed window_destroyed);

    /**
     * @brief An event handler for TV Store Close Window event. Opera fires this
     *        when a script calls window.close() within a TV Store window.
     * @param close_window_request The data for the event
     */
    void onTVStoreWindowClosed(omi::evt::TVStoreWindowClosed close_window_request);

    /**
     * @brief An event handler for CICAMFileRequested event.
     * @param file_request The CICAM file request.
     */
    void onCicamFileRequested(omi::evt::CICAMFileRequested file_request);

    /**
     * @brief An event handler for quota increase requests
     * @param request_quota_permission The data for the event
     */
    void onRequestQuotaPermission(omi::evt::RequestQuotaPermission request_quota_permission);

    /**
     * @brief An event handler for fullscreen mode requests
     * @param request_fullscreen_mode The data for the event
    */
    void onFullscreenModeRequest(omi::evt::FullscreenModeRequest request_fullscreen_mode);

    /**
     * @brief An event handler for browser memory events
     * @param browser memory event with memory event state
     */
    void onBrowserMemoryEvent(omi::evt::BrowserMemoryEvent browser_memory_event);

#if defined USING_SDK422 || defined USING_SDK423
    /**
     * @brief An event handler for browser fatal error events
     * @param fatal_error event with a reason
     */
    void onBrowserServiceFatal(omi::evt::BrowserServiceFatalError fatal_error);
#endif

    /**
     * @brief An event handler for browser EGL errors
     * @param egl_error The data for the event
     */
    void onEglContextLostError(omi::evt::EGLContextLost egl_context_lost);

    /**
     * @brief An event handler for parental query events
     * @param parental_control_query_event The data for the event
     */
    void onParentalControlQueryEvent(omi::evt::ParentalControlQueryEvent
                                     parental_control_query_event);

    void onHbbTVBroadcastRelatedStatusChanged(omi::evt::HbbTVBroadcastRelatedStatusChanged status_change);

    /**
     * @brief An event handler for Opera engine media object changed events
     * @param media_object_changed The data for this event
     */
    void onMediaObjectChanged(omi::evt::MediaObjectChanged media_object_changed);

#ifdef INCLUDE_OPAPP_SUPPORT
    /**
     * @brief An event handler for the result of starting an operator application.
     * @param start_result Details of the event.
     */
    void onOpAppApplicationStartResult(omi::evt::OpAppApplicationStartResult start_result);

    /**
     * @brief An event handler for the result of an operator application stopping.
     * @param stopped Details of the event.
     */
    void onOpAppApplicationStopped(omi::evt::OpAppApplicationStopped stopped);

    /**
     * @brief An event handler for an operator application changing state.
     * @param state_changed Details of the event.
     */
    void onOpAppStateChanged(omi::evt::OpAppStateChanged state_changed);

    /**
     * @brief An event handler for OpApp keyset changed events.
     * @param keyset_changed The data for the event.
     */
    void onOpAppKeysetChanged(omi::evt::OpAppKeySetChanged keyset_changed);

#ifdef USING_SDK420
    /**
     * @brief An event handler for OpApp discovery events.
     * @param discovered The data for the event.
     */
    void onOpAppApplicationsDiscovered(omi::evt::OpAppApplicationsDiscovered discovered);

    /**
     * @brief An event handler for OpApp non-discovery events.
     * @param not_discovered The data for the event.
     */
    void onOpAppApplicationsNotDiscovered(omi::evt::OpAppApplicationsNotDiscovered not_discovered);

    /**
     * @brief An event handler for OpApp installation requests.
     * @param request The data for the event.
     */
    void onOpAppApplicationInstallRequest(omi::evt::OpAppApplicationInstallRequest request);
#endif
    
#ifdef USING_SDK423
    /**
     * @brief An event handler for when OpApp discovery starts.
     * @param discovery_started The data for the event.
     */
    void onOpAppDiscoveryServiceBroadbandDiscoveryStarted(
                omi::evt::OpAppDiscoveryServiceBroadbandDiscoveryStarted discovery_started);

    /**
     * @brief An event handler for OpApp discovery status.
     * @param partial_result The data for the event.
     */
    void onOpAppDiscoveryServiceBroadbandDiscoveryPartialResult(
                omi::evt::OpAppDiscoveryServiceBroadbandDiscoveryPartialResult partial_result);

    /**
     * @brief An event handler for OpApp installation requests.
     * @param install_request The data for the event.
     */
    void onOpAppDiscoveryServiceInstallRequest(
                omi::evt::OpAppDiscoveryServiceInstallRequest install_request);

    /**
     * @brief An event handler for available OpApps.
     * @param available_applications The data for the event.
     */
    void onOpAppDiscoveryServiceAvailableApplications(
                omi::evt::OpAppDiscoveryServiceAvailableApplications available_applications);
#endif
    
    /**
     * @brief An event handler for OpApp Replace UI elements.
     * @param replace_ui_elements The data for the event.
     */
    void onOpAppReplaceUiElements(omi::evt::OpAppReplaceUiElements replace_ui_elements);
#endif

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    void onBroadcastResourceGranted(omi::evt::BroadcastResourceGranted granted_event);
    void onBroadcastResourceReleaseRequested(omi::evt::BroadcastResourceReleaseRequested release_request_event);
#endif

#ifdef USE_MEDIA_PLAYER_WINDOW
    // Media Player window events.
    void onMediaPlayerError(omi::evt::MediaPlayerError event);
    void onMediaPlayerLoadFinished(omi::evt::MediaPlayerLoadFinished event);
    void onMediaPlayerPlaybackRevoked(omi::evt::MediaPlayerPlaybackRevoked event);
#endif
#ifndef NOVATEK_PLATFORM
#ifdef USING_SDK423
    void onWebAudioCreated(omi::evt::WebAudioCreated event);
    void onWebAudioDeleted(omi::evt::WebAudioDeleted event);
#endif
#endif

    /**
     * @brief Checks  whether given url is cached or not.
     * @return true if url is cached, false otherwise
     */
    bool isInCache(cabot::String const & url);

    /**
     * @brief Enable or disable HbbTV
     *
     * @param hbbtv_enabled If true, enable HbbTV features
     *                      If false, disable HbbTV features
     * @param terminate_hbbtv_service whether disable action should terminate hbbtv service if running,
     * rather then relying on the Linear HbbTV Service lifecycle.
     */
    void enableHbbTv(bool const hbbtv_enabled, bool const terminate_hbbtv_service);

    /**
     * @brief Enable or disable HbbTV
     *
     * @param hbbtv_enabled If true, enable HbbTV features
     *                      If false, disable HbbTV features
     */
    void enableHbbTv(bool const hbbtv_enabled);

    /**
     * @brief Posts RaiseDSMCCStreamEvent when stream event is raised
     *
     * @param listener_id Identifier of the listener passed during the registration of a stream event.
     * @param video_id Identifier of a video broadcast object whose stream events are to be unregistered.
     * @param event_name The name of the event.
     * @param event_data The data of the event.
     * @param event_texT The text of the event.
     * @param event_status The text of the event.
     */
    void raiseStreamEvent(unsigned int listener_id,
            unsigned int video_id,
            const std::string& event_name,
            const std::string& event_data,
            const std::string& event_text,
            const std::string& event_status);

    /**
     * @brief Select whether the current service should be selected after the
     *        current application is terminated
     * @param select_upon_termination If true, the current service is to be selected
     *                                If false, service selection should not be done
     */
    void setSelectServiceUponAppTermination(bool select_upon_termination);

    /**
     * @brief Makes HbbTV window inactive/active
     * @param active set active HbbTV window if true; otherwise inactive.
     */
    void setHbbTVWindowActive(bool active);

    /**
     * @brief YT Certification: When Dial Server is run, generated additionalDataUrl
     *        is passed to OperaEngine. This additionalDataUrl should be included in
     *        youtube url when youtube is launched
     * @param additional_data_url Generated url that is passed to Opera Engine
     */
    void setAdditionalDataUrlForYoutube(cabot::String const& additional_data_url);

    /**
     * @brief YT Certification: The method how Youtube app is launched (from menu or
     *        remote controller) should be included in Youtube Url, as well. This method
     *        generates the final youtube url to be connected.
     * @param launch_method Either remote or menu
     * @return final Youtube Url string to be connected.
     */
    cabot::String getYoutubeUrl(cabot::String const& launch_method, cabot::String url = "https://www.youtube.com/tv");

    cabot::String const& userAgentString();

    OperaWindowManager const* getWindowManager();

    /**
     * @brief Update user agent string.
     */
    void updateUserAgent();

    void printCurrentMemoryUsage();
    void closedURLTimeout();
#ifdef INCLUDE_HOTELTV
    void HbbStoppedSPMTimeout();
#endif
    
    unsigned int getPlaneWidth();
    unsigned int getPlaneHeight();
    void setPlaneWidthHeight(unsigned int width, unsigned int height);
    /**
     * @brief Interface for setting proxy using Opera's setPref method
     * @param proxy string. format addr:port
     * @return true if proxy is set, false otherwise.
     */
    bool setHttpProxy(UString addr);

    /**
     * @brief Called to send a blur/focus event to the application's Window object
     *        when an HbbTV application loses/regains input focus (see HbbTV 1.4.1:10.2.2.1 specs)
     */
    void focusHbbtvApplication(bool const focus_application);

    /**
     * @brief Called to send access Criteria Changed event to browser to be sent to Op-App owner
     *        when XCA/FTVA access criteria values change     
     * @return whether sent AC message
     */
    bool  accessCriteriaChanged(cabot::String const& message) const;

    /**
     * @return A 16 bit value containing a set of bit flags representing the additional HbbTv
     * application features supported by the system.
     * Partially described in ETSI TS 102 796 - 7.2.3.1 - Table 5.
     */
    std::uint16_t getHbbTvApplicationProfile() const;

    /** @brief Get the required HbbTV Profile
      * @return int representing the HbbTV Profile supported
      * See enum omi::PlatformProfile
      */
    int getHbbTvProfile() const;

    /**
     * @brief Sets start url is opened
     * @param value :the new value
     */
    void setStarturlOpened(bool value);

    /**
     * @brief Checks start url is opened
     * @return true if start url is opened, false otherwise
     */
    bool isStarturlOpened() const;

    /**
     * @brief Activate/deactivate browser UI windows
     * @param acivate : true if activated, false otherwise
     */
    void setCookieActiveWindow(bool activate);

    /**
     * @brief Get corresponding nebula component type for given omi track type
     * @param track_type Track type to be converted
     * @return corresponding nebula component type
     */
    OIPF_ComponentType convertTrackTypeToNebulaType(
                const omi::common::TrackType track_type);

    /**
     * @brief Return the list of components of the content streaming at the time
     * @param type           The type of the component (Video, Audio or Subtitle)
     * @param component_list The list of components returned
     * @return true, if the components can be gathered successfully,
     *         false, otherwise
     */
    bool getComponents(OIPF_ComponentType type,
                       NEBULA_AVComponentList* component_list) override;

    /**
     * @brief Select the given A/V/S component
     * @param component The component to be selected
     * @return true, if component selection is successful, false otherwise
     */
    bool selectComponent(NEBULA_AVComponent const* component) override;

    /**
     * @brief Unselect the given A/V/S component
     * @param component The component to be unselected
     * @return true, if component unselection is successful, false otherwise
     */
    bool deselectComponent(NEBULA_AVComponent const* component) override;


    /**
     * @brief resume application coordinator
     */
    void resumeApplicatonCoordinator();

    /**
     * @brief suspend application coordinator
     */
    void suspendApplicatonCoordinator(frost_bool activate);

    /**
     * @brief resume Mheg
     */
    void resumeMheg();

    /**
     * @brief suspend Mheg
     */
    void suspendMheg();

    /**
     * @brief Set the current media component preferences.
     *
     * @note Only applicable when using Opera SDK 4.11 or above.
     *
     * @param preferred_subtitle_languages A list of ISO 639-1 language codes
     *                                     in order of priority, highest first.
     * @param subtitles_enabled Set to true if subtitles are to be presented, if available,
     *                          or set to false to prevent the display of subtitles.
     * @param preferred_audio_languages A list of ISO 639-1 language codes
     *                                  in order of priority, highest first.
     * @param audio_description_enabled Set to true if AD audio components should be presented,
     *                                  if available, or set to false if only main audio should
     *                                  be presented.
     */
    void setMediaComponentsPreferences(
            std::vector<std::string> const & preferred_subtitle_languages,
            bool subtitles_enabled,
            std::vector<std::string> const & preferred_audio_languages,
            bool audio_description_enabled);

    /**
     * @brief Set the audio description preference without post.
     * 
     * @param audio_description Set to true if AD audio components should be presented,
     *                          if available, or set to false if only main audio should
     *                          be presented.
     */
    void setAudioDescriptionPreference(bool audio_description);

    /**
     * @brief An event handler for browser GPU memory events
     * @param browser_gpu_memory_event memory event that shows the details about the GPU memory status.
     */
    void onBrowserGPUMemoryEvent(omi::evt::BrowserGPUMemoryEvent browser_gpu_memory_event);

    /**
     * @brief return the window memory information
     */
    void getWindowMemoryInformation();

    /**
     * @brief start an advertisement in a standard window
     * @param url The URL of the page to open
     * @return true if successful, false otherwise
     */
    bool startAdvertisementBrowser(cabot::String const& url);

    /**
     * @brief stop the currently running advertisement page
     */
    bool stopAdvertisementBrowser();

    /**
     * @brief An event handler that is called when the renderer process dies
     * @param renderer_process_died Instance of RendererProcessDied event which shows the details of the failure.
     */
    void onRendererProcessDied(omi::evt::RendererProcessDied renderer_process_died);

    /**
     * @brief send custom messages to Javascript code of active page
     *
     * @param message string to pass to active page
     */
    void sendPlatformMessage(const char* message);
    
    bool isWindowActive(cabot::String const& window_name);

    bool setWindowActiveByName(cabot::String const& window_name);

    /**
     * @brief prints the browser list and the active window list
     */
    void getWindowListAsName();

    /**
     * @brief sets given windows visibility as visible parameter
     *
     * @param window string is page name
     * @param visible boolean value, True when making window visible, false otherwise
     */
    void setAppVisibilityWithGivenName(cabot::String const& window, bool visible);

    /**
     * @brief Launch Browser Launcher.
     *
     * @param url URL of the launcher app that will be launched.
     *
     * @return true if URL is launch request is done, false otherwise.
     */
    bool startBrowserLauncher(cabot::String const& url);

    /**
     * @brief Terminates previously loaded Browser Launcher.
     */
    void stopBrowserLauncher();

    /**
     * @brief Reload browser launcher.
     */
    void reloadBrowserLauncher();

    /**
     * @brief Change the visibility of Browser Launcher.
     *
     * @param visible Make launcher visible if true, hide otherwise.
     *
     * @return true if visibility change request is done, false otherwise.
     */
    bool setBrowserLauncherVisible(bool visible);

    /**
     * @brief Return if Browser Launcher is visible or not.
     *
     * @return true if visible, false otherwise.
     */
    bool isBrowserLauncherVisible() const;

    /**
     * @brief Return if Browser Launcher is focused or not.
     *
     * @return true if focused, false otherwise.
     */
    bool isBrowserLauncherFocused() const;

    /**
     * @brief Give focus to the launcher window so that it will receive key input events.
     *
     * @param focus If true, give focus to the launcher, if false take focus from it.
     *
     * @return true on success, false otherwise.
     */
    bool setFocusToBrowserLauncher(bool focus);

    /**
     * @brief Launch a window hosting media content (e.g. mpd).
     *
     * @param url The url of the media to play.
     * @param linear_ip_service True if the url is a Linear IP Service.
     * @return true if the request to launch the media is successful, false otherwise.
     */
    bool startMediaPlayer(cabot::String const& url, bool const linear_ip_service);
       
#ifdef INCLUDE_HOTELTV
    void sendPlatformMessageInJsonFormat();
    void addSendPlatformMessageParameter(cabot::String first_param, cabot::String second_param);
    void clearSendPlatformMessageParameter();
    void setHbbAppStartedByUser(bool enable);
    bool isHbbAppStartedByUser();
#endif

    /**
     * Wrapper for the identical method in Opera.h but traces the Message type.
     *
     * @note This method should be used throughout opera integration to provide
     *       tracing of messages posted to Opera.
     *
     * From Opera.h:
     *    Help function for post() in the form of a template to allow
     *    implicit casting of the received future object to one matching
     *    the message type.
     *
     * @note This method does NOT take the ownership of the message instance
     *
     * @param message A message instance of the type that represent the required action.
     * @return A message future object instance that can be used to access the result
     *         from the message action in the case a result is expected.
     */
    template<typename TMessage>
    typename TMessage::FutureType post(const TMessage &message);

    /**
     * @brief Gets the rectangular region of the media player window.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the media player window has a rectangular region set, false otherwise.
     */
    bool getMediaPlayerPosition(double& x,
                                double& y,
                                double& width,
                                double& height);

    /**
     * @brief Sets the rectangular region of the media player window.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the media player window rectangular region is set, false otherwise.
     */
    bool setMediaPlayerPosition(double x,
                                double y,
                                double width,
                                double height);
    
    /**
     * @brief Set whether browser should launch ait autostart on termination of HbbTV Window.
     * @param  restart_flag whether or not to restart
     */
    void setRestartAfterTermination(bool const restart_flag);

private:
    // implement Active
    void threadMain();

    void tracePost(const omi::Message& msg) const;

    /**
     * @brief Activate/Deactivate media track playing in a window with
     *        given handle.
     * @param handle      browser window handle application is running in
     * @param track       media track to be activated/deactivated
     * @param activate    indicates whether to activate or deactivate given track
     */
    void setMediaTrack(OperaWindowHandle& handle,
                       const omi::common::MediaObject::Track& track,
                       bool activate);

    /**
     * @brief Find corresponding omi type media track for given a/v component
     * @param handle        browser window handle application is running in
     * @param component     a/v component to be searched for in media tracks
     * @param media_track   track to be returned if found
     * @return true if corresponding track is found, false otherwise.
     */
    bool findCorrespondingTrack(OperaWindowHandle handle,
                                NEBULA_AVComponent const* component,
                                omi::common::MediaObject::Track& media_track);
private:
    void destroyCurrentWindow();
    void enableUrlFiltering();
    void disableCertificateErrorPage(OperaWindowHandle window_handle);
    void enableNotifySriptsClosingWindows(OperaWindowHandle window_handle);
    void setXhrOriginCheck(OperaWindowHandle window_handle, const UString& value);
    void setStorageLimits();
    void setMseBufferMemoryLimits();
    bool setPreference(UString const& key, UString const& value);
    bool getGlobalPreference(UString const& key, UString & value);
    bool setGlobalPreference(UString const& key, UString const& value);
    void sendInvalidateAitMessage(OperaWindowHandle window_handle);
    void sendChannelChangedMessage(NEBULA_Channel_ID channel_id);
    bool handleXmlAitUrl(cabot::String const& url, bool parse_ait_xml, bool broadcast_dependent, bool allow_auto_transit);
    cabot::String getUserAgentBase();
    cabot::String getKeyRemappingsForURL(cabot::String const& url);

    /**
     * @brief Checks whether given url belongs to Opera TV Store and handles it internally if needed
     * @param url The URl of the page
     *
     * @return true if url loaded successfully, false otherwise
     */
    bool handleTVStoreUrl(cabot::String const& url);

    void startMemoryAccounting(unsigned int print_every_msec);

    bool getActiveWindowHandleByName(OperaWindowHandle& window_id, cabot::String const& name) const;
    static void setGPUMemAsFreed(void* /*user_data*/);

    /**
     * @brief Check whether Linear IP Service is active
     * @return true, if there is an active Linear IP Service else false
     */
    bool isLinearIPService() const;

    void handleResourceManagerIsBroadcastStoppedByBrowser();

    /**
     * @brief An event handler for Opera engine HbbTV app stopped events generated from
     *        a real omi::evt::HbbTVApplicationStopped or internally on browser shutdown.
     */
    void hbbTvApplicationStopped(vewd_integ::HbbTVApplicationStopped& hbbtv_app_stopped);

    void showWindowMemoryInformation(OperaWindow* win, std::string name);
    
private:
    static const unsigned int default_browser_window_width  = 1280;
    static const unsigned int default_browser_window_height = 720;

private:
    bool m_engine_running;
    bool m_do_restart;

#if defined USING_SDK422 || defined USING_SDK423
    OperaBrowserRestart m_restart;
    OperaBrowserRestartTestTool m_restart_testtool;
#endif

    std::unique_ptr<omi::evt::HbbTVApplicationStarted> m_last_hbbtv_app_started;
    omi::Browser* m_opera;
    ConnectionType m_connection_type;
    CursorPosition m_mouse_cursor_position;
    int m_mouse_active_buttons;
    bool m_is_exit_from_tv_browser_requested;
    bool m_is_special_app_starting;
    bool m_is_hbbtv_app_starting;
    bool m_is_tv_store_app_restarting;
    AnyWebBrowserProgressIndicator*  m_progress_indicator;
    BrowserEventPublisher*           m_event_publisher;
    OperaHbbTVBrowserEventPublisher* m_hbbtv_event_publisher;
    OperaMediaPlayerEventPublisher*  m_media_player_event_publisher;
    OperaOipfBrowserEventPublisher*  m_oipf_event_publisher;
    OperaWindowManager*              m_window_manager;
    OperaHbbTVParentalRatingChecker* m_parental_rating_checker;
    OperaHbbTVUIDialogHelper*        m_ui_dialog_helper;

    bool m_hbbtv_enabled;
    bool m_digital_ib_loaded;
    bool m_digital_ib_close_requested;
    bool m_advertisement_loaded;
    bool m_select_service_upon_app_termination;
    NEBULA_Channel_ID m_last_channel_id;
    NEBULA_Tuner_ID   m_last_ts_source_id;

    bool m_app_creation_started;
    cabot::String m_user_agent_string;

    cabot::String m_closed_window_url;

    bool m_virtual_keyboard_visible;
    bool m_is_teletext_app_started;
    bool m_is_memory_low;
    bool m_is_oom_occured;
    bool m_is_browserui_window;
    bool m_closed_url_started;
    bool m_window_resolution_changed;

    unsigned int m_plane_width;
    unsigned int m_plane_height;

    cabot::String m_additional_data_url;

    CabotTimerHandle m_memory_acct_timer;
    CabotTimerHandle m_closed_url_timer;
    CabotTimerHandle m_gpu_mem_free_timer;

#ifdef INCLUDE_HOTELTV
    CabotTimerHandle m_hbb_stopped_spm_timer;
#endif

    typedef decltype(static_cast<omi::Event*>(nullptr)->type()) EventProcessorsKey;
    typedef std::multimap<EventProcessorsKey, AnyOperaEventProcessor*> EventProcessors;

    EventProcessors m_event_processors;

    /**
     * @brief The user agent string provider for the opera engine.
     */
    AnyWebBrowserUserAgentProvider *m_user_agent_provider;

    uint64_t m_last_received_event;

    typedef std::map<int, omi::evt::AITUpdated::HbbTVApplication> AppInfo;
    AppInfo m_application_info;

    mutable FrostReadWriteMutex m_initialisation_mutex;
    bool m_init_completed;


    AnyWebBrowserAitHandler* m_ait_handler;

    bool m_is_teletext_available;
    unsigned int m_teletext_org_id;
    unsigned short m_teletext_app_id;
    typedef std::vector<std::string> Urls;
    typedef std::map<unsigned short, Urls> AppUrls;
    AppUrls m_ait_application_urls;

    OperaHbbTVAppKeyset m_app_keyset;

    typedef std::map<unsigned int, omi::evt::CICAMFileRequested*> AuxfsRequests;
    AuxfsRequests m_auxfs_requests;

    FrostMutex m_auxfs_requests_mutex;
    NEBULA_CallbackHandle m_cicam_status_callback_handle;
    NEBULA_CallbackHandle m_auxfs_reader_callback_handle;
    OperaOipfGlobalDrmClient* m_drm_client;

    std::uint16_t m_hbbtv_application_profile;
    int m_hbbtv_profile;

    OperaHandsetConfiguration* m_handset_configuration;
    OperaHandsetIntegration* m_handset_integration;
    ConnectionType m_handset_connection;

    AnyWebBrowserOpAppConfigurationProvider const* m_opapp_configuration_provider;
    OperaOpAppBrowserEventPublisher* m_opapp_event_publisher;
    OpAppFileProvider* m_opapp_file_provider;

    omi::msg::SetMediaComponentsPreferences m_component_preferences;
#ifdef INCLUDE_HOTELTV
    cabot::String m_spm_str;
    bool m_hbb_app_started_by_user;
#endif

    OperaMediaResourceManager* m_opera_resource_manager;

    bool m_opapp_suspended;
    bool m_opapp_active_postponed;
    bool m_is_external_source_selected;
};

#include "vewd_integration/core/OperaEngine.inl"

#endif
