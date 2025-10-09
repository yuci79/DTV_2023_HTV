/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaBrowserCore class.
 */

#include "config/using_sdk420.h"
#include "config/using_sdk422.h"
#include "config/using_sdk423.h"
#include "config/include_dial_support.h"
#include "config/include_opapp_support.h"
#include "config/include_hoteltv.h"

#include "OperaBrowserCore.hpp"

#include "language/LanguageCode.hpp"

#include "eclipse/core/MouseTypes.hpp"
#include "frost/frost_standards.h"
#include "nebula/adaptation_layer/nebula_configuration.h"
#include "nebula/adaptation_layer/nebula_tuner_configuration.h"
#include "nebula/adaptation_layer/nebula_mheg.h"
#include "nebula/adaptation_layer/nebula_channel_info.h"
#include "nebula/browser_interface/AnyWebBrowserTvPreferencesProvider.hpp"
#include "nebula/core/ait_hub/ApplicationIterator.hpp"
#include "nebula/core/ait_hub/ApplicationParser.hpp"
#include "nebula/core/ait_hub/MPEGTypes.hpp"
#include "nebula/core/browser_client/LanguageSetting.hpp"
#include "nebula/browser_interface/AnyWebBrowserAitHandler.hpp"

#include "vewd_common/OperaMouseController.hpp"
#include "vewd_common/bpi/bpi_internet_status.hpp"
#include "vewd_common/bpi/bpi_preferences.hpp"
#include "vewd_common/bpi/bpi_device_info.hpp"
#include "vewd_common/bpi/bpi_profile.hpp"
#include "vewd_common/bpi/bpi_nativeui_controller.hpp"
#include "vewd_common/bpi/bpi_graphics.hpp"
#if defined USING_SDK422 || defined USING_SDK423
#include "vewd_common/ipc/aurora/VewdAuroraIpcServer.hpp"
#include "vewd_integration/jspp_plugins/ipc/aurora/NativeOperaWindowManagerIpcAurora.hpp"
#include "vewd_integration/jspp_plugins/ipc/aurora/NativeOperaWindowManagerCallbacksIpcAurora.hpp"
#include "vewd_integration/ipc/aurora/OperaWindowManagerProxyIpcAurora.hpp"
#endif
#include "vewd_integration/graphics/AnyOperaGraphicsBackend.hpp"
#include "vewd_integration/graphics/OperaGraphicsBackendFactory.hpp"
#include "vewd_integration/oipf/NetworkSettingsEventPublisher.hpp"
#include "vewd_integration/oipf/ChannelConfigEventsPublisher.hpp"
#include "vewd_integration/oipf/PvrEventsPublisher.hpp"
#include "vewd_integration/oipf/SystemTimeUpdatedEventPublisher.hpp"
#include "vewd_integration/oipf/EventDispatchConfigurer.hpp"
#include "utilities_public/AnyCommand.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"

#include "frost/frost_time.h"

#include <stdlib.h>

#include <algorithm>

TRACE_IMPLEMENT("opera/browsercore");

#if !defined USING_SDK422 && !defined USING_SDK423
// These functions are required to stop unreferenced classes/functions from being removed
// at link time, that are required at runtime by Vewd shared libraries.
extern void init_opera_mumin_object_factory();
extern void init_opera_mumin_oipf_backend_factory();
#endif
extern void macIDPluginHack();
extern void md5PluginHack();

namespace
{

/**
 * @brief Convert a language code into ISO 639-1, where possible.
 * 
 * The Opera SDK may not support ISO 639-2 codes, so this function can be used
 * to perform the conversion.
 * 
 * @param language The code to convert in place.
 */
void
convertToIso639Dash1(NEBULA_LanguageCode & language)
{
    LanguageCode dvb_language(language.iso_str);

    // This code for Flemish is not in the ISO 639 standard but it is still
    // used, so convert it into a known code to allow successful conversion.
    if (dvb_language == LanguageCode("fle"))
    {
        dvb_language = LanguageCode("dut");
    }
    
    // This converts to a 2-character (i.e. ISO 639-1) language code (it
    // chooses the shortest code where there are multiple options).
    dvb_language.convertToNonBiblStr(language.iso_str);
}

#if defined USING_SDK422 || defined USING_SDK423
/**
 * @brief Bind to the VewdAuroraIpcServer.
 * 
 * Call bindToServer() for all classes in vewd_integration which need to 
 * bind to the VewdAuroraIpcServer.
 * 
 * @param server An rpc::server ready to accept bindings.
 */
void
bindVewdAuroraIpcServer(rpc::server& server)
{
    NativeOperaWindowManagerIpcAurora::bindToServer(server);
    NativeOperaWindowManagerCallbacksIpcAurora::bindToServer(server);
    OperaWindowManagerProxyIpcAurora::bindToServer(server);

    // Add more here...
}

void
resetVewdAuroraIpcCallbacks()
{
    TRACE_INFO(("%s()\n", __func__));

    NativeOperaWindowManagerCallbacksIpcAurora::reset();
    
    // Add more here...
}
#endif

} // namespace

////////////////////////////////////////////////////////////////////////////////

class OperaInitialiseCommand : public AnyCommand
{
public:
    OperaInitialiseCommand(
            OperaBrowserCore & browser_core,
            AnyWebBrowserUIController& ui_controller,
            AnyWebBrowserProgressIndicator& progress_indicator,
            AnyWebBrowserUserAgentProvider& user_agent_provider,
            AnyWebBrowserInternetStatusProvider& internet_status_provider,
            AnyWebBrowserTvPreferencesProvider& tv_preferences_provider,
            AnyWebBrowserTvPreferencesChangePublisher & tv_preferences_change_publisher,
            AnyWebBrowserAitHandler & ait_handler,
            AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider);

    virtual void execute();

private:
    OperaInitialiseCommand& operator=(OperaInitialiseCommand const&);

private:
        OperaBrowserCore & m_browser_core;
        AnyWebBrowserUIController & m_ui_controller;
        AnyWebBrowserProgressIndicator & m_progress_indicator;
        AnyWebBrowserUserAgentProvider & m_user_agent_provider;
        AnyWebBrowserInternetStatusProvider & m_internet_status_provider;
        AnyWebBrowserTvPreferencesProvider & m_tv_preferences_provider;
        AnyWebBrowserTvPreferencesChangePublisher & m_tv_preferences_change_publisher;
        AnyWebBrowserAitHandler & m_ait_handler;
        AnyWebBrowserOpAppConfigurationProvider const & m_opapp_configuration_provider;
};

OperaInitialiseCommand::OperaInitialiseCommand(
        OperaBrowserCore & browser_core,
        AnyWebBrowserUIController & ui_controller,
        AnyWebBrowserProgressIndicator & progress_indicator,
        AnyWebBrowserUserAgentProvider & user_agent_provider,
        AnyWebBrowserInternetStatusProvider & internet_status_provider,
        AnyWebBrowserTvPreferencesProvider & tv_preferences_provider,
        AnyWebBrowserTvPreferencesChangePublisher & tv_preferences_change_publisher,
        AnyWebBrowserAitHandler & ait_handler,
        AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider
        )
    : m_browser_core(browser_core)
    , m_ui_controller(ui_controller)
    , m_progress_indicator(progress_indicator)
    , m_user_agent_provider(user_agent_provider)
    , m_internet_status_provider(internet_status_provider)
    , m_tv_preferences_provider(tv_preferences_provider)
    , m_tv_preferences_change_publisher(tv_preferences_change_publisher)
    , m_ait_handler(ait_handler)
    , m_opapp_configuration_provider(opapp_configuration_provider)
{
}

