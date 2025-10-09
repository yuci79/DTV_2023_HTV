/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief The main interface between the application layer and the
 *        browser. Initialises the browser engine, passes input events, ait
 *        updates and dsmcc events.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERCORE_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERABROWSERCORE_HPP_INCLUDED

#include "config/include_dial_support.h"
#include "config/include_opapp_support.h"
#include "config/include_hoteltv.h"

#include "BrowserEventPublisher.hpp"
#include "OperaHbbTVBrowserEventPublisher.hpp"
#include "OperaHbbTVParentalRatingChecker.hpp"
#include "OperaHbbTVUIDialogHelper.hpp"
#include "OperaMediaPlayerEventPublisher.hpp"
#include "OperaOipfBrowserEventPublisher.hpp"

#include "vewd_integration/opapp/OperaOpAppBrowserEventPublisher.hpp"

#include "nebula/browser_interface/AnyWebBrowserEventListener.hpp"
#include "nebula/browser_interface/AnyWebBrowserHbbTVEventListener.hpp"
#include "nebula/browser_interface/AnyWebBrowserOipfEventListener.hpp"
#include "nebula/browser_interface/AnyWebBrowserTvPreferencesChangeListener.hpp"
#include "nebula/browser_interface/NullWebBrowserEventListener.hpp"
#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "eclipse/core/IPTypes.hpp"

#include "nebula/browser_interface/AnyWebBrowserCore.hpp"
#include "nebula/browser_interface/AnyWebBrowserAitHandler.hpp"
#include "nebula/core/ait_hub/AitSubtable.hpp"
#include "nebula/core/browser_client/LanguageSetting.hpp"
#include "nebula/core/browser_client/SoundSettings.hpp"
#include "nebula/core/browser_client/SubtitleSettings.hpp"
#include "nebula/core/browser_client/NetworkSettings.hpp"
#include "vewd_common/OperaMouseController.hpp"
#include "OperaEngine.hpp"
#include "../dsmcc/OperaDsmccEngine.hpp"
#include "vewd_common/AnyWebBrowserMouseController.hpp"
#include "OperaCertificateManager.hpp"

#include "utilities_public/FrostMutex.hpp"
#include "utilities_private/PrioritisedCommandThread.hpp"


class AnyOperaGraphicsBackend;
class EventDispatchConfigurer;
class VewdAuroraIpcServer;

enum
{
    do_not_track_option_enabled = 1,
    do_not_track_option_unset = -1,
};

/**
 * @brief Wrapped opera browser implementation.
 */