void OperaInitialiseCommand::execute()
{
    m_browser_core.initialiseInternal(m_ui_controller,
            m_progress_indicator,
            m_user_agent_provider,
            m_internet_status_provider,
            m_tv_preferences_provider,
            m_tv_preferences_change_publisher,
            m_ait_handler,
            m_opapp_configuration_provider);
}
////////////////////////////////////////////////////////////////////////////////

#include "frost/frost_semaphore.h"

class CommandResultLock
{
public:
    CommandResultLock();
    ~CommandResultLock();

    bool getResult();
    void setResult(bool result);
    bool isDestroyable();

private:
    CommandResultLock(const CommandResultLock &); //not impl.
    CommandResultLock& operator=(const CommandResultLock &); //not impl.

private:
    FROST_Semaphore* m_lock;
    bool m_result;
    bool m_getResult_called;
};

CommandResultLock::CommandResultLock()
: m_result(false),
  m_getResult_called(false)
{
    m_lock = FROST_SemaphoreCreate(0);
    if (m_lock == 0)
    {
        TRACE_ERROR(("CommandResultLock FROST_SemaphoreCreate failed !\n"));
    }
}

CommandResultLock::~CommandResultLock()
{
    FROST_SemaphoreDestroy(m_lock);
}

bool
CommandResultLock::isDestroyable()
{
    return m_getResult_called;
}

bool
CommandResultLock::getResult()
{
    FROST_SemaphoreWaitTimeout(m_lock, 10000);

    bool result = m_result;
    FROST_SemaphoreSignal(m_lock);
    m_getResult_called = true;
    return result;
}

void
CommandResultLock::setResult(bool result)
{
    m_result = result;
    FROST_SemaphoreSignal(m_lock);
}


class OperaFinaliseCommand : public AnyCommand
{
public:
    OperaFinaliseCommand(OperaBrowserCore& opera_browser_core, CommandResultLock* result_lock);
    void execute();
private:
    OperaFinaliseCommand& operator=(OperaFinaliseCommand const&);
    OperaFinaliseCommand(OperaFinaliseCommand const&);
private:
    OperaBrowserCore&  m_opera_browser_core;
    CommandResultLock* m_result_lock;
};

OperaFinaliseCommand::OperaFinaliseCommand(OperaBrowserCore& opera_browser_core,
                                           CommandResultLock* result_lock)
    : m_opera_browser_core(opera_browser_core)
    , m_result_lock(result_lock)
{}

void
OperaFinaliseCommand::execute()
{
    m_result_lock->setResult(m_opera_browser_core.finaliseInternal());
    while (m_result_lock->isDestroyable() == false)
    {
        FROST_Sleep(10);
    }
    delete m_result_lock;
    m_result_lock = 0;
}
////////////////////////////////////////////////////////////////////////////////


OperaBrowserCore::OperaBrowserCore()
    : m_opera_engine()
    , m_opera_graphics(nullptr)
    , m_opera_dsmcc_engine(nullptr)
    , m_hbbtv_enabled(HbbTVEnabledStatus::unknown)
    , m_hbbtv_suspended_temporarily(false)
    , m_event_dispatch_configurer(nullptr)
    , m_opera_certificate_manager(nullptr)
    , m_tv_preferences_change_publisher(nullptr)
    , m_initialise_browser_synchronously(false)
    , m_ait_handler(nullptr)
    , m_vewd_aurora_ipc_server(nullptr)
{
    if (! m_browser_mutex.initialise())
    {
        TRACE_ERROR(("m_browser_mutex could not be initialised\n"));
    }
}

OperaBrowserCore::~OperaBrowserCore()
{
    NullWebBrowserEventListener::setPublisher(nullptr);

    delete m_opera_graphics;
    delete m_opera_dsmcc_engine;
    delete m_opera_certificate_manager;
#if !defined USING_SDK422 && !defined USING_SDK423
    delete m_event_dispatch_configurer;
#endif
#if defined USING_SDK422 || defined USING_SDK423
    delete m_vewd_aurora_ipc_server;
#endif
}

bool
OperaBrowserCore::initialise(
        AnyWebBrowserUIController& ui_controller,
        AnyWebBrowserProgressIndicator& progress_indicator,
        AnyWebBrowserUserAgentProvider& user_agent_provider,
        AnyWebBrowserInternetStatusProvider& internet_status_provider,
        AnyWebBrowserTvPreferencesProvider& tv_preferences_provider,
        AnyWebBrowserTvPreferencesChangePublisher & tv_preferences_change_publisher,
        AnyWebBrowserAitHandler & ait_handler,
        AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider,
        bool initialise_browser_synchronously)
{

    if (!m_initialization_command_thread.initialise("OperaBrowserCore",
            FROST_highest_priority,
            128 * 1024))
    {
        return false;
    }

    TRACE_INFO(("%s SYNC: %d\n", __FUNCTION__, initialise_browser_synchronously));

    m_initialise_browser_synchronously = initialise_browser_synchronously;

    if (initialise_browser_synchronously)
    {
        return initialiseInternal(ui_controller,
                                  progress_indicator,
                                  user_agent_provider,
                                  internet_status_provider,
                                  tv_preferences_provider,
                                  tv_preferences_change_publisher,
                                  ait_handler,
                                  opapp_configuration_provider);
    }
    else
    {
        return m_initialization_command_thread.enqueueCommand(
                new OperaInitialiseCommand(*this, ui_controller,
                                                  progress_indicator,
                                                  user_agent_provider,
                                                  internet_status_provider,
                                                  tv_preferences_provider,
                                                  tv_preferences_change_publisher,
                                                  ait_handler,
                                                  opapp_configuration_provider));
    }
}

bool
OperaBrowserCore::initialiseInternal(
                    AnyWebBrowserUIController& ui_controller,
                    AnyWebBrowserProgressIndicator& progress_indicator,
                    AnyWebBrowserUserAgentProvider& user_agent_provider,
                    AnyWebBrowserInternetStatusProvider& internet_status_provider,
                    AnyWebBrowserTvPreferencesProvider& tv_preferences_provider,
                    AnyWebBrowserTvPreferencesChangePublisher & tv_preferences_change_publisher,
                    AnyWebBrowserAitHandler & ait_handler,
                    AnyWebBrowserOpAppConfigurationProvider const & opapp_configuration_provider)
{
    TRACE_INFO(("initialiseInternal() \n"));
    m_ait_handler = &ait_handler;
#if !defined USING_SDK422 && !defined USING_SDK423
    init_opera_mumin_object_factory();
    init_opera_mumin_oipf_backend_factory();
#endif
    macIDPluginHack();
    md5PluginHack();

#if !defined USING_SDK422 && !defined USING_SDK423
    if (!m_event_dispatch_configurer)
    {
        m_event_dispatch_configurer = new EventDispatchConfigurer();
        if (!m_event_dispatch_configurer->initialise())
        {
            return false;
        }
    }
#endif

#if defined USING_SDK422 || defined USING_SDK423
    RpcBoundFunction rpc_bind_function = bindVewdAuroraIpcServer;
    RpcResetFunction rpc_reset_function = resetVewdAuroraIpcCallbacks;
    m_vewd_aurora_ipc_server = new VewdAuroraIpcServer(rpc_bind_function, rpc_reset_function);

    NullWebBrowserEventListener::setPublisher(this);
#endif

    if (!m_event_publisher.initialise())
    {
        TRACE_ERROR(("Failed to initialise event publisher\n"));
        return false;
    }

    OperaOpAppBrowserEventPublisher * opapp_event_publisher = nullptr;
#ifdef INCLUDE_OPAPP_SUPPORT
    opapp_event_publisher = &m_opapp_event_publisher;
#endif
    
    const std::uint16_t hbbtv_application_profile = 
        tv_preferences_provider.getHbbTvApplicationProfile();
    const int hbbtv_profile = tv_preferences_provider.getHbbTvProfile();
    if (m_opera_engine.initialise(&user_agent_provider, 
                                  &m_event_publisher, 
                                  &m_hbbtv_event_publisher,
                                  &m_media_player_event_publisher,
                                  opapp_event_publisher,
                                  &opapp_configuration_provider,
                                  &ait_handler,
                                  hbbtv_application_profile,
                                  hbbtv_profile,
                                  &m_oipf_event_publisher))
    {
        m_opera_graphics = createGraphicsBackend(m_opera_engine);
        if (!m_opera_graphics || !m_opera_graphics->initialise())
        {
            TRACE_ERROR(("Failed to initialise the browser's graph backend\n"));
            return false;
        }

        m_opera_engine.setMouseMaxBounds(m_opera_graphics->width(), m_opera_graphics->height());
        m_opera_engine.setPlaneWidthHeight(m_opera_graphics->width(), m_opera_graphics->height());

        m_opera_engine.setProgressIndicator(&progress_indicator);

        BPI_DeviceInfoInitialise();
        BPI_ProfileInitialise();
        BPI_NativeUiControllerSetUiController(&ui_controller);
        BPI_PreferencesSetPreferenceProvider(&tv_preferences_provider);
        BPI_InternetStatusSetInternetStatusProvider(&internet_status_provider);
        BPI_GraphicsInitialiseNativeSize(m_opera_graphics->width(), m_opera_graphics->height());

        m_mouse_controller.initialize();
        m_mouse_controller.registerBrowserCore(this);

        if (!m_parental_rating_checker.initialise(&ui_controller))
        {
            TRACE_ERROR(("Failed to initialise parental rating checker\n"));
            return false;
        }

        m_opera_engine.setParentalRatingChecker(&m_parental_rating_checker);

        if (!m_ui_dialog_helper.initialise(&ui_controller))
        {
            TRACE_ERROR(("Failed to initialise ui dialog helper\n"));
            return false;
        }

        m_opera_engine.setUIDialogHelper(&m_ui_dialog_helper);

        if (!m_event_thread.initialise("OperaBrowserEvent",
                                       FROST_medium_priority, 512, 16))
        {
            TRACE_ERROR(("initialiseInternal() could not create event thread"));
            return false;
        }
#ifdef INCLUDE_OPAPP_SUPPORT
        if (!m_opapp_event_publisher.initialise(m_event_thread))
        {
            TRACE_ERROR(("Failed to initialise HBBTV operator application event publisher\n"));
            return false;
        }
#endif

        if (!m_opera_engine.start())
        {
            TRACE_INFO(("Failed to start browser engine\n"));
            return false;
        }

        if (!m_initialise_browser_synchronously)
        {
            m_last_selected_service_id = NEBULA_GetCurrentChannel();
            TRACE_INFO(("%s() m_last_selected_service_id: %d\n", 
                        __FUNCTION__, m_last_selected_service_id));
        }

        if (!m_opera_dsmcc_engine)
        {
            if (!m_dsmcc_thread.initialise("OperaDsmccThread",
                                 FROST_high_priority,
                                 64 * 1024))
            {
                TRACE_ERROR(("initialiseInternal() could not create thread"));
                return false;
            }
            m_opera_dsmcc_engine = new OperaDsmccEngine(m_opera_engine, m_dsmcc_thread);

            if (!m_opera_dsmcc_engine)
            {
                TRACE_ERROR(("initialiseInternal(): Cannot create dsmcc engine \n"));
                return false;
            }
            if (!m_opera_dsmcc_engine->initialise())
            {
                TRACE_ERROR(("initialise(): Cannot initialise dsmcc handler \n"));
                delete m_opera_dsmcc_engine;
                m_opera_dsmcc_engine = nullptr;
                return false;
            }
        }
        
        if (!m_hbbtv_event_publisher.initialise(m_event_thread))
        {
            TRACE_ERROR(("Failed to initialise HBBTV event publisher\n"));
            return false;
        }

        if (!m_media_player_event_publisher.initialise(m_event_thread))
        {
            TRACE_ERROR(("Failed to initialise Media Player event publisher\n"));
            return false;
        }

        if (!m_oipf_event_publisher.initialise(m_event_thread))
        {
            TRACE_ERROR(("Failed to initialise OIPF event publisher\n"));
            return false;
        }

        if (!ait_handler.initialise(this))
        {
            TRACE_ERROR(("Failed to iniitalise ait handler\n"));
            return false;
        }

        if (!m_opera_certificate_manager)
        {
            m_opera_certificate_manager = new OperaCertificateManager(m_opera_engine);

            if (!m_opera_certificate_manager)
            {
                TRACE_ERROR(("initialiseInternal(): Cannot create certificate manager \n"));
                return false;
            }
            if (! m_opera_certificate_manager->initialise())
            {
                TRACE_ERROR(("initialiseInternal(): Cannot initialise certificate manager\n"));
                delete m_opera_certificate_manager;
                m_opera_certificate_manager = nullptr;
                return false;
            }
            m_opera_certificate_manager->setBrowserEventPublisher(&m_event_publisher);
        }

        notifySystemLanguageUpdated();

        registerForTvPreferencesChangeNotifications(tv_preferences_change_publisher);

        m_hbbtv_event_publisher.notifyHbbTVInitialised();
        
        // call to onBrowserInitComplete moved from OperaEngine::initialise to here, as otherwise 
        // it is possible that other modules will act upon the notification before 
        // OperaBrowserCore is also initialised for example before it has populated 
        // the m_last_selected_service_id
        // SHREDDER-7787 and SHREDDER-8005
        m_opera_engine.onBrowserInitComplete();

        TRACE_INFO(("initialiseInternal() SUCCESS \n"));
        return true;
    }
    TRACE_ERROR(("initialiseInternal() FAILED!\n"));
    return false;
}

void
OperaBrowserCore::finalise()
{
    TRACE_INFO(("Finalising\n"));

    CommandResultLock* result = new CommandResultLock();
    if(!m_initialization_command_thread.enqueueCommand(new OperaFinaliseCommand(*this, result)))
    {
        TRACE_ERROR(("Could not enqueue OperaFinaliseCommand\n"));
        finaliseInternal();
    }
    else
    {
        const bool finalised = result->getResult();
        if (finalised)
        {
            TRACE_INFO(("%s() Finalization is successful.\n", __FUNCTION__));
        }
        else
        {
            TRACE_ERROR(("%s() Finalization is not successful.\n", __FUNCTION__));
        }
    }
}