class OperaBrowserCore : public AnyWebBrowserCore
                       , public AnyWebBrowserMouseController
                       , public AnyWebBrowserTvPreferencesChangeListener
                       , private NullWebBrowserEventListener
{
public:
    OperaBrowserCore();
    virtual ~OperaBrowserCore();
    // Implement AnyWebBrowserCore.
    virtual bool initialise(
            AnyWebBrowserUIController& ui_controller,
            AnyWebBrowserProgressIndicator& progress_indicator,
            AnyWebBrowserUserAgentProvider& user_agent_provider,
            AnyWebBrowserInternetStatusProvider& internet_status_provider,
            AnyWebBrowserTvPreferencesProvider& tv_preferences_provider,
            AnyWebBrowserTvPreferencesChangePublisher & user_preferences_change_publisher,
            AnyWebBrowserAitHandler & ait_handler,
            AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider,
            bool initialise_browser_synchronously);
    virtual void finalise();
    bool initialisationCompleted() override;    
    bool handleKeyPress(nebula::KeyCode key) override;    
    bool handleKeyPressWithDownUpKeys(nebula::KeyCode key, bool key_released) override;

    virtual bool createApp( cabot::String const& url,
                            bool parse_ait_xml,
                            bool broadcast_related,
                            bool hbbtv_application,
                            bool hbbtv_service,
                            bool allow_auto_transit = true);
    virtual bool terminateCurrentApp(bool invalidate_ait);
    bool terminateCurrentApp(WebBrowserAppType const app_type, bool const invalidate_ait) override;

    bool setOperatorAppDiscoveryEnabled(bool enabled) override;
    bool isOperatorAppDiscoveryEnabled() const override;
    bool startOperatorAppDiscovery(
            nebula::OperatorApplicationDiscoveryProperties const & configuration) override;
    bool stopOperatorAppDiscovery() override;
    bool uninstallOperatorApp(
                std::uint16_t application_id, std::uint32_t organisation_id) override;
    bool setOperatorApplicationDiscoveryUris(std::vector<cabot::String> const & uris) override;
    bool startOperatorApp(
            nebula::OperatorApplicationProperties const & application) override;
    bool stopOperatorApp() override;
    void suspendOperatorApp() override;
    void resumeOperatorApp() override;
    bool isOperatorAppSuspended() const override;
    bool setOperatorAppState(
            nebula::OperatorApplicationState new_state) override;
    bool setOperatorAppContext(const cabot::String& query_url) override;
    bool handleOperatorAppKeyPress(
                            nebula::KeyCode key_code,
                            nebula::KeyPressHandlingReason keypress_handling_reason) override;

    virtual void setApmStarted(bool apm_started);
    virtual bool isAnyAppStartingClosedURL();

#ifdef INCLUDE_HOTELTV
    /**
     * @brief Make the active window or view visible on the display,
     *        or hide according to the 'visible' parameter
     * @param visible show window if true; otherwise hide the window.
     */
    virtual void setVisible(bool visible);
#endif

    virtual WebBrowserAppType getCurrentAppType();
    virtual bool isTeletextApplicationStarted() const;
    virtual bool isAnyAppRunning();
    virtual std::vector<WebBrowserAppType> runningAppTypes() const;
    virtual bool isMediaPlaying();  
    bool isAppRunning(const WebBrowserAppType type) const override;
    bool isHbbTvAppStarting() const override;
    virtual bool isDigitalIbCloseRequested();
    virtual void resetDigitalIbCloseRequest();
    virtual void getUIApp(cabot::String& application_url);
    virtual bool createUIApp(cabot::String const& application_url,
                             cabot::String const& overlay_application_url,
                             ApplicationResolution application_resolution
                             = application_resolution_default);
    virtual bool recreateUIApp();
    virtual bool destroyUIApp();
    virtual void setVisibleUIApp(bool visible);
    virtual bool isUIAppVisible();
    virtual void setVisibleAllWindows(bool visible);

    /**
     * @brief delete the cookie from the vector by index
     *
     * @param index number of the cookie to be deleted
     *
     * @return true if success false otherwise
     */
    virtual bool deleteCookieByIndex(int index_to_delete);

    /**
     * @brief get all opera cookies as a vector
     *
     * @return vector of WebBrowserCookies
     */
    virtual std::vector<WebBrowserCookie> getCookies();
    virtual void handleAppCertificateError(bool allow, unsigned int error_id);
    virtual void setFocusToOpApp(bool focus);    
    virtual bool isOpAppFocused() const;
    virtual bool isHbbSystemSuspended() const;
    virtual bool suspendHbbSystem(bool const terminate_hbbtv_service,
                                  bool bind_to_current_channel);
    virtual bool resumeHbbSystem();
    virtual bool setHbbSystemEnabled(bool enable);
    virtual bool isHbbSystemEnabled() const;
    bool hasHbbSystemBeenSet() const override;
    virtual bool dnsSettingsUpdated(cabot::String const& dns_primary,
                                    cabot::String const& dns_secondary);
    virtual bool connectionStatusChanged(ConnectionStatus& new_status);
    virtual bool setHttpProxyParams(IPProxyParams const& proxy_params);
    virtual bool getHttpProxyParams(IPProxyParams & proxy_params);
    virtual bool historyBack(int const);
    virtual bool isTeletextAvailable();
    virtual bool startTeletext();
    virtual bool stopTeletext(bool launch_autostart_app) override;
    virtual void setUsbKeyboardStateAttached(bool attached);
    virtual bool setUrl(cabot::String const&);
    virtual bool setUrl(cabot::String const& url,
                        bool parse_ait_xml,
                        bool broadcast_related);
    virtual bool openDvbIXmlAit(cabot::String const& xml_ait_url);
    virtual bool createInternetBrowser(cabot::String const& start_url);
    virtual bool destroyInternetBrowser();
    virtual bool getInternetBrowserCurrentUrl(cabot::String & url);

    virtual bool isInternetBrowserRunning();
    virtual bool isInternetBrowserExitRequested() const;
    virtual bool isTvStoreAppRunning() const;
    virtual bool isTvStoreAppRestarting();
    virtual bool isSpecialAppStarting() const;
    virtual void setSpecialAppStarting(bool status);
    virtual bool isExternalSourceSelected() const;    
    virtual void setExternalSourceSelected(bool selected);
    virtual bool initialiseMouse();
    virtual bool isMouseEnabled();
    virtual void mouseEvent(MouseEventData event);
    virtual void notifySystemLanguageUpdated();
    virtual void notifyUserAgentUpdateRequested();
    virtual bool isInCache(cabot::String const& url);
    virtual void factoryReset();
    virtual void clearPersistentStorage(bool clear_cookies = true);
    virtual void clearPrivateBrowserData();
    virtual void disableDiskCache();
    virtual void clearCookies();
    virtual void disableCookies();
    virtual void enableCookies();
    virtual void dateTimeUpdated();
    virtual void updateDoNotTrackOption();
    void focusHbbTvApplication(bool const focus_application) override;
    void clearAit() override;
    bool processAit(AitSectionPtr const& ait,
                    const bool pid_changed,
                    bool empty_pmt) override;
    void serviceSelected(NEBULA_Channel_ID channel_id,
                         NEBULA_Tuner_ID transport_stream_source_id,
                         NEBULA_ChannelChangeResult channel_change_result) override;
    void setStarturlOpened(bool value);
    bool isStarturlOpened() const;
    void setCookieActiveWindow(bool activate);
    virtual UString getBrowserVersionString() const;


    // Youtube Certification: additionalDataUrl and launch method should be included in url
#ifdef INCLUDE_DIAL_SUPPORT
    virtual void setAdditionalDataUrlForYoutube(cabot::String additional_data_url);
#endif
    virtual cabot::String getYoutubeUrl(cabot::String launch_method);
    virtual void setCursorPosition(int x, int y);
    virtual bool getCursorPosition(cabot::String& cursor_position) const;

    virtual void sendPlatformMessage(const char* message);
    virtual bool setWindowActiveByName(cabot::String const& window_name);
    virtual bool isWindowActive(cabot::String const& window_name);
    virtual void getWindowListAsName();
    virtual void setAppVisibilityWithGivenName(cabot::String const& window, bool visible);
    virtual void setMediaComponentsPreferences();
    virtual void getWindowMemoryInformation();

    virtual bool startAdvertisementBrowser(cabot::String const& url);
    virtual bool stopAdvertisementBrowser();

    virtual bool startBrowserLauncher(cabot::String const& url);
    virtual void stopBrowserLauncher();
    virtual void reloadBrowserLauncher();
    virtual bool setBrowserLauncherVisible(bool visible);
    virtual bool isBrowserLauncherVisible() const;
    virtual bool setFocusToBrowserLauncher(bool focus);
    virtual bool isBrowserLauncherFocused() const;

    bool startMediaPlayer(cabot::String const& url, bool const linear_ip_service) override;
    bool isAppRunningByType(WebBrowserAppType const app_type) const override;

    bool getMediaPlayerPosition(double& x,
                                double& y,
                                double& width,
                                double& height) override;
                               
    bool setMediaPlayerPosition(double x,
                                double y,
                                double width,
                                double height) override;
    
    void setRestartAfterTermination(bool const restart_flag) override;

#ifdef INCLUDE_HOTELTV
    virtual void sendPlatformMessageInJsonFormat();
    virtual void addSendPlatformMessageParameter(cabot::String first_param, cabot::String second_param);
    virtual void clearSendPlatformMessageParameter();
    virtual void setHbbAppStartedByUser(bool enable);
    virtual bool isHbbAppStartedByUser();
#endif

public:
    // Implement AnyWebBrowserKeyHandler.
    virtual std::uint32_t getKeyset();
    virtual void setKeyset(std::uint32_t keyset);
    virtual std::uint32_t getAvailableKeyset();

public:
    // Implement AnyWebBrowserEventPublisher
    virtual bool registerListener(AnyWebBrowserEventListener* listener);
    virtual bool deregisterListener(AnyWebBrowserEventListener* listener);

    // AnyWebBrowserHbbTVEventPublisher
    void registerHbbTVListener(AnyWebBrowserHbbTVEventListener* listener) override;
    void deregisterHbbTVListener(AnyWebBrowserHbbTVEventListener* listener) override;

    // AnyWebBrowserMediaPlayerEventPublisher
    void registerMediaPlayerListener(AnyWebBrowserMediaPlayerEventListener* listener) override;
    void deregisterMediaPlayerListener(AnyWebBrowserMediaPlayerEventListener* listener) override;
    
    // Implement AnyWebBrowserOpAppEventPublisher interface.
    void registerOpAppListener(AnyWebBrowserOpAppEventListener & listener) override;
    void deregisterOpAppListener(AnyWebBrowserOpAppEventListener & listener) override;

     // Implement AnyWebBrowserOipfEventPublisher interface.
    void registerOipfListener(AnyWebBrowserOipfEventListener* listener) override;
    void deregisterOipfListener(AnyWebBrowserOipfEventListener* listener) override;

public:
    // Implement AnyWebBrowserMouseController
    virtual bool deactivateMouse();
    virtual bool activateMouse();
    virtual bool activateAirMouse();
    virtual bool deactivateAirMouse();

protected:
    // Overriding NullWebBrowserEventListener
    void notifyBrowserProcessStateChange(BrowserProcessState state) override;

private:
    bool activateMouseLocked();
    bool deactivateMouseLocked();

public:
    // Implement AnyWebBrowserTvPreferencesChangeListener.
    void update(WebBrowserTvPreferencesChange const & change);

public:
    virtual bool isMimeTypeSupported(cabot::String const& mime_type) const;
    virtual bool isMimeTypeSupportedAvailable() const;
    virtual bool accessCriteriaChanged(cabot::String const& message) const;    

private:
    bool isKeyForDirection(nebula::KeyCode key);
    bool isColoredKey(nebula::KeyCode key);
    cabot::String nebulaLanguageCodeToOperaLanguageCode(NEBULA_LanguageCode nebula_language_code);
    bool terminateCurrentAppLocked(bool invalidate_ait = false);

private:
    // Copy and assign not allowed.
    OperaBrowserCore(OperaBrowserCore const&);
    OperaBrowserCore& operator=(OperaBrowserCore const&);

    friend class OperaInitialiseCommand;
    friend class OperaFinaliseCommand;

    bool initialiseInternal(
                    AnyWebBrowserUIController& ui_controller,
                    AnyWebBrowserProgressIndicator& progress_indicator,
                    AnyWebBrowserUserAgentProvider& user_agent_provider,
                    AnyWebBrowserInternetStatusProvider& internet_status_provider,
                    AnyWebBrowserTvPreferencesProvider& tv_preferences_provider,
                    AnyWebBrowserTvPreferencesChangePublisher & tv_preferences_change_publisher,
                    AnyWebBrowserAitHandler & ait_handler,
                    AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider);
    bool finaliseInternal();

    void registerForTvPreferencesChangeNotifications(
                    AnyWebBrowserTvPreferencesChangePublisher & change_publisher);
    void deregisterForTvPreferencesChangeNotifications();

    enum class HbbTVEnabledStatus
    {
        unknown,
        disabled,
        enabled
    };
    HbbTVEnabledStatus m_hbbtv_enabled;
    bool m_hbbtv_suspended_temporarily;

    OperaEngine                     m_opera_engine;
    AnyOperaGraphicsBackend*        m_opera_graphics;
    nebula::LanguageSetting         m_language_setting;
    nebula::SoundSettings           m_sound_settings;
    nebula::SubtitleSettings        m_subtitle_settings;
    nebula::NetworkSettings         m_network_settings;
    OperaMouseController            m_mouse_controller;
    BrowserEventPublisher           m_event_publisher;
    OperaHbbTVBrowserEventPublisher m_hbbtv_event_publisher;
    OperaMediaPlayerEventPublisher  m_media_player_event_publisher;
    OperaOipfBrowserEventPublisher  m_oipf_event_publisher;
#ifdef INCLUDE_OPAPP_SUPPORT
    OperaOpAppBrowserEventPublisher m_opapp_event_publisher;
#endif
    EventDispatchConfigurer*        m_event_dispatch_configurer;
    OperaDsmccEngine*               m_opera_dsmcc_engine;
    OperaCertificateManager*        m_opera_certificate_manager;
    CommandThread                   m_dsmcc_thread;
    OperaHbbTVParentalRatingChecker m_parental_rating_checker;
    OperaHbbTVUIDialogHelper        m_ui_dialog_helper;
    AnyWebBrowserTvPreferencesChangePublisher * m_tv_preferences_change_publisher;
    VewdAuroraIpcServer*            m_vewd_aurora_ipc_server;

    IPProxyParams m_proxy_params;

    mutable FrostMutex m_browser_mutex;

    PrioritisedCommandThread m_initialization_command_thread;
    CommandThread m_event_thread;

    AnyWebBrowserAitHandler* m_ait_handler;
    bool m_initialise_browser_synchronously;

    NEBULA_Channel_ID m_last_selected_service_id;
};
#endif