bool
OperaBrowserCore::finaliseInternal()
{
    FrostMutexLock lock(m_browser_mutex);

    m_mouse_controller.stopCapture();
    m_opera_engine.stop();
    m_opera_engine.terminate();

#if !defined USING_SDK422 && !defined USING_SDK423
    if (m_event_dispatch_configurer)
    {
        m_event_dispatch_configurer->terminate();
    }
#endif

    deregisterForTvPreferencesChangeNotifications();
    return true;
}

 
bool
OperaBrowserCore::handleKeyPress(nebula::KeyCode key)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    if (m_mouse_controller.isMouseEnabled())
    {
        const OperaWindowManager* window_manager = m_opera_engine.getWindowManager();
        if (window_manager)
        {
            const OperaWindow* active_window = window_manager->getActiveWindow();
            if (active_window && active_window->type() != OperaWindow::WindowType::window_browser_ui)
            {
                if (isKeyForDirection(key) || isColoredKey(key))
                {
                    m_mouse_controller.setMouseEnabled(false);
                }
                else if (key == nebula::Enter || key == nebula::KeySelect)
                {
                    m_opera_engine.sendMouseClick();
                    return true;
                }
            }
            else
            {
                if (m_mouse_controller.isAirMouseEnabled())
                {
                    if (key == nebula::KeySelect)
                    {
                        m_opera_engine.sendMouseClick();
                        return true;
                    }
                }
            }
        }
    }

    return m_opera_engine.handleKeyInput(key);
}
 
bool
OperaBrowserCore::handleKeyPressWithDownUpKeys(nebula::KeyCode key, bool key_released)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    if (m_mouse_controller.isMouseEnabled())
    {
        const OperaWindowManager* window_manager = m_opera_engine.getWindowManager();
        if (window_manager)
        {
            const OperaWindow* active_window = window_manager->getActiveWindow();
            if (active_window && active_window->type() != OperaWindow::WindowType::window_browser_ui)
            {
                if (isKeyForDirection(key) || isColoredKey(key))
                {
                    m_mouse_controller.setMouseEnabled(false);
                }
                else if (key == nebula::Enter || key == nebula::KeySelect)
                {
                    m_opera_engine.sendMouseClick();
                    return true;
                }
            }
            else
            {
                if (m_mouse_controller.isAirMouseEnabled())
                {
                    if (key == nebula::KeySelect)
                    {
                        m_opera_engine.sendMouseClick();
                        return true;
                    }
                }
            }
        }
    }

    return m_opera_engine.handleKeyInputWithDownUpKeys(key, key_released);
}

bool
OperaBrowserCore::isKeyForDirection(nebula::KeyCode key)
{
    if (key == nebula::KeyDown  || key == nebula::KeyUp   ||
        key == nebula::KeyRight || key == nebula::KeyLeft)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
OperaBrowserCore::isColoredKey(nebula::KeyCode key)
{
    if (key == nebula::KeyRed  || key == nebula::KeyBlue   ||
        key == nebula::KeyGreen || key == nebula::KeyYellow)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
OperaBrowserCore::createApp(cabot::String const& url, 
                            bool parse_ait_xml, 
                            bool broadcast_related, 
                            bool hbbtv_application,
                            bool hbbtv_service,
                            bool allow_auto_transit)
{
    FrostMutexLock lock(m_browser_mutex);
    
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    // check that the BrowserCore has the hbbtv enabled 
    if (m_hbbtv_enabled != HbbTVEnabledStatus::enabled && (hbbtv_application || hbbtv_service))
    {
        hbbtv_application = false;
        hbbtv_service = false;
        TRACE_WARN(("HbbTV application requested however HbbTV not enabled, setting to false\n"));
    }
    return m_opera_engine.openUrl(url, parse_ait_xml, broadcast_related, hbbtv_application, hbbtv_service, allow_auto_transit);
}

bool
OperaBrowserCore::terminateCurrentApp(bool invalidate_ait)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return terminateCurrentAppLocked(invalidate_ait);
}

bool
OperaBrowserCore::terminateCurrentApp(
    WebBrowserAppType const app_type,
    bool const invalidate_ait)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.terminateCurrentApp(app_type, invalidate_ait);
}

void
OperaBrowserCore::setApmStarted(bool apm_started)
{
    m_opera_engine.setApmStarted(apm_started);
}

bool
OperaBrowserCore::isAnyAppStartingClosedURL()
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isAnyAppStartingClosedURL();
}

bool
OperaBrowserCore::terminateCurrentAppLocked(bool invalidate_ait)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    bool set_select_service_upon_termination = true;
    if (isSpecialAppStarting() || isTvStoreAppRestarting() || isExternalSourceSelected())
    {
        set_select_service_upon_termination = false;
    }
    m_opera_engine.terminateCurrentApp(set_select_service_upon_termination, invalidate_ait);
    return true;
}

#ifdef INCLUDE_HOTELTV
void
OperaBrowserCore::setVisible(bool visible)
{
    m_opera_engine.setVisible(visible);
}
#endif

WebBrowserAppType
OperaBrowserCore::getCurrentAppType()
{
    /*
     * Note: The mutex has been removed here because it resulted in a deadlock
     *       Relates to STEALTH-1890.
     *       OperaEngine::getCurrentAppType() provides its own protection.
     */
    //FrostMutexLock lock(m_browser_mutex);

    return m_opera_engine.getCurrentAppType();
}

bool
OperaBrowserCore::isTeletextApplicationStarted() const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isTeletextApplicationStarted();
}

bool
OperaBrowserCore::isAnyAppRunning()
{
    /*
     * Note: The mutex has been removed here because it resulted in a deadlock
     *       Relates to STEALTH-1890.
     *       OperaEngine::isAppRunning() provides its own protection.
     */
    //FrostMutexLock lock(m_browser_mutex);
    return m_opera_engine.isAppRunning();
}

std::vector<WebBrowserAppType>
OperaBrowserCore::runningAppTypes() const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return {};
    }
    return m_opera_engine.runningAppTypes();
}

bool 
OperaBrowserCore::isAppRunning(const WebBrowserAppType type) const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    } 
    return (m_opera_engine.getCurrentAppType() == type && m_opera_engine.isAppRunning());
}

bool 
OperaBrowserCore::isAppRunningByType(const WebBrowserAppType type) const
{
    /*
     * Note: The mutex has been removed here because it resulted in a deadlock
     *       from within terminateCurrentApp() which had already locked the same mutex.
     *       Relates to STEALTH-1871.
     *       No lock was needed as OperaEngine::isAppRunningByType() provides its own protection.
     */
    //FrostMutexLock lock(m_browser_mutex);
    
    return (m_opera_engine.isAppRunningByType(type));
}

bool 
OperaBrowserCore::isMediaPlaying()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isMediaPlaying();
}

bool 
OperaBrowserCore::isHbbTvAppStarting() const 
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isHbbTvAppStarting();
}

bool
OperaBrowserCore::isDigitalIbCloseRequested()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isDigitalIbCloseRequested();
}

void
OperaBrowserCore::resetDigitalIbCloseRequest()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.resetDigitalIbCloseRequest();
}

void
OperaBrowserCore::handleAppCertificateError(bool allow, unsigned int error_id)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    m_opera_certificate_manager->handleAppCertificateError(allow, error_id);
}

void
OperaBrowserCore::getUIApp(cabot::String& application_url)
{
    FrostMutexLock lock(m_browser_mutex);
    return m_opera_engine.getUIApp(application_url);
}

bool
OperaBrowserCore::createUIApp(cabot::String const& application_url,
                              cabot::String const& overlay_application_url,
                              ApplicationResolution application_resolution)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.createUIApp(application_url,
                                      overlay_application_url,
                                      application_resolution);
}

bool
OperaBrowserCore::recreateUIApp()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.recreateUIApp();
}

bool
OperaBrowserCore::destroyUIApp()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    m_opera_engine.destroyUIApp();
    return true;
}

void
OperaBrowserCore::setVisibleUIApp(bool visible)
{
    FrostMutexLock lock(m_browser_mutex);
    m_opera_engine.setVisibleUIApp(visible);
}

bool
OperaBrowserCore::isUIAppVisible()
{
    FrostMutexLock lock(m_browser_mutex);
    return m_opera_engine.isUIAppVisible();
}

void
OperaBrowserCore::setVisibleAllWindows(bool visible)
{
    FrostMutexLock lock(m_browser_mutex);
    m_opera_engine.setVisibleAllWindows(visible);
}

void
OperaBrowserCore::setFocusToOpApp(bool focus)
{
    FrostMutexLock lock(m_browser_mutex);
    m_opera_engine.setFocusToOpApp(focus);
}

bool 
OperaBrowserCore::isOpAppFocused() const 
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isOpAppFocused();
}

bool
OperaBrowserCore::setOperatorAppDiscoveryEnabled(bool enabled)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.setOpAppDiscoveryEnabled(enabled);
}

bool
OperaBrowserCore::isOperatorAppDiscoveryEnabled() const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isOpAppDiscoveryEnabled();
}

bool
OperaBrowserCore::startOperatorAppDiscovery(
        nebula::OperatorApplicationDiscoveryProperties const & configuration)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.startOpAppDiscovery(configuration);
}

bool
OperaBrowserCore::stopOperatorAppDiscovery()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.stopOpAppDiscovery();
}

bool
OperaBrowserCore::uninstallOperatorApp(
                std::uint16_t application_id, std::uint32_t organisation_id)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.uninstallOpApp(application_id, organisation_id);
}

bool
OperaBrowserCore::setOperatorApplicationDiscoveryUris(
        std::vector<cabot::String> const & uris)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    m_opera_engine.setOperatorApplicationDiscoveryUris(uris);
    return true;
}

bool
OperaBrowserCore::startOperatorApp(
        nebula::OperatorApplicationProperties const & application)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.startOpApp(application);
}

bool
OperaBrowserCore::stopOperatorApp()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.stopOpApp();
}

void
OperaBrowserCore::suspendOperatorApp()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.suspendOpApp();
}

void
OperaBrowserCore::resumeOperatorApp()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.resumeOpApp();
}

bool
OperaBrowserCore::isOperatorAppSuspended() const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isOpAppSuspended();
}

bool
OperaBrowserCore::setOperatorAppState(
        nebula::OperatorApplicationState new_state)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.setOpAppState(new_state);
}

bool
OperaBrowserCore::setOperatorAppContext(const cabot::String& query_url)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.setOpAppContext(query_url);
}

bool
OperaBrowserCore::handleOperatorAppKeyPress(nebula::KeyCode key_code, 
                          nebula::KeyPressHandlingReason keypress_handling_reason)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.handleOpAppKeyInput(key_code, keypress_handling_reason);
}

bool
OperaBrowserCore::isHbbSystemSuspended() const
{
    FrostMutexLock lock(m_browser_mutex);
    return m_hbbtv_suspended_temporarily;
}

bool
OperaBrowserCore::suspendHbbSystem(bool const terminate_hbbtv_service,
                                   bool const bind_to_current_channel)
{
    TRACE_INFO(("Suspending HbbTV\n"));
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    if(m_hbbtv_enabled != HbbTVEnabledStatus::enabled)
    {
        TRACE_ERROR(("suspendHbbSystem: HbbTV features disabled, so can not be suspended\n"));
        return false;
    }
    m_hbbtv_suspended_temporarily = true;
    m_opera_engine.setSelectServiceUponAppTermination(bind_to_current_channel);
    m_opera_engine.enableHbbTv(false, terminate_hbbtv_service);

    return true;
}

bool
OperaBrowserCore::resumeHbbSystem()
{
    TRACE_INFO(("Resume HbbTV\n"));
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    if(m_hbbtv_enabled != HbbTVEnabledStatus::enabled)
    {
        TRACE_ERROR(("resumeHbbSystem: HbbTV features disabled, so can not be resumed\n"));
        return false;
    }
    m_hbbtv_suspended_temporarily = false;
    m_opera_engine.setSelectServiceUponAppTermination(true);
    m_opera_engine.enableHbbTv(true);
    
    return true;
}

bool
OperaBrowserCore::setHbbSystemEnabled(bool enable)
{
    bool result = m_opera_engine.initialisationCompleted();
    TRACE_INFO(("Hbb is %s enabled\n", enable ? "" : "NOT"));
    if (m_opera_engine.initialisationCompleted())
    {
        m_hbbtv_enabled = (enable ? HbbTVEnabledStatus::enabled : HbbTVEnabledStatus::disabled);
        m_opera_engine.enableHbbTv(enable);
    }
    else
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
    }
    return result;
}

bool 
OperaBrowserCore::hasHbbSystemBeenSet() const
{
    FrostMutexLock lock(m_browser_mutex);
    return (m_hbbtv_enabled != HbbTVEnabledStatus::unknown);
}

bool
OperaBrowserCore::isHbbSystemEnabled() const
{
    FrostMutexLock lock(m_browser_mutex);
    return (m_hbbtv_enabled == HbbTVEnabledStatus::enabled ? true : false);
}

bool
OperaBrowserCore::dnsSettingsUpdated(cabot::String const& dns_primary,
                                     cabot::String const& dns_secondary)
{
    // Opera does not care about this
    CABOT_UNUSED_PARAM(dns_primary);
    CABOT_UNUSED_PARAM(dns_secondary);
    return true;
}

bool
OperaBrowserCore::connectionStatusChanged(ConnectionStatus& new_status)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    bool connected = (new_status.connection_status == ConnectionStatus::connection_connected);

    TRACE_INFO(("Connection status has changed, connected '%d'\n", connected));

    ConnectionType conn_type = connection_type_unknown;

    switch (new_status.connection_type)
    {
    case ConnectionStatus::connection_type_wired:
        conn_type = connection_type_wired;
        break;
    case ConnectionStatus::connection_type_wireless:
        conn_type = connection_type_wireless;
        break;
    default:
        conn_type = connection_type_unknown;
        break;
    }

    m_opera_engine.onConnectionStatusChange(connected, conn_type, new_status.interface_name);
    notifyUserAgentUpdateRequested();
    return true;
}


bool
OperaBrowserCore::setHttpProxyParams(IPProxyParams const& proxy_params)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    m_proxy_params = proxy_params;
    UString proxy_str = "";
    if (proxy_params.m_use_proxy)
    {
        proxy_str = proxy_params.m_addr.charArray();
        proxy_str.append(":");
        proxy_str.append(UString::fromInt(proxy_params.m_port));
    }
    bool result = m_opera_engine.setHttpProxy(proxy_str);
    TRACE_INFO(("setHttpProxyParams proxy address = '%s' \n", proxy_str.charArray()));
    return result;
}

bool
OperaBrowserCore::getHttpProxyParams(IPProxyParams & proxy_params)
{
    proxy_params = m_proxy_params;
    return true;
}

bool
OperaBrowserCore::historyBack(int const steps)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    m_opera_engine.moveInHistory(steps);
    return true;
}

bool
OperaBrowserCore::isTeletextAvailable()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isTeletextAvailable();
}

bool
OperaBrowserCore::startTeletext()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.startTeletext();
}

bool
OperaBrowserCore::stopTeletext(bool launch_autostart_app)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.stopTeletext(launch_autostart_app);
}

void
OperaBrowserCore::setUsbKeyboardStateAttached(bool attached)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    m_opera_engine.setUsbKeyboardStateAttached(attached);
}

bool
OperaBrowserCore::setUrl(cabot::String const& url)
{
    return setUrl(url, false, false);
}

bool
OperaBrowserCore::setUrl(   cabot::String const& url, 
                            bool parse_ait_xml, 
                            bool broadcast_related)
{
    bool launch_in_hbbtv_window = false;
    bool hbbtv_service = false;
    return createApp(url, parse_ait_xml, broadcast_related, launch_in_hbbtv_window, hbbtv_service);
}

bool
OperaBrowserCore::openDvbIXmlAit(cabot::String const& xml_ait_url)
{
    FrostMutexLock lock(m_browser_mutex);
    
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.openDvbIXmlAit(xml_ait_url);
}

bool
OperaBrowserCore::createInternetBrowser(cabot::String const& start_url)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    activateMouseLocked();
    return m_opera_engine.startTvBrowser(start_url);
}

bool
OperaBrowserCore::destroyInternetBrowser()
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    deactivateMouseLocked();
    return m_opera_engine.stopTvBrowser();
}

bool
OperaBrowserCore::getInternetBrowserCurrentUrl(cabot::String & url)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.tvBrowserUrl(url);
}

bool
OperaBrowserCore::isInternetBrowserRunning()
{
    /*
     * Note: The mutex has been removed here because it resulted in a deadlock
     *       Relates to STEALTH-1890.
     *       OperaEngine::isTvBrowserRunning() provides its own protection.
     */
    //FrostMutexLock lock(m_browser_mutex);
    return m_opera_engine.isTvBrowserRunning();
}

bool
OperaBrowserCore::isInternetBrowserExitRequested() const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isTvBrowserExitRequested();
}

bool
OperaBrowserCore::isTvStoreAppRunning() const
{
    /*
     * Note: The mutex has been removed here because it resulted in a deadlock
     *       Relates to STEALTH-1890.
     *       OperaEngine::isTvStoreAppRunning() provides its own protection.
     */
    //FrostMutexLock lock(m_browser_mutex);
    return m_opera_engine.isTvStoreAppRunning();
}

bool
OperaBrowserCore::isTvStoreAppRestarting()
{
    //Note: The mutex has been removed here because it resulted in a deadlock from within
    //terminateCurrentApp() which had already locked the same mutex.
    //No lock was needed anyway because it is just reading OperaEngine's member variable, m_is_tv_store_app_restarting
    //FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.isTvStoreAppRestarting();
}

bool
OperaBrowserCore::isSpecialAppStarting() const
{
    //Note: The mutex has been removed here because it resulted in a deadlock from within
    //terminateCurrentApp() which had already locked the same mutex.
    //No lock was needed anyway because it is just reading OperaEngine's member variable, m_is_special_app_starting
    //FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isSpecialAppStarting();
}

void
OperaBrowserCore::setSpecialAppStarting(bool status)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    return m_opera_engine.setSpecialAppStarting(status);
}

bool
OperaBrowserCore::isExternalSourceSelected() const
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isExternalSourceSelected();
}

void
OperaBrowserCore::setExternalSourceSelected(bool selected)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    return m_opera_engine.setExternalSourceSelected(selected);
}

bool
OperaBrowserCore::initialiseMouse()
{
    return true;
}

bool
OperaBrowserCore::deactivateMouse()
{
    FrostMutexLock lock(m_browser_mutex);

    return deactivateMouseLocked();
}

bool
OperaBrowserCore::deactivateMouseLocked()
{
    m_mouse_controller.setMouseEnabled(false);
    return m_mouse_controller.stopCapture();
}

bool
OperaBrowserCore::activateMouse()
{
    FrostMutexLock lock(m_browser_mutex);

    return activateMouseLocked();
}

bool
OperaBrowserCore::activateMouseLocked()
{
    if (m_mouse_controller.startCapture())
    {
        m_mouse_controller.setMouseEnabled(true);
        return true;
    }
    return false;
}

bool
OperaBrowserCore::isMouseEnabled()
{
    return m_mouse_controller.isMouseEnabled();
}

void
OperaBrowserCore::mouseEvent(MouseEventData event)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    if (m_mouse_controller.isMouseEnabled())
    {
        // TODO: Mouse events are better queued
        m_opera_engine.onMouseEvent(event);
    }
    else
    {
        m_mouse_controller.readDisabledMouseData(event);
    }
}

bool
OperaBrowserCore::activateAirMouse()
{
    FrostMutexLock lock(m_browser_mutex);
    m_mouse_controller.setAirMouseEnabled(true);
    return true;
}

bool
OperaBrowserCore::deactivateAirMouse()
{
    FrostMutexLock lock(m_browser_mutex);
    m_mouse_controller.setAirMouseEnabled(false);
    return true;
}

bool
OperaBrowserCore::isInCache(cabot::String const & url)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.isInCache(url);
}

cabot::String
OperaBrowserCore::nebulaLanguageCodeToOperaLanguageCode(NEBULA_LanguageCode nebula_language_code)
{
    cabot::String language_code = nebula_language_code.iso_str;

    if (language_code == "es")
    {
        language_code.append("-ES");
    }
    return language_code;
}

void
OperaBrowserCore::notifySystemLanguageUpdated()
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    NEBULA_LanguageCode language_code;

    if (m_language_setting.getNonBiblSystemLanguage(&language_code))
    {
        cabot::String lang_code = nebulaLanguageCodeToOperaLanguageCode(language_code);
        // Language code "mon" is not in ISO 639-2 standard. Fix this when language code for montenegro is fixed.
        if(lang_code.compare("mn") == 0)
        {
            lang_code = "me";
        }
        m_opera_engine.updateHttpAcceptLang(lang_code.c_str());
        m_opera_engine.updateAppLocale(lang_code.c_str());
    }
}

void
OperaBrowserCore::notifyUserAgentUpdateRequested()
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.updateUserAgent();
}

void
OperaBrowserCore::updateDoNotTrackOption()
{
    int option = do_not_track_option_unset;
    if (NEBULA_IsDoNotTrackEnabled() == frost_true)
    {
        option = do_not_track_option_enabled;
    }
    //m_opera_engine.updateDoNotTrack(UString::fromInt(option));
    m_opera_engine.updateDoNotTrack(option==do_not_track_option_enabled ? "true" : "false");
}

std::uint32_t
OperaBrowserCore::getKeyset()
{
    /* unused */
    return 0;
}

void
OperaBrowserCore::setKeyset(std::uint32_t keyset)
{ 
    /* unused */
    CABOT_UNUSED_PARAM(keyset);
}

std::uint32_t
OperaBrowserCore::getAvailableKeyset()
{
    /* unused */
    return 0;
}

void OperaBrowserCore::serviceSelected(NEBULA_Channel_ID channel_id,
        NEBULA_Tuner_ID transport_stream_source_id,
        NEBULA_ChannelChangeResult channel_change_result)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
    }
    else
    {
        // re added for the merged ParentalRating logic that is using last_selected_service_id
        if (channel_change_result == NEBULA_ChannelChangeSuccess ||
            channel_change_result == NEBULA_ChannelChangeStarting)
        {
            if (channel_id != -1)
            {
                m_last_selected_service_id = channel_id;
            }
        }

        TRACE_INFO(("Service (%d) selected\n", channel_id));
        m_opera_engine.onServiceSelect(channel_id, transport_stream_source_id, channel_change_result);
    }
}

void 
OperaBrowserCore::clearAit()
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
    }
    else
    {
        m_opera_engine.clearAit();
    }  
}

bool
OperaBrowserCore::processAit(AitSectionPtr const& ait,
                             const bool pid_changed,
                             bool empty_pmt)
{
    bool passed_to_the_engine = false;
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
    }
    else
    {
        passed_to_the_engine = m_opera_engine.onAitUpdate(ait, pid_changed, empty_pmt);
    }
    return passed_to_the_engine;
}

bool 
OperaBrowserCore::registerListener(AnyWebBrowserEventListener* listener)
{
    m_event_publisher.attach(listener);
    return true;
}

bool
OperaBrowserCore::deregisterListener(AnyWebBrowserEventListener* listener)
{
    m_event_publisher.detach(listener);
    return true;
}

void
OperaBrowserCore::deregisterHbbTVListener(AnyWebBrowserHbbTVEventListener* listener)
{
    m_hbbtv_event_publisher.detach(listener);
}

void
OperaBrowserCore::registerHbbTVListener(AnyWebBrowserHbbTVEventListener* listener)
{
    m_hbbtv_event_publisher.attach(listener);
}

void 
OperaBrowserCore::registerMediaPlayerListener(AnyWebBrowserMediaPlayerEventListener* listener)
{
    m_media_player_event_publisher.attach(listener);
}

void 
OperaBrowserCore::deregisterMediaPlayerListener(AnyWebBrowserMediaPlayerEventListener* listener)
{
    m_media_player_event_publisher.detach(listener);
}
    
void
OperaBrowserCore::registerOpAppListener(AnyWebBrowserOpAppEventListener & listener)
{
#ifdef INCLUDE_OPAPP_SUPPORT
    m_opapp_event_publisher.attach(listener);
#else
    CABOT_UNUSED_PARAM(listener);
#endif
}

void
OperaBrowserCore::deregisterOpAppListener(AnyWebBrowserOpAppEventListener & listener)
{
#ifdef INCLUDE_OPAPP_SUPPORT
    m_opapp_event_publisher.detach(listener);
#else
    CABOT_UNUSED_PARAM(listener);
#endif
}

void
OperaBrowserCore::deregisterOipfListener(AnyWebBrowserOipfEventListener* listener)
{
    m_oipf_event_publisher.detach(listener);
}

void
OperaBrowserCore::registerOipfListener(AnyWebBrowserOipfEventListener* listener)
{
    m_oipf_event_publisher.attach(listener);
}

void
OperaBrowserCore::factoryReset()
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.clearOpAppFiles();
    m_opera_engine.clearPrivateData();
}

void
OperaBrowserCore::clearPersistentStorage(bool clear_cookies)
{
    TRACE_INFO(("clear private data before OAD\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.clearPrivateData(clear_cookies);
}

void
OperaBrowserCore::clearPrivateBrowserData()
{
    TRACE_INFO(("clear private data before OAD\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.clearPrivateBrowserData();
}

void
OperaBrowserCore::disableDiskCache()
{
    TRACE_INFO(("disable disk cache before OAD\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.disableDiskCache();
}

void
OperaBrowserCore::clearCookies()
{
    TRACE_INFO(("clear cookies\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.clearCookies();
}

std::vector<WebBrowserCookie>
OperaBrowserCore::getCookies()
{
    std::vector<WebBrowserCookie> operacookies;
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return operacookies;
    }
    else
    {
        return m_opera_engine.getCookies();
    }
}

bool 
OperaBrowserCore::deleteCookieByIndex(int index_to_delete)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    if (index_to_delete < 0)
    {
        return false;
    }
    return m_opera_engine.deleteCookieByIndex(index_to_delete);
}

void
OperaBrowserCore::disableCookies()
{
    TRACE_INFO(("disable cookies\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.disableCookies();
}

void
OperaBrowserCore::enableCookies()
{
    TRACE_INFO(("enable cookies\n"));
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    m_opera_engine.enableCookies();
}


AnyWebBrowserCore*
createWebBrowserCore()
{
    return new OperaBrowserCore;
}

#ifdef INCLUDE_DIAL_SUPPORT
void
OperaBrowserCore::setAdditionalDataUrlForYoutube(cabot::String additional_data_url)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return ;
    }
    m_opera_engine.setAdditionalDataUrlForYoutube(additional_data_url);
}
#endif

cabot::String
OperaBrowserCore::getYoutubeUrl(cabot::String launch_method)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return "";
    }
    return m_opera_engine.getYoutubeUrl(launch_method);
}

void
OperaBrowserCore::setCursorPosition(int x, int y)
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    MouseEventData mouse_event = {};

    mouse_event.x = x;
    mouse_event.y = y;
    mouse_event.is_position_absolute = true;

    m_opera_engine.onMouseEvent(mouse_event);
}

bool
OperaBrowserCore::getCursorPosition(cabot::String& cursor_position) const
{
    FrostMutexLock lock(m_browser_mutex);
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.getCursorPosition(cursor_position);
}

void OperaBrowserCore::dateTimeUpdated()
{}

bool 
OperaBrowserCore::isMimeTypeSupported(cabot::String const& mime_type) const
{
    CABOT_UNUSED_PARAM(mime_type);
    return false;
}

bool 
OperaBrowserCore::accessCriteriaChanged(cabot::String const& message) const
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    
    TRACE_INFO(("Access Criteria Changed\n"));
    return m_opera_engine.accessCriteriaChanged(message);
}

bool 
OperaBrowserCore::isMimeTypeSupportedAvailable() const
{
    return false;
}

void
OperaBrowserCore::focusHbbTvApplication(bool const focus_application)
{
    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    TRACE_INFO(("HbbTvApplication is now %s focused\n", focus_application ? "" : "not"));
    m_opera_engine.focusHbbtvApplication(focus_application);
}

bool
OperaBrowserCore::initialisationCompleted()
{
    return m_opera_engine.initialisationCompleted();
}

void
OperaBrowserCore::update(WebBrowserTvPreferencesChange const & change)
{
    switch (change.id())
    {
    case WebBrowserTvPreferencesChange::id_audio_description_enabled:
    case WebBrowserTvPreferencesChange::id_primary_audio_language:
    case WebBrowserTvPreferencesChange::id_primary_subtitle_language:
    case WebBrowserTvPreferencesChange::id_secondary_audio_language:
    case WebBrowserTvPreferencesChange::id_secondary_subtitle_language:
    case WebBrowserTvPreferencesChange::id_subtitles_enabled:
        setMediaComponentsPreferences();
        break;
    case WebBrowserTvPreferencesChange::id_wifi_profile_found:
    case WebBrowserTvPreferencesChange::id_selected_network_device:
    {
        // If network type is disabled by user or set as Wireless but no profile is saved,
        // set broadband status to browser as NOT_AVAILABLE.

        NEBULA_IPDevice selected_device = m_network_settings.getSelectedNetworkDevice();
        const bool profile_found = m_network_settings.anyWifiProfileSaved();
        if ( (selected_device == NEBULA_IPDeviceWireless && !profile_found)
                              || selected_device == NEBULA_IPDeviceUnknown )

        {
            ConnectionStatus conn_status;
            conn_status.connection_speed = 0;
            conn_status.connection_status = ConnectionStatus::connection_disconnected;
            conn_status.connection_type = ConnectionStatus::connection_type_unknown;
            conn_status.interface_name = "";
            m_ait_handler->notifyInternetStatusUpdated(0,0,0,0,conn_status.connection_status);
            (void)connectionStatusChanged(conn_status);
        }
        break;
    }
    default:
        // Not relevant.
        break;
    }
}

void
OperaBrowserCore::registerForTvPreferencesChangeNotifications(
    AnyWebBrowserTvPreferencesChangePublisher & change_publisher)
{
    // Register for changes to user preferences and perform an initial
    // update.
    if (change_publisher.registerListener(*this))
    {
        m_tv_preferences_change_publisher = &change_publisher;
    }
    setMediaComponentsPreferences();
}

void
OperaBrowserCore::deregisterForTvPreferencesChangeNotifications()
{
    if (m_tv_preferences_change_publisher)
    {
        m_tv_preferences_change_publisher->deregisterListener(*this);
    }
}

void
OperaBrowserCore::setMediaComponentsPreferences()
{
    NEBULA_LanguageCode language;
    std::vector<std::string> preferred_subtitle_languages;
    if (m_language_setting.getPrimarySubtitleLanguage(&language))
    {
        convertToIso639Dash1(language);
        preferred_subtitle_languages.push_back(language.iso_str);
    }
    if (m_language_setting.getSecondarySubtitleLanguage(&language))
    {
        convertToIso639Dash1(language);
        preferred_subtitle_languages.push_back(language.iso_str);
    }

    bool const subtitles_enabled = m_subtitle_settings.getSubtitlePreference();
    
    std::vector<std::string> preferred_audio_languages;
    if (m_language_setting.getPrimaryAudioLanguage(&language))
    {
        convertToIso639Dash1(language);
        preferred_audio_languages.push_back(language.iso_str);
    }
    if (m_language_setting.getSecondaryAudioLanguage(&language))
    {
        convertToIso639Dash1(language);
        preferred_audio_languages.push_back(language.iso_str);
    }
    
    bool const audio_description_enabled = m_sound_settings.getAudioDescriptionStatus();

    m_opera_engine.setMediaComponentsPreferences(preferred_subtitle_languages,
                                                 subtitles_enabled,
                                                 preferred_audio_languages,
                                                 audio_description_enabled);
}

void
OperaBrowserCore::setStarturlOpened(bool value)
{
    m_opera_engine.setStarturlOpened(value);
}

bool
OperaBrowserCore::isStarturlOpened() const
{
    return m_opera_engine.isStarturlOpened();
}

void 
OperaBrowserCore::setCookieActiveWindow(bool activate)
{
    m_opera_engine.setCookieActiveWindow(activate);

    if (!activate)
    {    
        deactivateMouseLocked();
    }
}

UString
OperaBrowserCore::getBrowserVersionString() const
{
    UString version_string;

#if defined(USING_SDK423)
    version_string = "Vewd 4.23";
#elif defined(USING_SDK422)
    version_string = "Vewd 4.22";
#elif defined(USING_SDK420)
    version_string = "Vewd 4.20";
#else
    version_string = "Vewd 4.13";
#endif

    return version_string;
}

bool
OperaBrowserCore::setWindowActiveByName(cabot::String const& window_name)
{
    return m_opera_engine.setWindowActiveByName(window_name);
}

void
OperaBrowserCore::getWindowListAsName()
{
    m_opera_engine.getWindowListAsName();
}

void
OperaBrowserCore::setAppVisibilityWithGivenName(cabot::String const& window, bool visible)
{
    m_opera_engine.setAppVisibilityWithGivenName(window, visible);
}

void
OperaBrowserCore::sendPlatformMessage(const char* message)
{
    FrostMutexLock lock(m_browser_mutex);

    m_opera_engine.sendPlatformMessage(message);
}

bool
OperaBrowserCore::isWindowActive(cabot::String const& window_name)
{
    return m_opera_engine.isWindowActive(window_name);
}

#ifdef INCLUDE_HOTELTV
void
OperaBrowserCore::sendPlatformMessageInJsonFormat()
{
    m_opera_engine.sendPlatformMessageInJsonFormat();
}

void
OperaBrowserCore::addSendPlatformMessageParameter(cabot::String first_param, cabot::String second_param)
{
    m_opera_engine.addSendPlatformMessageParameter(first_param, second_param);
}

void
OperaBrowserCore::clearSendPlatformMessageParameter()
{
    m_opera_engine.clearSendPlatformMessageParameter();
}

void
OperaBrowserCore::setHbbAppStartedByUser(bool enable)
{
        m_opera_engine.setHbbAppStartedByUser(enable);
}

bool
OperaBrowserCore::isHbbAppStartedByUser()
{
    return m_opera_engine.isHbbAppStartedByUser();
}
#endif

void 
OperaBrowserCore::getWindowMemoryInformation()
{
   m_opera_engine.getWindowMemoryInformation();
}

bool
OperaBrowserCore::startAdvertisementBrowser(cabot::String const& url)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.startAdvertisementBrowser(url);
}

bool
OperaBrowserCore::stopAdvertisementBrowser()
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.stopAdvertisementBrowser();
}

bool
OperaBrowserCore::startBrowserLauncher(cabot::String const& url)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.startBrowserLauncher(url);
}

bool
OperaBrowserCore::isBrowserLauncherFocused() const
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.isBrowserLauncherFocused();
}

bool
OperaBrowserCore::isBrowserLauncherVisible() const
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.isBrowserLauncherVisible();
}

bool
OperaBrowserCore::setFocusToBrowserLauncher(bool focus)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.setFocusToBrowserLauncher(focus);
}

void
OperaBrowserCore::stopBrowserLauncher()
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    m_opera_engine.stopBrowserLauncher();
}

void
OperaBrowserCore::reloadBrowserLauncher()
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }

    m_opera_engine.reloadBrowserLauncher();
}

bool
OperaBrowserCore::setBrowserLauncherVisible(bool visible)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }

    return m_opera_engine.setBrowserLauncherVisible(visible);
}

bool 
OperaBrowserCore::startMediaPlayer(cabot::String const& url, bool const linear_ip_service)
{
    FrostMutexLock lock(m_browser_mutex);

    if (!m_opera_engine.initialisationCompleted())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return false;
    }
    return m_opera_engine.startMediaPlayer(url, linear_ip_service);
}

void
OperaBrowserCore::notifyBrowserProcessStateChange(BrowserProcessState state)
{
#if defined USING_SDK422 || defined USING_SDK423
    if (state == BrowserProcessState::browser_process_state_shutdown)
    {
        if (m_vewd_aurora_ipc_server)
        {
            m_vewd_aurora_ipc_server->resetCallbacks();
        }
    }
#endif
}

bool
OperaBrowserCore::getMediaPlayerPosition(double &x,
                                         double &y,
                                         double &width,
                                         double &height)
{
    return m_opera_engine.getMediaPlayerPosition(x, y, width, height);
}

bool
OperaBrowserCore::setMediaPlayerPosition(double x,
                                         double y,
                                         double width,
                                         double height)
{
    return m_opera_engine.setMediaPlayerPosition(x, y, width, height);
}

void 
OperaBrowserCore::setRestartAfterTermination(bool const restart_flag)
{
    m_opera_engine.setRestartAfterTermination(restart_flag);
}
