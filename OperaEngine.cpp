/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the main Opera engine functionality
 */

#include "config/vewd_integration/enable_directfb_backend.h"
#include "config/vewd_integration/enable_opengles_backend.h"
#include "config/vewd_integration/enable_broadcast_resource_management.h"
#include "config/vewd_integration/enable_wx_native_egl.h"
#include "config/vewd_integration/use_media_player_window.h"
#include "config/include_extended_logging.h"

#include "config/using_sdk420.h"
#include "config/using_sdk422.h"
#include "config/using_sdk423.h"
#include "config/using_sdk423_lima2.h"
#include "config/include_browser_restart_support.h"
#include "config/include_opapp_support.h"
#include "config/include_xpca.h"
#include "config/novatek_platform.h"
#include "config/broadcom_platform.h"
#include "config/include_hybrid_ui.h"
#include "config/gles_ui.h"
#include "config/nflx5t.h"
#include "config/disable_security.h"
#include "config/include_hoteltv.h"
#include "config/include_mheg_engine.h"
#include "config/include_g31.h"
#include "OperaEngine.hpp"
#include "OperaWindow.hpp"
#include "OperaWindowManager.hpp"
#include "OperaHbbTVBrowserEventPublisher.hpp"
#include "OperaOipfBrowserEventPublisher.hpp"
#include "HbbTVApplicationStopped.hpp"
#include "HbbTVApplicationStopped.inl"

#ifdef ENABLE_DIRECTFB_BACKEND
#include "directfb.h"
#endif
#include "ice/ice_osd.h"
#include "eclipse/core/MouseTypes.hpp"
#ifdef INCLUDE_HOTELTV
#include "eclipse/easi/EASIFilePath.hpp"
#include "http_client/Url.hpp"
#endif

#include "frost/frost_standards.h"
#include "frost/frost_time.h"
#include "frost/frost_debug.h"

#include "nebula/adaptation_layer/nebula_ca_controller.h"
#include "nebula/adaptation_layer/nebula_channel_info.h"
#include "nebula/adaptation_layer/nebula_application_capability.h"
#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include "nebula/adaptation_layer/nebula_web_browser.h"
#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "nebula/adaptation_layer/nebula_external_av_source_navigator.h"
#include "nebula/adaptation_layer/nebula_auxfs_reader.h"
#include "nebula/adaptation_layer/nebula_country_setting.h"
#include "nebula/adaptation_layer/nebula_mheg.h"
#include "nebula/adaptation_layer/nebula_sound_settings.h"
#include "nebula/adaptation_layer/nebula_diagnostics.h"
#include "nebula/browser_interface/AnyWebBrowserAitHandler.hpp"
#include "nebula/browser_interface/AnyWebBrowserEventListener.hpp"
#include "nebula/browser_interface/AnyWebBrowserOpAppConfigurationProvider.hpp"
#include "nebula/browser_interface/AnyWebBrowserProgressIndicator.hpp"
#include "nebula/browser_interface/AnyWebBrowserUserAgentProvider.hpp"
#include "nebula/browser_interface/WebBrowserAitApplication.hpp"
#include "nebula/browser_interface/WebBrowserUIControllerDialogTypes.hpp"
#ifdef INCLUDE_OPAPP_SUPPORT
#include "nebula/opapp/OpAppUIElementsManagerProvider.hpp"
#include "nebula/opapp/AnyOpAppUIElementsManager.hpp"
#endif

#ifdef INCLUDE_EXTENDED_LOGGING
#include "nebula/adaptation_layer/nebula_extended_logging.h"
#endif

#include "nebula_src/adaptation_layer/AnyAvObjectMediaPlayer.hpp"
#include "nebula_src/adaptation_layer/AnyAvObjectMediaPlayerResourceHandler.hpp"
#include "nebula_src/adaptation_layer/NebulaMediaPlayerFactory.hpp"
#include "nebula_src/adaptation_layer/NebulaMsePlayer.hpp"

#include <omi/common/ApplicationUsageType.h>
#include <omi/events/AITParsingStatus.h>
#include <omi/events/CertificateError.h>
#include <omi/events/HbbTVApplicationVisibilityChanged.h>
#include <omi/events/HbbTVApplicationStarted.h>
#include <omi/events/HbbTVApplicationNotStarted.h>
#include <omi/events/HbbTVApplicationStopped.h>
#include <omi/events/HbbTVAITAppLoadRequest.h>
#include <omi/events/HbbTVAppURLLoadRequest.h>
#include <omi/events/HbbTVBroadcastRelatedStatusChanged.h>
#include <omi/events/HbbTVKeysetChanged.h>
#include <omi/events/NewWindowBlocked.h>
#include <omi/events/LoadingStarted.h>
#include <omi/events/LoadingFinished.h>

#include <omi/events/OIPFApplicationStarted.h>
#include <omi/events/OIPFApplicationStopped.h>
#include <omi/events/OIPFApplicationStartFailed.h>
#include <omi/events/OIPFApplicationActivated.h>
#include <omi/events/OIPFApplicationDeactivated.h>


#ifdef INCLUDE_OPAPP_SUPPORT
#include <omi/events/OpAppApplicationStartResult.h>
#include <omi/events/OpAppApplicationStopped.h>
#include <omi/events/OpAppStateChanged.h>
#include <omi/events/OpAppKeySetChanged.h>
#ifdef USING_SDK420
#include <omi/events/OpAppApplicationsDiscovered.h>
#include <omi/events/OpAppApplicationsNotDiscovered.h>
#include <omi/events/OpAppApplicationInstallRequest.h>
#endif
#ifdef USING_SDK423
#include <omi/events/OpAppDiscoveryServiceBroadbandDiscoveryStarted.h>
#include <omi/events/OpAppDiscoveryServiceBroadbandDiscoveryPartialResult.h>
#include <omi/events/OpAppDiscoveryServiceInstallRequest.h>
#include <omi/events/OpAppDiscoveryServiceAvailableApplications.h>
#endif
#include <omi/events/OpAppReplaceUiElements.h>
#endif
#include <omi/events/UnhandledKeyInput.h>
#include <omi/events/VirtualKeyboardVisibilityChanged.h>
#include <omi/events/RequestQuotaPermission.h>
#include <omi/events/FullscreenModeRequest.h>
#include <omi/events/WindowDestroyed.h>
#include <omi/events/HbbTVApplicationLoadTimedOut.h>
#include <omi/events/ParentalControlQueryEvent.h>
#include <omi/events/TVStoreWindowClosed.h>
#include <omi/events/RendererProcessDied.h>
#include <omi/events/BrowserGPUMemoryEvent.h>

#ifdef USE_MEDIA_PLAYER_WINDOW
#include <omi/events/MediaPlayerError.h>
#include <omi/events/MediaPlayerLoadFinished.h>
#include <omi/events/MediaPlayerPlaybackRevoked.h>
#endif

#include <omi/Cookie.h>
#include <omi/messages/DeleteCookie.h>
#include <omi/messages/GetCookies.h>
#include <omi/messages/ChannelChanged.h>
#include <omi/messages/ClearWebFormsData.h>
#include <omi/events/CICAMFileRequested.h>
#include <omi/events/URLChanged.h>
#include "vewd_integration/helper/OperaKeyHelper.hpp"
#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#if !defined USING_SDK422 && !defined USING_SDK423
#include <omi/messages/ClearApplicationCache.h>
#endif
#include <omi/messages/ClearCookies.h>
#include <omi/messages/ClearDiskCache.h>
#include <omi/messages/ClearLocalStorage.h>
#include <omi/messages/CreateBrowserUIWindow.h>
#include <omi/messages/CreateWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/InvalidateAIT.h>
#include <omi/messages/GetBrowserMemoryInformation.h>
#include <omi/messages/GetWindowMemoryInformation.h>
#include <omi/messages/GetPref.h>
#include <omi/messages/MouseInput.h>
#include <omi/messages/ParseAIT.h>
#ifdef USING_SDK423_LIMA2
#include <omi/messages/LoadAndParseDvbIXMLAIT.h>
#endif
#include <omi/messages/LoadURLApplication.h>
#include <omi/events/ScriptRequestCloseWindow.h>
#include <omi/events/MediaObjectChanged.h>
#include <omi/messages/LoadURL.h>
#include <omi/messages/StopLoading.h>
#include <omi/messages/RaiseDSMCCStreamEvent.h>
#include <omi/messages/SwitchMediaObjectTrack.h>
#include <omi/messages/ActivateOIPFApplication.h>
#include <omi/messages/DeactivateOIPFApplication.h>
#include <omi/messages/TerminateApplication.h>
#include <omi/messages/SetMediaComponentsPreferences.h>
#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
#include <omi/messages/RegisterBroadcastResource.h>
#include <omi/events/BroadcastResourceGranted.h>
#include <omi/events/BroadcastResourceReleaseRequested.h>
#include <omi/messages/ForceRequestBroadcastResource.h>
#endif
#include "omi/messages/SendPlatformEvent.h"
#include "OperaBrowserCore.hpp"
#include "OperaEventProcessor.hpp"
#include "OperaHbbTVWindow.hpp"
#include "OperaBrowserWindow.hpp"
#include "OperaCspWindow.hpp"
#include "OperaBrowserUIWindow.hpp"
#include "OperaOipfWindow.hpp"
#ifdef USE_MEDIA_PLAYER_WINDOW
#include "OperaMediaPlayerWindow.hpp"
#else
#include "OperaDashPlayerWindow.hpp"
using OperaMediaPlayerWindow = OperaDashPlayerWindow;
#endif

#ifdef INCLUDE_OPAPP_SUPPORT
#include "vewd_integration/opapp/OpAppDiscoveryMonitor.hpp"
#ifdef USING_SDK420
#include "vewd_integration/opapp/OpAppFileProvider.hpp"
#endif
#include "vewd_integration/opapp/OperaOpAppWindow.hpp"
#endif

#include "OperaMediaResourceManager.hpp"

#if defined(USING_SDK420)
#include "vewd_integration/oipf/OperaOipfGlobalDrmClient.hpp"
#endif
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"
#include "utilities_public/join.hpp"
#include "utilities_public/UnderlyingType.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_private/cabot_timer.h"
#include "profile_helper/ProfileHelper.h"

#include "utilities_public/HBBTVURLList.hpp"

#include "vewd_integration/EventNames.hpp"
#include "vewd_integration/MessageNames.hpp"

#include "vewd_integration/graphics/OperaGraphicsBackendFactory.hpp"
#include "vewd_integration/handset/OperaHandsetConfiguration.hpp"
#include "vewd_integration/handset/OperaHandsetIntegration.hpp"

#include <omi/messages/GetMediaInfo.h>
#ifdef INCLUDE_XPCA
#include "uva_custom_interfaces/platform_notification_hook.h"
#endif

#include <inttypes.h>
//TODO WIN32 is never defined. For wx do not do this.
#ifndef WIN32
#ifndef NOVATEK_PLATFORM
extern "C" int platform_Insmod(const char* module_path);
#endif
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <math.h> // for round
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <csignal>
#include <vector>
#include <stdio.h>

TRACE_IMPLEMENT("opera/engine");

#include "vewd_integration/helper/OperaSdk4Utilities.inl"

#if !defined USING_SDK422 && !defined USING_SDK423
/**
 * @note Template specialisations for SDK420 and SDK413 where 
 *              omi::msg:GetBrowserMemoryInformation::Result
 *              omi::msg::GetCookies::Result
 *        do not have an error member.
 */
template <>
bool checkResult<omi::msg::GetBrowserMemoryInformation::Result>(
                        omi::msg::GetBrowserMemoryInformation::Result& )
{
    return true;
}

template <>
bool checkResult<omi::msg::GetCookies::Result>(omi::msg::GetCookies::Result& )
{
    return true;
}
#endif

// #define MEMORY_ACCOUNTING

#define DEFAULT_HOTELTV_URL "http://cloudhoteltv.vestek.com.tr"
#define STARTURL_FILE_PATH "/conf/browser/starturl.txt"

const unsigned int closed_url_time_in_ms  = 4*1000;

namespace
{
cabot::String getCertificateFolder();
cabot::String getUserDataDirectory();
cabot::String getTrustedOrigin();
#if defined(MEMORY_ACCOUNTING) && defined(CABOT_TRACE)
void traceMemoryUsageTimerFunc(void* user_data);
#endif
void getMemoryLimits(cabot::String& system_mem_limit, cabot::String& gpu_mem_limit);
bool getPersistentStorageLimit(unsigned int* storage_limit_kb);
void closedURLTimerFunc(void* user_data);
void cicamStatusCallback(void* user_data, frost_bool available);
void auxfsReaderCallback(void* user_data, frost_uint transaction_id, const unsigned char* data, frost_uint data_length);
#ifdef INCLUDE_XPCA
void xpcaOperatorCallback(const char* key, const char* value);
#endif

ait::ApplicationControlCodes convertOperaControlCode(
                                        omi::evt::AITUpdated::HbbTVApplicationControlCode control_code);

static OperaMediaResourceManager* g_resource_manager = nullptr;
static bool m_gpu_mem_freeing = false;
}


OperaEngine::OperaEngine()
    : m_engine_running(false)
    , m_do_restart(false)
#if defined USING_SDK422 || defined USING_SDK423
    , m_restart()
    , m_restart_testtool(&m_restart)
#endif
    , m_opera(nullptr)
    , m_progress_indicator(nullptr)
    , m_connection_type(connection_type_unknown)
    , m_mouse_active_buttons(0)
    , m_is_exit_from_tv_browser_requested(false)
    , m_is_special_app_starting(false)
    , m_is_hbbtv_app_starting(false)
    , m_is_tv_store_app_restarting(false)
    , m_is_browserui_window(false)
    , m_mouse_cursor_position(default_browser_window_width, default_browser_window_height)
    , m_event_publisher(nullptr)
    , m_parental_rating_checker(nullptr)
    , m_ui_dialog_helper(nullptr)
    , m_last_channel_id(0)
    , m_last_ts_source_id(0)
    , m_window_manager(nullptr)
    , m_hbbtv_enabled(false)
    , m_digital_ib_loaded(false)
    , m_digital_ib_close_requested(false)
    , m_advertisement_loaded(false)
    , m_select_service_upon_app_termination(true)
    , m_app_creation_started(false)
    , m_virtual_keyboard_visible(false)
    , m_additional_data_url("")
    , m_memory_acct_timer(0)
    , m_gpu_mem_free_timer(0)
    , m_closed_url_timer(0)
#ifdef INCLUDE_HOTELTV
    , m_hbb_stopped_spm_timer(0)
#endif
    , m_is_memory_low(false)
    , m_is_oom_occured(false)
    , m_plane_width(default_browser_window_width)
    , m_plane_height(default_browser_window_height)
    , m_last_received_event(0)
    , m_closed_window_url("")
    , m_closed_url_started(false)
    , m_window_resolution_changed(false)
    , m_init_completed(false)
    , m_cicam_status_callback_handle(nullptr)
    , m_auxfs_reader_callback_handle(nullptr)
    , m_drm_client(nullptr)
    , m_app_keyset()
    , m_is_teletext_available(false)
    , m_teletext_org_id(0)
    , m_teletext_app_id(0)
    , m_hbbtv_application_profile(0)
    , m_hbbtv_profile(0)
    , m_hbbtv_event_publisher(nullptr)
    , m_media_player_event_publisher(nullptr)
    , m_ait_handler(nullptr)
    , m_handset_connection(connection_type_unknown)
    , m_handset_configuration(nullptr)
    , m_handset_integration(nullptr)
    , m_opapp_configuration_provider(nullptr)
    , m_is_teletext_app_started(false)
    , m_user_agent_provider(nullptr)
    , m_opapp_event_publisher(nullptr)
    , m_opapp_file_provider(nullptr)
    , m_opera_resource_manager(nullptr)
    , m_oipf_event_publisher(nullptr)
    , m_opapp_suspended(false)
    , m_opapp_active_postponed(false)    
    , m_is_external_source_selected(false)
#ifdef INCLUDE_HOTELTV
    ,m_hbb_app_started_by_user(false)
#endif
{
    auto result = m_initialisation_mutex.initialise();

    if (!result)
    {
        SHOULD_NOT_BE_HERE();
    }
}

OperaEngine::~OperaEngine()
{
    // Check that terminate() has been called prior to destruction.
    CABOT_ASSERT(m_init_completed == false);
}

ExplodingBoolean OperaEngine::initialise(
        AnyWebBrowserUserAgentProvider* user_agent_provider,
        BrowserEventPublisher* event_publisher,
        OperaHbbTVBrowserEventPublisher* hbbtv_event_publisher,
        OperaMediaPlayerEventPublisher* media_player_event_publisher,
        OperaOpAppBrowserEventPublisher* opapp_event_publisher,
        AnyWebBrowserOpAppConfigurationProvider const* opapp_configuration_provider,
        AnyWebBrowserAitHandler* ait_handler,
        std::uint16_t hbbtv_application_profile,
        int hbbtv_profile,
        OperaOipfBrowserEventPublisher* oipf_event_publisher)
{
    CABOT_ASSERT(!m_opera);

    m_opapp_configuration_provider = opapp_configuration_provider;
    m_event_publisher = event_publisher;
    m_hbbtv_event_publisher = hbbtv_event_publisher;
    m_media_player_event_publisher = media_player_event_publisher;
    m_ait_handler = ait_handler;
    m_oipf_event_publisher = oipf_event_publisher;

    cabot::String certificates_dir = "--certificate-dir=" + ::getCertificateFolder();
    cabot::String additional_jsplugins_dir;
#ifdef INCLUDE_HOTELTV
    cabot::String user_data_dir = "--user-data-dir=" + ::getUserDataDirectory();
    cabot::String unsafely_treat_secure = "--unsafely-treat-insecure-origin-as-secure=" + ::getTrustedOrigin();
    cabot::String browser_dir = getenv("BROWSER_DIR");
    additional_jsplugins_dir = "--jsplugins-dir="+ browser_dir + "/jspluginshoteltv";
#else
    if (Profile_IsOpAppVewdDemoEnabled() != 0)
    {
        cabot::String browser_dir = getenv("BROWSER_DIR");
        additional_jsplugins_dir = "--jsplugins-dir="+ browser_dir + "/jspluginshoteltv";
    }
#endif
    cabot::String system_mem_limit;
    cabot::String gpu_mem_limit;
    ::getMemoryLimits(system_mem_limit, gpu_mem_limit);

    int width;
    int height;
    NEBULA_SystemInformationProviderGetPanelInformation(width, height);
    if(width > 1920 || height > 1080 )
    {
        setenv("MSTAR_SDK_MAX_VIDEO_W", "3840", 1);
        setenv("MSTAR_SDK_MAX_VIDEO_H", "2160", 1);
    }
    else
    {
        setenv("MSTAR_SDK_MAX_VIDEO_W", "1920", 1);
        setenv("MSTAR_SDK_MAX_VIDEO_H", "1080", 1);
    }
    cabot::String native_width = cabot::String::fromInt(width);
    cabot::String native_height= cabot::String::fromInt(height);

    setenv("LLD_NATIVE_W", native_width.charArray(),1);
    setenv("LLD_NATIVE_H", native_height.charArray(),1);
    setenv("MEMORY_LIMIT", system_mem_limit.charArray(), 1);
    setenv("NETVIEW_SEGMENT_ERROR_RETRY_COUNT", "10", 1);
    setenv("NETVIEW_SEGMENT_ERROR_RETRY_COUNT_LIVE", "20", 1);


    for (const auto& env : GraphicsBackendEnvironment::getGraphicsEnvironment())
    {
        const int overwrite = 1;
        setenv(env.name.c_str(), env.value.c_str(), overwrite);
    }

    // This option is not supported with DirectFB in SDK 4.11.2 or above.
#ifdef ENABLE_DIRECTFB_BACKEND
    cabot::String gpu_avail_mem;
#else
    cabot::String gpu_avail_mem = "--force-gpu-mem-available-mb=" + gpu_mem_limit;
#endif

    m_hbbtv_application_profile = hbbtv_application_profile;
    m_hbbtv_profile = hbbtv_profile;

    std::vector<char* > argv
    {
        (char *) "aurora.elf",
        (char *) "--log-level=warning",
        (char *) "--enable-virtual-keyboard",
        (char *) "--enable-unrestricted-certificate-error-reporting",
        (char *) "--enable-reduce-memory-usage-when-user-is-idle",
        (char *) gpu_avail_mem.charArray(),
        (char *) certificates_dir.charArray(),
        (char *) "--allow-running-insecure-content", // TODO: this is added so that Channel 4
                                                     // FVP tests can proceed. It will be
                                                     // reverted once the Channel 4 fixes it.
        (char *) "--max-ganesh-texture-cache-mb=32",
        (char *) "--dfb-memory-limit-mb=80",
#if defined(ENABLE_OPENGLES_BACKEND) && (defined(NOVATEK_PLATFORM) || defined(BROADCOM_PLATFORM))
        (char *) "--hw-gles",
#endif
        (char *) "--enable-spatial-navigation",
        (char *) "--max-untiled-layer-width=1024",
        (char *) "--max-untiled-layer-height=1024",
        (char *) "--disable-blink-features=BoxSizeDependentCompositorAnimations",
        (char *) "--disable-accelerated-jpeg-decoding",
        (char *) "--enable-blink-features=MediaSourceNewAbortAndDuration",
#ifdef INCLUDE_HOTELTV
        (char *) unsafely_treat_secure.charArray(),
        (char *) user_data_dir.charArray(),
#endif
        (char *) additional_jsplugins_dir.charArray(),
        (char *) "--disable-internal-ait-finder",
        (char *) "--disable-internal-dsmcc-support",
        //(char *) "--video-logs",
        //(char *) "--v=2",
        //(char *) "--media-backend-logs",
        //(char *) "--extensions-logs",
        (char *) "--disable-internal-ms-data-finder",
#ifndef ENABLE_BROADCAST_RESOURCE_MANAGEMENT
        (char *) "--with-feature:broadcast-resource-user=off",
#endif
        (char *) "--disable-internal-temi-timeline-tracker",
        // Since we do not implement a UVADvbBackend we need to disable all Vewd One features
        // except v1-cs-manager and v1-media-synchroniser.
        (char *) "--disable-features=v1-capabilities,v1-master-configuration,"
                                    "v1-parental-control-manager,v1-search-manager,"
                                    "v1-video-broadcast,v1-mds-fetcher"
    };

#ifdef DISABLE_SECURITY
    // If required, DISABLE_SECURITY flag should be activated via security_disabled compile options.
    // By default value of option is none because of security issues.
    // For Browser debug , below lines should be activated.

    if (NEBULA_DiagnosticsGetBrowserDebugEnabled())
    {
        argv.push_back((char *) "--remote-debugging-port=4725");
    }
#endif

    // This argv does not appear to change the behaviour of the browser with SDK 4.23.
    if (CX_ENABLE_WX_NATIVE_EGL && CX_USING_SDK422)
    {
        argv.push_back((char *) "--use-gl=egl");
    }

#ifdef DISABLE_SECURITY
    cabot::String browser_parameters_file_location = getenv("BROWSER_HOME");
    browser_parameters_file_location += "/browser_parameters.txt";
    std::ifstream ifs (browser_parameters_file_location.charArray());
    if (ifs.is_open())
    {
        std::string parameter;
        while (ifs.good() && std::getline(ifs,parameter))
        {
            argv.push_back(strdup(parameter.c_str()));
        }
    }
    else
    {
        TRACE_INFO(("%s(): File does not exist or cannot be opened\n", __FUNCTION__));
    }
    ifs.close();
#endif

    if (Profile_IsIgnoreCertificateErrorsEnabled() != 0)
    {
        // Some servers that work on https protocol use misconfigured certificates which prevents browser to connect them 
        // For example some tests on Ligada device work on https but Ligada certificate is not well configured
        // This profile option enables us to bypass certificate errors and continue communication with the server
        argv.push_back((char *) "--ignore-certificate-errors");
        argv.push_back((char *) "--ignore-ssl-errors");
    }

    if (Profile_IsCrossOriginOpAppNavEnabled() != 0)
    {
        // Because we use not ready Operator Application and normal app as OpApp
        // (etc. Vewd Demo, Molotov) we need to define below parameter to could open
        // normal app links as OpApp
        argv.push_back((char *) "--allow-cross-origin-opapp-navigation-for-testing");
    }

    if (Profile_IsUnencryptedOpAppPackageEnabled() != 0)
    {
        // Because we use unencrypted OpApp package and not ready OpApp
        // (etc. Vewd Demo OpApp) we need to define below parameter
        argv.push_back((char *) "--allow-unencrypted-opapp-packages-for-testing");
    }

    if (Profile_IsOpAppVewdDemoEnabled() != 0)
    {
        argv.push_back((char *) "--vestelos-disable-keyset-check");
    } 

    omi::StaticConfiguration opera_config;

#if ENABLE_DIRECTFB_BACKEND
    TRACE_INFO(("DirectFB backend is not used anymore....!!!!!! Please use another backend\n"));
#elif ENABLE_OPENGLES_BACKEND
    opera_config.backend = omi::StaticConfiguration::OPENGLES;
    TRACE_INFO(("OPENGLES backend set...\n"));
#else
    opera_config.backend = omi::StaticConfiguration::SOFTWARE;
#endif

    char board_name[32] = {0};
    Profile_GetBoardName(board_name, 32);

    std::string board_name_str("Vestel-");
    board_name_str.append(board_name);

    opera_config.device_model_name = board_name_str;

    argv.push_back((char*)"--num-raster-threads=2");
    argv.push_back((char*)"--enable-gpu-rasterization");

#if defined USING_SDK422 || defined USING_SDK423
    if (CX_INCLUDE_BROWSER_RESTART_SUPPORT)
    {
        opera_config.persevere_omi_client = true;
    }
#endif

    int size_of_argv = argv.size();
    omi::Browser* opera = new omi::Browser(size_of_argv, &argv[0], opera_config);

    if (opera == nullptr)
    {
        return ExplodingBoolean(false);
    }

    m_opera = opera;

    m_user_agent_provider = user_agent_provider;
    m_user_agent_provider->setUserAgentBase(getUserAgentBase());
    updateUserAgent();

    addEventHandler(&OperaEngine::onLoadingStarted, this);
    addEventHandler(&OperaEngine::onLoadingFinished, this);
    addEventHandler(&OperaEngine::OnNewWindowBlocked, this);
    addEventHandler(&OperaEngine::onUrlChanged, this);
    addEventHandler(&OperaEngine::onVirtualKeyboardVisibilityChanged, this);

    addEventHandler(&OperaEngine::onHbbTvApplicationVisibilityChanged, this);
    addEventHandler(&OperaEngine::onHbbTvApplicationStarted, this);
    addEventHandler(&OperaEngine::onHbbTvApplicationNotStarted, this);
    addEventHandler(&OperaEngine::onHbbTvApplicationStopped, this);
    addEventHandler(&OperaEngine::onHbbTVBroadcastRelatedStatusChanged, this);
    addEventHandler(&OperaEngine::onHbbTVAITAppLoadRequest, this);
    addEventHandler(&OperaEngine::onHbbTVAppURLLoadRequest, this);
    addEventHandler(&OperaEngine::onHbbTvKeysetChanged, this);
    addEventHandler(&OperaEngine::onAitParsingStatus, this);
    addEventHandler(&OperaEngine::onAitUpdated, this);
    addEventHandler(&OperaEngine::onWindowDestroyed, this);
    addEventHandler(&OperaEngine::onHbbTvApplicationLoadTimedOut, this);
    addEventHandler(&OperaEngine::onParentalControlQueryEvent, this);
    addEventHandler(&OperaEngine::onScriptRequestCloseWindow, this);
    addEventHandler(&OperaEngine::onTVStoreWindowClosed, this);
    addEventHandler(&OperaEngine::onRequestQuotaPermission, this);
    addEventHandler(&OperaEngine::onFullscreenModeRequest, this);
    addEventHandler(&OperaEngine::onBrowserMemoryEvent, this);
    addEventHandler(&OperaEngine::onBrowserGPUMemoryEvent, this);
    addEventHandler(&OperaEngine::onRendererProcessDied, this);
    addEventHandler(&OperaEngine::onEglContextLostError, this);
#if defined USING_SDK422 || defined USING_SDK423
    addEventHandler(&OperaEngine::onBrowserServiceFatal, this);
#endif
#ifdef GLES_UI
    addEventHandler(&OperaEngine::onUnhandledKeyInput, this);
#else
    char customer[255] = {0};
    NEBULA_SystemInformationProviderGetMainCustomerName(customer);
    if ((Profile_GetPortalMode() != Vestel)
        || (Profile_IsFreeviewPlayEnabled() != 0)
        || (strcmp("MEDION",customer) == 0))
    {
        addEventHandler(&OperaEngine::onUnhandledKeyInput, this);
    }
#endif
#ifdef INCLUDE_HYBRID_UI
    NEBULA_ApplicationSharedCapability shared_capabilities;
    NEBULA_GetSharedCapability(&shared_capabilities);
    if (shared_capabilities.hybrid_ui_availability)
#endif
    {
        addEventHandler(&OperaEngine::onOipfApplicationStarted, this);
        addEventHandler(&OperaEngine::onOipfApplicationStopped, this);
        addEventHandler(&OperaEngine::onOipfApplicationFailed, this);
        addEventHandler(&OperaEngine::onOipfApplicationActivated, this);
        addEventHandler(&OperaEngine::onOipfApplicationDeactivated, this);
    }

    addEventHandler(&OperaEngine::onCicamFileRequested, this);
    addEventHandler(&OperaEngine::onMediaObjectChanged,this);

#ifdef INCLUDE_OPAPP_SUPPORT
    addEventHandler(&OperaEngine::onOpAppApplicationStartResult, this);
    addEventHandler(&OperaEngine::onOpAppApplicationStopped, this);
    addEventHandler(&OperaEngine::onOpAppStateChanged, this);
    addEventHandler(&OperaEngine::onOpAppKeysetChanged, this);
#ifdef USING_SDK420
    addEventHandler(&OperaEngine::onOpAppApplicationsDiscovered, this);
    addEventHandler(&OperaEngine::onOpAppApplicationsNotDiscovered, this);
    addEventHandler(&OperaEngine::onOpAppApplicationInstallRequest, this);
#endif
#ifdef USING_SDK423
    addEventHandler(&OperaEngine::onOpAppDiscoveryServiceBroadbandDiscoveryStarted, this);
    addEventHandler(&OperaEngine::onOpAppDiscoveryServiceBroadbandDiscoveryPartialResult, this);
    addEventHandler(&OperaEngine::onOpAppDiscoveryServiceInstallRequest, this);
    addEventHandler(&OperaEngine::onOpAppDiscoveryServiceAvailableApplications, this);
#endif
    addEventHandler(&OperaEngine::onOpAppReplaceUiElements, this);
#endif

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    addEventHandler(&OperaEngine::onBroadcastResourceGranted, this);
    addEventHandler(&OperaEngine::onBroadcastResourceReleaseRequested, this);
#endif

#ifdef USE_MEDIA_PLAYER_WINDOW
    addEventHandler(&OperaEngine::onMediaPlayerError, this);
    addEventHandler(&OperaEngine::onMediaPlayerLoadFinished, this);
    addEventHandler(&OperaEngine::onMediaPlayerPlaybackRevoked, this);
#endif
#ifndef NOVATEK_PLATFORM
#ifdef USING_SDK423
    addEventHandler(&OperaEngine::onWebAudioCreated, this);
    addEventHandler(&OperaEngine::onWebAudioDeleted, this);
#endif
#endif

    m_window_manager = new OperaWindowManager(*this);
    if (!m_window_manager)
    {
        return ExplodingBoolean(false);
    }

    const int broadcast_resource_id = 0;
    m_window_manager->initialise(broadcast_resource_id);

    m_opera_resource_manager = new OperaMediaResourceManager(m_window_manager, this);
    if (!m_opera_resource_manager)
    {
        return ExplodingBoolean(false);
    }

    g_resource_manager = m_opera_resource_manager;

    onConnectionStatusChange(false, connection_type_unknown,"");
    setStorageLimits();
    setMseBufferMemoryLimits();

    if (!m_auxfs_requests_mutex.initialise())
    {
        return ExplodingBoolean(false);
    }
    m_cicam_status_callback_handle = NEBULA_CicamStatusRegisterCallback(cicamStatusCallback, this);
    // Report the module status in case it was inserted prior to callback registration.
    NEBULA_CAModuleState module_state;
    if (NEBULA_GetCAModuleState(&module_state))
    {
        bool const available = (module_state == NEBULA_CaModuleInserted);
        onCicamStatusChange(available);
    }
    m_auxfs_reader_callback_handle = NEBULA_AuxfsReaderRegisterCallback(auxfsReaderCallback, this);

#if defined(USING_SDK420)
    m_drm_client = new OperaOipfGlobalDrmClient;
#endif

    m_handset_integration = new OperaHandsetIntegration(*this, m_window_manager->hbbTVWindow().handle());
    m_handset_configuration = new OperaHandsetConfiguration();
    if (m_ui_dialog_helper)
    {
        m_handset_integration->setUIDialogHelper(m_ui_dialog_helper);
    }

#ifdef INCLUDE_XPCA
    UVAPlatformNotificationHookRegister(xpcaOperatorCallback);
#endif

#ifdef INCLUDE_OPAPP_SUPPORT
    m_opapp_event_publisher = opapp_event_publisher;
    if (!m_opapp_event_publisher)
    {
        return ExplodingBoolean(false);
    }

#ifdef USING_SDK420
    m_opapp_file_provider = new OpAppFileProvider(*this);
    if (!m_opapp_file_provider)
    {
        return ExplodingBoolean(false);
    }
#endif
#else
    CABOT_UNUSED_PARAM(opapp_event_publisher);
#endif

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    //Register as a broadcast resource user
    const int used_resource_id = broadcast_resource_id;
    const std::vector<int> resource_candidates { broadcast_resource_id };
    //post(omi::msg::RegisterBroadcastResource(used_resource_id, resource_candidates)).get();
    post(omi::msg::RegisterBroadcastResource(used_resource_id, resource_candidates));
#endif

    TRACE_INFO(("initialised\n"));

    startMemoryAccounting(1000);
    m_init_completed = true;

    return ExplodingBoolean(true);
}

bool
OperaEngine::initialisationCompleted() const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    return initialisationCompletedLocked();
}

bool
OperaEngine::initialisationCompletedLocked() const
{
    return m_init_completed;
}

void
OperaEngine::terminate()
{
    FrostWriteMutexLock lock(m_initialisation_mutex);

    m_init_completed = false;

    for (EventProcessors::const_iterator it = m_event_processors.begin();
            it != m_event_processors.end(); ++it)
    {
        delete it->second;
    }
    m_event_processors.clear();

    for (AuxfsRequests::const_iterator it = m_auxfs_requests.begin();
            it != m_auxfs_requests.end(); ++it)
    {
        delete it->second;
    }
    m_auxfs_requests.clear();

    NEBULA_DeregisterCallback(m_cicam_status_callback_handle);
    NEBULA_DeregisterCallback(m_auxfs_reader_callback_handle);
#if defined USING_SDK420
    delete m_drm_client;
#endif
    if (m_opera_resource_manager)
    {
        delete m_opera_resource_manager;
    }
    g_resource_manager = nullptr;
#if defined INCLUDE_OPAPP_SUPPORT && defined USING_SDK420
    delete m_opapp_file_provider;
#endif
    delete m_window_manager;
    delete m_opera;

    if(m_handset_integration)
    {
        delete m_handset_integration;
    }
    if(m_handset_configuration)
    {
        delete m_handset_configuration;
    }
}

omi::Browser*
OperaEngine::opera()
{
    CABOT_ASSERT(m_opera);

    return m_opera;
}

bool OperaEngine::start()
{
    TRACE_INFO(("Starting engine\n"));

    return run("opera_engine", FROST_high_priority, 64 * 1024);
}

void OperaEngine::stop()
{
    TRACE_INFO(("Stopping engine\n"));

    if (CabotTimerExists(m_memory_acct_timer))
    {
        CabotTimerStop(m_memory_acct_timer);
        CabotTimerDestroy(m_memory_acct_timer);
    }
    if (CabotTimerExists(m_gpu_mem_free_timer))
    {
        CabotTimerStop(m_gpu_mem_free_timer);
        CabotTimerDestroy(m_gpu_mem_free_timer);
    }

    if (CabotTimerExists(m_closed_url_timer))
    {
        CabotTimerStop(m_closed_url_timer);
        CabotTimerDestroy(m_closed_url_timer);
    }

    terminateCurrentApp();
    m_engine_running = false;
    waitForThreadTermination();
}

bool OperaEngine::openUrl(  cabot::String const& url,
                            bool parse_ait_xml,
                            bool broadcast_related,
                            bool hbbtv_application,
                            bool hbbtv_service,
                            bool allow_auto_transit)
{
    if (handleXmlAitUrl(url, parse_ait_xml, broadcast_related, allow_auto_transit))
    {
        return true;
    }

    if (hbbtv_application && createHbbTVApp(url, hbbtv_service))
    {
        return true;
    }

    // If this is a HbbTV oriented launcher app, load it as OIPF app but remove the configuration query string
    if (url.find("vstl_config=oipf") != cabot::String::npos)
    {
        cabot::String reverted_url = url;

        reverted_url.replaceAll("vstl_config=oipf&","");
        reverted_url.replaceAll("&vstl_config=oipf","");
        reverted_url.replaceAll("?vstl_config=oipf/","");
        reverted_url.replaceAll("?vstl_config=oipf","");
        
        return m_window_manager->startOipfApp(reverted_url);
    }

    m_window_manager->hbbTVWindow().setLinearIPService(false);
    m_window_manager->mediaPlayerWindow().setLinearIPService(false);

    if (handleTVStoreUrl(url))
    {
        return true;
    }

    CABOT_ASSERT(m_window_manager);

    m_app_creation_started = true;
    if(m_window_manager->startCspApp(url))
    {
        m_window_manager->cspWindow().setLinearIPService(hbbtv_service);
        return true;
    }
    return false;
}

namespace
{

bool
getChannelInfo(NEBULA_Channel_ID channel_id, NEBULA_Channel& channel_info)
{
    if (!channel_id)
    {
        channel_id = NEBULA_GetCurrentChannel();
    }

    memset(&channel_info, 0, sizeof(channel_info));

    return NEBULA_GetChannelInfoByID(channel_id, &channel_info) == frost_true;
}

}

bool
OperaEngine::openDvbIXmlAit(cabot::String const& xml_ait_url)
{
#ifdef USING_SDK423_LIMA2
    NEBULA_Channel channel_info;
    if (getChannelInfo(m_last_channel_id, channel_info))
    {
        post(omi::msg::LoadAndParseDvbIXMLAIT(m_window_manager->hbbTVWindow().handle(),
                                              channel_info.onid,
                                              channel_info.tsid,
                                              channel_info.sid,
                                              channel_info.ccid,
                                              xml_ait_url.charArray()));

        m_window_manager->setActive(m_window_manager->hbbTVWindow(), true);
        return true;
    }
#endif
    return false;
}

bool
OperaEngine::isTeletextAvailable()
{
    return m_is_teletext_available;
}

bool
OperaEngine::isTeletextApplicationStarted() const
{
    return m_is_teletext_app_started;
}

bool
OperaEngine::startTeletext()
{
    if (!m_is_teletext_available)
    {
        return false;
    }

    m_is_teletext_app_started = true;
    m_is_hbbtv_app_starting = true;

    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("startTeletext org_id = %u app_id = %u\n",
                    m_teletext_org_id, m_teletext_app_id));
    bool result =  m_window_manager->startHbbTVApp(m_teletext_org_id, m_teletext_app_id);
    if(result)
    {
        int wait_count = 0;
        while(m_is_hbbtv_app_starting)
        {
            wait_count +=1;
            FROST_Sleep(10);
            if(wait_count > 500)
            {
                break;
            }
            if (wait_count % 100 == 0)
            {
                TRACE_INFO(("Waited %d seconds for teletext hbbtv application to start\n", 
                    wait_count/100));
            }
        }
        
        TRACE_VERBOSE(("startTeletext(): Waited %d seconds for teletext application to start, wait_count: %d\n",
            wait_count/100, wait_count));
        if(!m_is_teletext_app_started)
        {   //wait for teletext hbbtv application to start for up to 5 seconds, if teletext app is not runing at the end then return false
            result = false;
        }
    }
    return result;
}

bool
OperaEngine::stopTeletext(bool launch_autostart_app)
{
    if (!isAppRunning() || getCurrentAppType() != WebBrowserAppType::hbbtv_txt_app)
    {
        return false;
    }

    CABOT_ASSERT(m_window_manager);

    m_is_teletext_app_started = false;

    TRACE_VERBOSE(("stopTeletext\n"));
    m_window_manager->stopHbbTvApp(launch_autostart_app);

    return true;
}

bool
OperaEngine::createHbbTVApp(cabot::String const& url, bool hbbtv_service)
{
    CABOT_ASSERT(m_window_manager);
    bool result = m_window_manager->startHbbTVApp(url);
    if(result)
    {
        m_is_hbbtv_app_starting = true;
        m_window_manager->hbbTVWindow().setLinearIPService(hbbtv_service);
    }
    TRACE_VERBOSE(("createHbbTVApp  = %s (hbbtv_service %d) startHbbTVApp returned %d\n", 
                    url.charArray(), hbbtv_service, result));
    return result;
}

bool
OperaEngine::getOpAppConfiguration(
            nebula::OperatorApplicationConfiguration & configuration) const
{
    bool success = false;
    if (m_opapp_configuration_provider)
    {
        configuration = m_opapp_configuration_provider->configuration();
        success = configuration.valid();
    }
    return success;
}

bool
OperaEngine::setOpAppDiscoveryEnabled(bool enabled)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().setDiscoveryEnabled(enabled);
#else
    CABOT_UNUSED_PARAM(enabled);
#endif
    return success;
}

bool
OperaEngine::isOpAppDiscoveryEnabled() const
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().isDiscoveryEnabled();
#endif
    return success;
}

bool
OperaEngine::startOpAppDiscovery(
    nebula::OperatorApplicationDiscoveryProperties const & configuration)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().startDiscovery(configuration);
#else
    CABOT_UNUSED_PARAM(configuration);
#endif
    return success;
}

bool
OperaEngine::stopOpAppDiscovery()
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().stopDiscovery();
#endif
    return success;
}

bool
OperaEngine::uninstallOpApp(
                std::uint16_t application_id, std::uint32_t organisation_id)          
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    auto& window = m_window_manager->opAppWindow();
    success = window.uninstall(application_id, organisation_id);
#else
    CABOT_UNUSED_PARAM(application_id);
    CABOT_UNUSED_PARAM(organisation_id);
#endif
    return success;
}

void
OperaEngine::setOperatorApplicationDiscoveryUris(
    std::vector<cabot::String> const & uris)
{
#ifdef INCLUDE_OPAPP_SUPPORT
    m_window_manager->opAppWindow().setDiscoveryUris(uris);
#else
    CABOT_UNUSED_PARAM(uris);
#endif
}

bool
OperaEngine::startOpApp(
    nebula::OperatorApplicationProperties const & application)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().start(application);
#else
    CABOT_UNUSED_PARAM(application);
#endif
    return success;
}

bool
OperaEngine::stopOpApp()
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().stop();
    m_opapp_suspended = false;
    m_opapp_active_postponed = false;
    if (success)
    {        
        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppStopRequested();
        }
    }
#endif
    return success;
}

void
OperaEngine::suspendOpApp()
{
#ifdef INCLUDE_OPAPP_SUPPORT
    TRACE_INFO(("suspendOpApp() \n"));
    if (!m_opapp_suspended)
    {
        m_opapp_suspended = true;
        OperaOpAppWindow& opapp_window = m_window_manager->opAppWindow();
        m_window_manager->setVisible(opapp_window, false);
        m_window_manager->setActive(opapp_window, false);
        m_window_manager->setFocusable(opapp_window, false);

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
        const auto handle = opapp_window.handle();
        if (m_opera_resource_manager && m_opera_resource_manager->isMediaPlayerInUse(handle))
        {
            TRACE_INFO(("suspendOpApp() - MP in use \n"));
            int wait_count = 0;
            post(omi::msg::ForceRequestBroadcastResource());
            while (m_opera_resource_manager->isMediaPlayerInUse(handle) && (wait_count < 100))
            {
                FROST_Sleep(50);
                wait_count += 1;
            }
            TRACE_INFO(("suspendOpApp() MP Stopped wait_count = %d\n", wait_count));
        }
#endif
        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppSuspended();
        }
    }
#endif
}

void
OperaEngine::resumeOpApp()
{
#ifdef INCLUDE_OPAPP_SUPPORT
    TRACE_INFO(("resumeOpApp() \n"));
    if (m_opapp_suspended)
    {
        m_opapp_suspended = false;
        OperaOpAppWindow& opapp_window = m_window_manager->opAppWindow();
        m_window_manager->setVisible(opapp_window, true);
        m_window_manager->setActive(opapp_window, true);
        m_window_manager->setFocusable(opapp_window, true);
        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppResumed();
        }
    }
#endif
}

bool
OperaEngine::isOpAppSuspended() const
{
    return m_opapp_suspended;
}

bool
OperaEngine::setOpAppState(nebula::OperatorApplicationState new_state)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().setState(new_state);
#else
    CABOT_UNUSED_PARAM(new_state);
#endif
    return success;
}

bool
OperaEngine::setOpAppContext(const cabot::String& query_url)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    success = m_window_manager->opAppWindow().setContext(query_url);
#else
    CABOT_UNUSED_PARAM(query_url);
#endif
    return success;
}

bool
OperaEngine::handleOpAppKeyInput(nebula::KeyCode key_code,
                                 nebula::KeyPressHandlingReason keypress_handling_reason)
{
    bool success = false;
#ifdef INCLUDE_OPAPP_SUPPORT
    const OperaOpAppWindow& opapp_window = m_window_manager->opAppWindow();

    if (opapp_window.shouldHandleKey(key_code, keypress_handling_reason))
    {
        m_window_manager->sendKeyInput(opapp_window, key_code);
        success = true;
    }
#else
    CABOT_UNUSED_PARAM(key_code);
    CABOT_UNUSED_PARAM(keypress_handling_reason);
#endif
    return success;
}

void
OperaEngine::clearOpAppFiles()
{
#ifdef INCLUDE_OPAPP_SUPPORT
    m_window_manager->opAppWindow().clearOpAppFiles();
#endif
}

void OperaEngine::getUIApp(cabot::String& application_url)
{
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->getUiApp(application_url);
}

bool OperaEngine::createUIApp(cabot::String const& application_url,
                              cabot::String const& overlay_application_url,
                              ApplicationResolution application_resolution)
{
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->startUiApp(application_url,
                                        overlay_application_url,
                                        application_resolution);
}

bool OperaEngine::startTvBrowser(cabot::String const& start_url)
{
    CABOT_ASSERT(m_window_manager);
    TRACE_INFO(("Starting Tv Browser with URL(%s)\n", start_url.charArray()));

    m_is_browserui_window = true;
    m_app_creation_started = false;
    m_is_exit_from_tv_browser_requested = false;
    m_window_manager->startBrowserUi(start_url);
    m_mouse_cursor_position.reset();

    return true;
}

bool OperaEngine::stopTvBrowser()
{
    CABOT_ASSERT(m_window_manager);
    TRACE_INFO(("Stopping Tv Browser\n"));
    m_is_browserui_window = false;
    m_window_manager->stopBrowserUi();
    updateUserAgent();
    return true;
}

bool OperaEngine::tvBrowserUrl(cabot::String & url)
{
    CABOT_ASSERT(m_window_manager);

    OperaWindow* browser_ui_window = m_window_manager->getWindowByName(OperaWindow::window_name_browser_ui);
    if (browser_ui_window)
    {
        m_window_manager->getUrl(*browser_ui_window, url);
        return true;
    }

    return false;
}

void OperaEngine::resetTvBrowserPage()
{
    CABOT_ASSERT(m_window_manager);

    OperaWindow* browser_ui_window = m_window_manager->getWindowByName(OperaWindow::window_name_browser_ui);
    if (browser_ui_window)
    {
        OperaWindowHandle handle = browser_ui_window->handle();
        //post(omi::msg::StopLoading(handle)).get();
        //post(omi::msg::LoadURL(handle, "about:blank")).get();
        post(omi::msg::StopLoading(handle));
        post(omi::msg::LoadURL(handle, "about:blank"));        
        TRACE_INFO(("ResetTVBrowserPage\n"));
    }
}

bool OperaEngine::isTvBrowserRunning() const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }
    
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->isCurrentActiveWindowType(OperaWindow::WindowType::window_browser_ui);
}

void
OperaEngine::onTvBrowserExitRequested()
{
    if (isTvBrowserRunning())
    {
        m_is_exit_from_tv_browser_requested = true;
    }
}

bool
OperaEngine::isTvBrowserExitRequested() const
{
    return m_is_exit_from_tv_browser_requested;
}

bool
OperaEngine::isSpecialAppStarting() const
{
    return m_is_special_app_starting;
}

void
OperaEngine::setSpecialAppStarting(bool status)
{
    m_is_special_app_starting = status;
}

bool
OperaEngine::isExternalSourceSelected() const
{
    return m_is_external_source_selected;
}    

void
OperaEngine::setExternalSourceSelected(bool selected)
{
    m_is_external_source_selected = selected;
}

bool OperaEngine::isTvStoreAppRunning() const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }
    
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->isCurrentActiveWindowType(OperaWindow::WindowType::window_tv_store);
}

bool
OperaEngine::isTvStoreAppRestarting()
{
    bool ret_value = false;
    if (m_is_tv_store_app_restarting)
    {
        ret_value = true;
        m_is_tv_store_app_restarting = false;
    }
    return ret_value;
}

unsigned int
OperaEngine::getPlaneWidth()
{
    return m_plane_width;
}

unsigned int
OperaEngine::getPlaneHeight()
{
    return m_plane_height;
}

void
OperaEngine::setPlaneWidthHeight(unsigned int width, unsigned int height)
{
    m_plane_width = width;
    m_plane_height = height;
}

void
OperaEngine::setAdditionalDataUrlForYoutube(cabot::String const& additional_data_url)
{
    m_additional_data_url = additional_data_url;
}

cabot::String
OperaEngine::getYoutubeUrl(cabot::String const& launch_method, cabot::String url)
{
    cabot::String youtube_url;
    if(url.find("?") != cabot::String::npos)
    {
        youtube_url = url + "&" + "launch=";
    }
    else
    {
        youtube_url = url + "?" + "launch=";
    }
    youtube_url += launch_method;
    youtube_url += "&" + m_additional_data_url;
    return youtube_url;
}

bool
OperaEngine::isWindowActive(cabot::String const& window_name)
{
    CABOT_ASSERT(m_window_manager);

    bool ret_value = false;

    OperaWindow* settings_menu_win = m_window_manager->getWindowByName(window_name);

    if (settings_menu_win)
    {
        ret_value = m_window_manager->isActive(*settings_menu_win);
    }

    return ret_value;
}

bool
OperaEngine::setWindowActiveByName(cabot::String const& window_name)
{
    CABOT_ASSERT(m_window_manager);

    bool ret_value = false;

    OperaWindow* menu_win = m_window_manager->getWindowByName(window_name);

    if (menu_win)
    {
        ret_value = m_window_manager->isActive(*menu_win);

        if (!ret_value)
        {
            m_window_manager->setActive(*menu_win, true);

            ret_value = m_window_manager->isActive(*menu_win);

            if (!ret_value)
            {
                TRACE_ERROR(("Window could not be activated!\n"));
            }
        }
    }

    return ret_value;
}

void
OperaEngine::getWindowListAsName()
{
    m_window_manager->getWindowListAsName();
}

void
OperaEngine::setAppVisibilityWithGivenName(cabot::String const& window, bool visible)
{
    CABOT_ASSERT(m_window_manager);
    if (!initialisationCompletedLocked())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return;
    }
    TRACE_INFO(("Setting window:(%s) %svisible \n",window.c_str(), visible ? "": "in"));
    m_window_manager->setAppVisibilityWithGivenName(window, visible);
}

void
OperaEngine::onUnhandledKeyInput(omi::evt::UnhandledKeyInput unhandled_key)
{
    using omi::evt::UnhandledKeyInput;
    TRACE_VERBOSE(("onUnhandledKeyInput virtual_key : %s event : %s \n",
            omi::common::ToString(unhandled_key.virtual_key).c_str(),
            UnhandledKeyInput::ToString(unhandled_key.event_type).c_str()));

    // HbbTv excluded from history mechanism (ETSI TS 102 796 v1.4.1 A.2.6.4)
    const WebBrowserAppType current_app_type = getCurrentAppType();
    const bool hbbtv_app_running = isAppRunning() &&
            (current_app_type == WebBrowserAppType::hbbtv_app ||
             current_app_type == WebBrowserAppType::hbbtv_txt_app);

    if (!hbbtv_app_running)
    {
        if (unhandled_key.virtual_key == omi::common::Key::OMI_KEY_BACK &&
            unhandled_key.event_type == UnhandledKeyInput::KEYDOWN)
        {
#ifdef GLES_UI
            if (getCurrentHistoryIndex() > 0 || isTvBrowserRunning())
            {
                TRACE_VERBOSE(("Back To Previous Page Current History Index : %d\n", getCurrentHistoryIndex()));
                moveInHistory(-1);
            }
            else
            {
                //do not terminate app in hoteltv
                if (Profile_GetB2BProduct() != Profile_IPHotelTV && Profile_GetB2BProduct() != Profile_RFHotelTV)
                {
                    TRACE_VERBOSE(("Terminate Current App \n"));
                    terminateCurrentApp();
                }
            }
#else
            moveInHistory(-1);
#endif
        }
    }
}

void
OperaEngine::onLoadingStarted(omi::evt::LoadingStarted loading_started)
{
    TRACE_INFO(("Loading of %s started\n", loading_started.url.c_str()));
#ifdef GLES_UI
    if (loading_started.url.find("https://portaltv.tv/Auth.aspx?") != std::string::npos)
    {
        TRACE_INFO(("\n!!!!!!Block Portal Page\n"));
        post(omi::msg::StopLoading(loading_started.window_id));
/* TODO CRAZY these two types are not related
        if (loading_started.type() == OperaWindow::WindowType::window_browser_ui)
        {
            onTvBrowserExitRequested();
        }
        else*/
        {
            onAppHostChanged(loading_started.window_id, "browser://exitportal");
        }
    }
#endif
#ifndef DISABLE_SECURITY
    if(loading_started.url.find("file://") != std::string::npos)
    {
        OperaWindow* window = m_window_manager->getWindowByHandle(loading_started.window_id);
        if (window)
        {
            if (window->type() == OperaWindow::WindowType::window_browser_ui)
            {
                TRACE_INFO(("\n!!!!!!Block file:/// Page for Browser UI Window \n"));
                post(omi::msg::StopLoading(loading_started.window_id));
                post(omi::msg::LoadURL(loading_started.window_id, "about:blank"));
            }
        }
    }
#endif
    if (loading_started.url.find("youtube.com/tv") != std::string::npos)
    {
        if (loading_started.url.find("env_isVideoInfoVisible") == std::string::npos)
        {
            if (loading_started.url.find("launch") == std::string::npos
                    && loading_started.url.find("additionalDataUrl") == std::string::npos)
            {
                post(omi::msg::StopLoading(loading_started.window_id));
                cabot::String youtube_url = getYoutubeUrl("menu", loading_started.url.c_str());
                post(omi::msg::LoadURL(loading_started.window_id, youtube_url.charArray()));
            }
        }
    }

    char board_name[32] = {0};
    Profile_GetBoardName(board_name, 32);
    cabot::String board_name_str(board_name);

    if (strcmp(m_closed_window_url.charArray(), loading_started.url.c_str()) == 0)
    {
        m_closed_url_started = true;
        m_closed_window_url = ("");
    }

    if (loading_started.url.find("browser://") != std::string::npos ||
             loading_started.url.find("netflix.swf") != std::string::npos ||
             loading_started.url.find("amazon.swf") != std::string::npos ||
             loading_started.url.find("youtube.swf") != std::string::npos ||
             loading_started.url.find("youtubemusic.swf") != std::string::npos ||
             loading_started.url.find("youtubekids.swf") != std::string::npos ||
             loading_started.url.find("freeviewportal") != std::string::npos ||
             loading_started.url.find("open_smart_home_menu") != std::string::npos ||
             loading_started.url.find("hdplusopapp") != std::string::npos)
    {
        post(omi::msg::StopLoading(loading_started.window_id));
        onAppHostChanged(loading_started.window_id, loading_started.url.c_str());
    }
    else if (loading_started.url.find("google.com/calendar") != std::string::npos &&
             loading_started.url.find("render?tab=mc") == std::string::npos &&
             loading_started.url.find("ServiceLoginAuth") == std::string::npos)
    {
        // redirect to non-mobile google calendar URL
        std::string redirect_url = loading_started.url;
        redirect_url += "/render?tab=mc";

        post(omi::msg::StopLoading(loading_started.window_id));
        post(omi::msg::LoadURL(loading_started.window_id, redirect_url));
    }
    else if (loading_started.url.find("tvstore.opera") != std::string::npos
            || loading_started.url.find("appstore://") != std::string::npos)
    {
        OperaWindow* window = m_window_manager->getWindowByHandle(loading_started.window_id);
        if (window)
        {
            if (window->type() == OperaWindow::WindowType::window_csp)
            {
                post(omi::msg::StopLoading(loading_started.window_id));
                cabot::String url = loading_started.url.c_str();
                handleTVStoreUrl(url);
            }
        }
    }

    if (m_engine_running && m_progress_indicator)
    {
        m_progress_indicator->showProgress(0);
    }
    OperaWindow* window = m_window_manager->getWindowByHandle(loading_started.window_id);
#ifdef NFLX5T
    if (window && window->type() == OperaWindow::WindowType::window_hbbtv)
    {
        m_window_manager->setVisible(*window, true);
    }
#endif
#ifdef INCLUDE_OPAPP_SUPPORT
    if (window && window->type() == OperaWindow::WindowType::window_opapp)
    {
        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppLoadingStarted();
        }
    }
#endif    
}

void
OperaEngine::OnNewWindowBlocked(omi::evt::NewWindowBlocked block_msg)
{
    //if source is in blacklist and target is not in domain of source
    if (block_msg.target_url.find(block_msg.opener_url) == std::string::npos)
    {
        post(omi::msg::StopLoading(block_msg.opener));
    }
}

void
OperaEngine::onLoadingFinished(omi::evt::LoadingFinished loading_finished)
{
    if (m_engine_running && m_progress_indicator)
    {
        m_progress_indicator->hideProgress();
    }

    OperaWindow* window = nullptr;
    bool inform_hbbtv_of_error = false;
    const char* url = loading_finished.url.c_str();
    TRACE_INFO(("omi::evt::LoadingFinished url %s, status %u, \"%s\", error_code %d, \"%s\", http_status_code %d\n",
                url,
                loading_finished.status,
                omi::evt::LoadingFinished::ToString(loading_finished.status).c_str(),
                loading_finished.error_code,
                loading_finished.error_message.c_str(),
                loading_finished.http_status_code));

    switch (loading_finished.status)
    {
    case omi::evt::LoadingFinished::LOAD_SUCCEEDED:
        TRACE_INFO(("Loading of URL '%s' completed successfully\n", url));

        window = m_window_manager->getWindowByHandle(loading_finished.window_id);

        if (window)
        {
            if (m_window_resolution_changed &&
               (loading_finished.url.find("qual-e.appspot.com") != std::string::npos))
            {
                m_window_manager->setVisible(*window, true);
                m_window_resolution_changed = false;
            }

            cabot::String key_remappings = getKeyRemappingsForURL(url);
            if (!key_remappings.empty())
            {
                setPreference("key_remapping", key_remappings.charArray());
            }

            char board_name[32] = {0};
            Profile_GetBoardName(board_name, 32);
            cabot::String board_name_str(board_name);

            if ((loading_finished.url.find("youtube.com/tv") != std::string::npos) ||
                (loading_finished.url.find("qual-e.appspot.com") != std::string::npos))
            {
                unsigned int keyset = keyset_red | keyset_green | keyset_yellow | keyset_blue
                                    | keyset_navigation | keyset_vcr | keyset_alpha | keyset_other;
                std::vector<int> others;
                others.push_back(static_cast<int>(omi::common::Key::OMI_KEY_SUBTITLE));
                others.push_back(static_cast<int>(omi::common::Key::OMI_KEY_CSP_KEY_1));
                window->setKeySet(keyset, others);
            }
            window->setCurrentPageUrl(url);
        }
        //If broadcast related HBB application loads auto start URL then start broadcast
        if (getCurrentAppType() == WebBrowserAppType::hbbtv_app)
        {
            cabot::String loaded_url = url;
            OperaHbbTVWindow& hbbtv_win = m_window_manager->hbbTVWindow();
            //truncate protocol string
            cabot::String::size_type query_position = loaded_url.find("://");
            if (query_position != cabot::String::npos)
            {
                loaded_url = (char*)(url+query_position+3);
            }
            //truncate query string
            query_position = loaded_url.reverseFindChar('?');
            if (query_position != cabot::String::npos)
            {
                loaded_url.truncate(query_position);
            }
            if ((!hbbtv_win.getAutoStartUrl().empty()) &&
                (hbbtv_win.getAutoStartUrl().find(loaded_url) != cabot::String::npos) &&
                (m_last_received_event != 0x9649db0f8cc69ee5)) //HbbTVApplicationStarted
            {
                NEBULA_SelectChannel(NEBULA_BindToCurrent, -1, nebula_channel_change_normal);
            }
        }
#ifndef USE_MEDIA_PLAYER_WINDOW
        // Not needed (or received) for SDK 4.23 when using the dedicated media player window
        // as it has a specific MediaPlayerLoadFinished event.
        if (m_media_player_event_publisher &&
            getCurrentAppType() == WebBrowserAppType::media_player)
        {
            cabot::String unencoded_url = (m_window_manager ? "" : url);
            if (m_window_manager)
            {
                auto& media_player = m_window_manager->mediaPlayerWindow();
                if (!media_player.checkUnEncodedUrl(url, unencoded_url))
                {
                    unencoded_url = url;
                }
            }
            m_media_player_event_publisher->notifyUrlLoadingFinished(unencoded_url);
        }
#endif
        // This part is added as bbc/watson/watsontest do not follow standards and wants to use 0x7FF keyset
        // without setting the keyset accordingly.
        if ((loading_finished.url.find("watsontest.hibox.fi") != std::string::npos) ||
                (loading_finished.url.find("watson.fi") != std::string::npos))
        {
            window = m_window_manager->getWindowByHandle(loading_finished.window_id);
            if (window)
            {
                unsigned int keyset = keyset_red | keyset_green | keyset_yellow | keyset_blue
                        | keyset_navigation | keyset_vcr | keyset_scroll | keyset_info
                        | keyset_numeric | keyset_alpha | keyset_other;
                std::vector<int> others;
                others.push_back(static_cast<int>(omi::common::Key::OMI_KEY_SUBTITLE));
                others.push_back(static_cast<int>(omi::common::Key::OMI_KEY_MENU));
                others.push_back(static_cast<int>(omi::common::Key::OMI_KEY_RECORD));
                window->setKeySet(keyset, others);
            }
        }

        if (m_app_creation_started)
        {
            bool const linear_service = m_window_manager->cspWindow().isLinearIPService();
            onAppCreated(loading_finished.window_id, url,
                         WebBrowserAppType::user_app, linear_service);
            m_app_creation_started = false;
        }
        onAppDocDone(loading_finished.window_id, url, loading_finished.http_status_code);

        if ((loading_finished.url.find("portaltv.tv/partner/infobar/") != std::string::npos)
            && window)
        {
            m_window_manager->setVisible(*window, true);
            m_window_manager->setActive(*window, true);
        }

        if (m_is_oom_occured)
        {
            TRACE_INFO(("m_is_oom_ocurred is %d\n", m_is_oom_occured));
            if (window && window->type() == OperaWindow::WindowType::window_browser_ui)
            {
                onAppContentError(loading_finished.window_id,
                                loading_finished.error_code,
                                loading_finished.url.c_str(),
                                m_is_oom_occured);
                TRACE_INFO(("Opening Error Popup for TV Browser\n"));
                m_is_oom_occured = false;
            }
        }
        break;
    case omi::evt::LoadingFinished::LOAD_FAILED:
        TRACE_INFO(("Loading of URL '%s' failed with error code %d\n",
            url, loading_finished.error_code));

        // Set empty string if loading fails
        window = m_window_manager->getWindowByHandle(loading_finished.window_id);
        if (window)
        {
            window->setCurrentPageUrl("");
            inform_hbbtv_of_error = (window->type() == OperaWindow::WindowType::window_hbbtv);
        }

        onAppContentError(loading_finished.window_id,
                            loading_finished.error_code,
                            loading_finished.url.c_str());
        break;
    case omi::evt::LoadingFinished::LOAD_ABORTED:
        TRACE_INFO(("Loading of URL '%s' was aborted\n", url));
        OperaWindow* window = m_window_manager->getWindowByHandle(loading_finished.window_id);
        if (window)
        {
            if (window->type() == OperaWindow::WindowType::window_tv_store)
            {
                if (m_is_memory_low)
                {
                    m_is_tv_store_app_restarting = true;
                }
                onAppContentError(loading_finished.window_id,
                                    loading_finished.error_code,
                                    loading_finished.url.c_str(),
                                    m_is_memory_low);
                m_is_memory_low = false;
            }
            inform_hbbtv_of_error = (window->type() == OperaWindow::WindowType::window_hbbtv);
        }

        // Set empty string if loading aborted
        window = m_window_manager->getWindowByHandle(loading_finished.window_id);
        if (window)
        {
            window->setCurrentPageUrl("");
        }
        break;
    }
    if (inform_hbbtv_of_error && m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyHbbTVApplicationNotStartedOnURL(
            loading_finished.url.c_str(),
            (loading_finished.status == omi::evt::LoadingFinished::LOAD_ABORTED));
    }
#ifdef INCLUDE_OPAPP_SUPPORT    
    window = m_window_manager->getWindowByHandle(loading_finished.window_id);
    if (window && window->type() == OperaWindow::WindowType::window_opapp)
    {
        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppLoadingFinished();
        }
    }
#endif
}

cabot::String OperaEngine::getKeyRemappingsForURL(cabot::String const& url)
{
    static const char youtube_url[] = "youtube.com/tv";
    static const char youtube_test_suite_url[] = "qual-e.appspot.com";

    // Key codes for remote keys as specified in YouTube 2016 requirements,
    // section 9.0.
    // Format: Key name : Key code in JSs
    static const char youtube_remappings[] =
          R"***({ "SUBTITLE" : 175, "CSP_KEY_1" : 172 })***";

    if (url.find(youtube_url) != cabot::String::npos ||
        url.find(youtube_test_suite_url) != cabot::String::npos)
    {
        return youtube_remappings;
    }

    return "";
}

void
OperaEngine::onVirtualKeyboardVisibilityChanged(
    omi::evt::VirtualKeyboardVisibilityChanged virtual_keyboard_visibility_changed)
{
    if (m_virtual_keyboard_visible != virtual_keyboard_visibility_changed.is_visible)
    {
        m_virtual_keyboard_visible = virtual_keyboard_visibility_changed.is_visible;
        if (m_virtual_keyboard_visible)
        {
            NEBULA_WebBrowserVirtualKeyboardOpened();
            TRACE_INFO(("WebBrowserVirtualKeyboardOpened\n"));
        }
        else
        {
            NEBULA_WebBrowserVirtualKeyboardClosed();
            TRACE_INFO(("WebBrowserVirtualKeyboardClosed\n"));
        }
    }
}

void OperaEngine::onHbbTvApplicationLoadTimedOut(omi::evt::HbbTVApplicationLoadTimedOut hbbtv_app_load_timed_out)
{
    // Timeout values are controlled by 'hbbtv_load_timeout.dvb' / 'hbbtv_load_timeout.http'
    // preferences
    TRACE_WARN(("HbbTV app load timed out URL: %s\n",  hbbtv_app_load_timed_out.app_url.c_str()));
    TRACE_WARN(("\torg_id: %u\n", hbbtv_app_load_timed_out.org_id));
    TRACE_WARN(("\tapp_id: %u\n", hbbtv_app_load_timed_out.app_id));
    TRACE_WARN(("\tbroadcast related?: %d\n", hbbtv_app_load_timed_out.broadcast_related));

    // Default behaviour
    hbbtv_app_load_timed_out.Abort();

    // Or potentially prompt the user to continue...
    //hbbtv_app_load_timed_out.Continue();
}

void OperaEngine::onHbbTvApplicationVisibilityChanged(
    omi::evt::HbbTVApplicationVisibilityChanged hbbtv_app_visibility)
{
    TRACE_INFO(("HbbTV app visibility URL: %s\n", hbbtv_app_visibility.app_url.c_str()));
    TRACE_INFO(("\torg_id: %u\n", hbbtv_app_visibility.org_id));
    TRACE_INFO(("\tapp_id: %u\n", hbbtv_app_visibility.app_id));
    TRACE_INFO(("\tvisible?: %d\n", static_cast<int>(hbbtv_app_visibility.visible)));

    TRACE_ONLY_PARAM(hbbtv_app_visibility);

#ifdef INCLUDE_HOTELTV
    addSendPlatformMessageParameter("Type", "Event");
    addSendPlatformMessageParameter("Message", "HbbTVApplicationVisibilityChanged");
    sendPlatformMessageInJsonFormat();
#endif
}

void OperaEngine::onHbbTvApplicationStarted(omi::evt::HbbTVApplicationStarted hbbtv_app_started)
{
    CABOT_ASSERT(m_window_manager);

    m_last_hbbtv_app_started.reset(new omi::evt::HbbTVApplicationStarted(hbbtv_app_started));

    const char* url = hbbtv_app_started.app_url.c_str();
    TRACE_INFO(("HbbTV app started URL: %s\n", url));
    TRACE_INFO(("\torg_id: %u\n", hbbtv_app_started.org_id));
    TRACE_INFO(("\tapp_id: %u\n", hbbtv_app_started.app_id));
    TRACE_INFO(("\tbroadcast_related?: %d\n", hbbtv_app_started.broadcast_related));
    TRACE_INFO(("\tauto_started? : %d\n", hbbtv_app_started.autostart_launching));

    OperaHbbTVWindow& hbbtv_win = m_window_manager->hbbTVWindow();

    WebBrowserAppType app_type = WebBrowserAppType::hbbtv_app;
    if (!hbbtv_app_started.broadcast_related)
    {
        OperaWindow* window = m_window_manager->getWindowByHandle(hbbtv_app_started.window_id);
        TRACE_VERBOSE(("onHbbTvApplicationStarted() - Not broadcast related, window type: %d\n", window->type()));
        if (window)
        {
            //Set the keyset to the default value only if it is a not broadcast related app
            //by following the OIPF vol5 DAE specs section 7.2.5
            m_app_keyset.setDefaultKeyset(window);
        }
    }
    m_window_manager->setActive(hbbtv_win, true);

    hbbtv_win.setBroadcastRelated(hbbtv_app_started.broadcast_related);
    if (hbbtv_app_started.broadcast_related && hbbtv_app_started.autostart_launching)
    {
        hbbtv_win.setAutoStartUrl(url);
    }
    else
    {
        hbbtv_win.setAutoStartUrl("");
    }

    bool is_teletext_app = false;

    if (m_is_teletext_available)
    {
        is_teletext_app = (m_teletext_org_id == hbbtv_app_started.org_id &&
                           m_teletext_app_id == hbbtv_app_started.app_id);
    }
    hbbtv_win.setTeletextAppRunning(is_teletext_app);

    onAppCreated(hbbtv_app_started.window_id, url, app_type, hbbtv_win.isLinearIPService());

    if(m_handset_integration && m_handset_connection != connection_type_unknown)
    {
        m_handset_integration->HandleHbbTVApplicationStarted(hbbtv_app_started);
    }

    if (m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyHbbTVApplicationStarted(hbbtv_app_started);
    }
    m_is_hbbtv_app_starting = false;
#ifdef INCLUDE_HOTELTV
    if (CabotTimerExists(m_hbb_stopped_spm_timer))
    {
        //delete timer, to not to send HbbTVApplicationStopped SPM mesagge to HtmlUI
        CabotTimerStop(m_hbb_stopped_spm_timer);
        CabotTimerDestroy(m_hbb_stopped_spm_timer);
    }

    addSendPlatformMessageParameter("Type", "Event");
    addSendPlatformMessageParameter("Message", "HbbTVApplicationStarted");
    sendPlatformMessageInJsonFormat();
#endif
}

void
OperaEngine::onHbbTvApplicationNotStarted(omi::evt::HbbTVApplicationNotStarted hbbtv_app_not_started)
{
    TRACE_INFO(("HbbTV app not started ait_parsing = '%d' reason = '%s'\n", 
        hbbtv_app_not_started.ait_parsing,
        omi::evt::HbbTVApplicationNotStarted::ToString(hbbtv_app_not_started.reason).c_str()));
        
    OperaWindow* window = m_window_manager->getWindowByHandle(hbbtv_app_not_started.window_id);
    if (window)
    {
        WebBrowserAppType app_type = WebBrowserAppType::hbbtv_app;

        TRACE_VERBOSE(("onHbbTvApplicationNotStarted() window_type: %d\n", window->type()));

        if (m_hbbtv_event_publisher)
        {
            m_hbbtv_event_publisher->notifyHbbTVApplicationNotStarted(hbbtv_app_not_started.ait_parsing);
        }
        m_window_manager->hbbTVWindow().setLinearIPService(false);
        if (m_event_publisher)
        {
            // to mirror logic witihn Ronesans that checks ait_parsing && !m_ait_application_urls.empty())
            bool publisher_notified = false;
            if (hbbtv_app_not_started.ait_parsing)
            {
                bool include_url_on_app = false;
                std::vector<WebBrowserAitApplication> applications;
                m_ait_handler->getApplications(applications, include_url_on_app);

                if (!applications.empty())
                {
                    // set to true  here as autostart check added, within DTV_TRUNK development
                    publisher_notified = true;

                    // The window will only receive the url on finished loaded, here it has failed
                    // and therefore does not have the url, so use m_ait_application_urls
                    for (const auto& application : applications)
                    {
                        if (application.getControlCode() == ait::application_control_code_autostart)
                        {
                            TRACE_ERROR(("AIT HbbTV app not started %s\n",
                                          application.toString().charArray()));

                            const std::vector<cabot::String> application_urls
                                = application.getAppUrls();
                            for (const auto& url : application_urls)
                            {
                                const cabot::String cabot_url = url.c_str();
                                TRACE_ERROR(("\tURL: %s\n", cabot_url.charArray()));
                                m_event_publisher->notifyAppContentError(0, cabot_url, app_type, false);
                            }
                        }
                    }
                }
            }
            if (!publisher_notified)
            {
                const cabot::String current_url = window->getCurrentPageUrl();
                TRACE_ERROR(("HbbTV app not started URL: %s\n", current_url.charArray()));
                m_event_publisher->notifyAppContentError(0, current_url, app_type, false);
            }
        }

        m_window_manager->reset(*window);
        m_is_teletext_app_started = false;
        m_is_hbbtv_app_starting = false;
    }
    if (m_handset_integration && m_handset_connection != connection_type_unknown)
    {
        m_handset_integration->HandleHbbTVApplicationNotStarted(hbbtv_app_not_started);
    }
}

void OperaEngine::onHbbTvApplicationStopped(omi::evt::HbbTVApplicationStopped hbbtv_app_stopped)
{
    m_last_hbbtv_app_started.reset(nullptr);

#if defined USING_SDK422 || defined USING_SDK423
    if (CX_INCLUDE_BROWSER_RESTART_SUPPORT && !m_do_restart)
    {
        m_restart.clearHistory();
    }
#endif

    vewd_integ::HbbTVApplicationStopped event(hbbtv_app_stopped);
    hbbTvApplicationStopped(event);
}

void 
OperaEngine::hbbTvApplicationStopped(vewd_integ::HbbTVApplicationStopped& hbbtv_app_stopped)
{ 
    CABOT_ASSERT(m_window_manager);

    const char* url = hbbtv_app_stopped.app_url.c_str();
    TRACE_INFO(("HbbTV app stopped URL: %s\n", url));
    TRACE_INFO(("\torg_id: %u\n", hbbtv_app_stopped.org_id));
    TRACE_INFO(("\tapp_id: %u\n", hbbtv_app_stopped.app_id));
    TRACE_INFO(("\tbroadcast_related?: %d\n", hbbtv_app_stopped.broadcast_related));

    onAppTerminated(hbbtv_app_stopped.window_id,
                    hbbtv_app_stopped.window_id,
                    url,
                    false,
                    WebBrowserAppType::hbbtv_app);

    OperaWindow* window = m_window_manager->getWindowByHandle(hbbtv_app_stopped.window_id);
    if (window)
    {
        const bool clear_keyset = false;
        m_window_manager->reset(*window, clear_keyset);
        if(hbbtv_app_stopped.org_id == m_teletext_org_id && 
            hbbtv_app_stopped.app_id == m_teletext_app_id)
        {
            m_is_teletext_app_started = false;
        }
    }

    if (m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyHbbTVApplicationStopped(hbbtv_app_stopped);
    }

    if(m_handset_integration && m_handset_connection != connection_type_unknown)
    {
        m_handset_integration->HandleHbbTVApplicationStopped(hbbtv_app_stopped);
    }
    m_is_hbbtv_app_starting = false;
    m_window_manager->hbbTVWindow().setLinearIPService(false);
}

void 
OperaEngine::onHbbTVAppURLLoadRequest(omi::evt::HbbTVAppURLLoadRequest app_url_load_request)
{
    TRACE_VERBOSE(("onHbbTVAppURLLoadRequest url  = '%s'\n",
        app_url_load_request.app_url.c_str()));
    if (m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyHbbTVAppURLLoadRequest(app_url_load_request);
    }
}

#ifdef INCLUDE_HOTELTV
void OperaEngine::HbbStoppedSPMTimeout()
{
    addSendPlatformMessageParameter("Type", "Event");
    addSendPlatformMessageParameter("Message", "HbbTVApplicationStopped");
    sendPlatformMessageInJsonFormat();
}
#endif

void OperaEngine::onHbbTVAITAppLoadRequest(omi::evt::HbbTVAITAppLoadRequest hbbtv_app_event)
{
    CABOT_ASSERT(m_window_manager);
    bool is_app_denied = false;
    const char* url = hbbtv_app_event.app_url.c_str();
    TRACE_INFO(("HbbTV AIT app load request: %s\n", url));
    TRACE_INFO(("\torg_id: %u\n", hbbtv_app_event.org_id));
    TRACE_INFO(("\tapp_id: %u\n", hbbtv_app_event.app_id));
    TRACE_INFO(("\tbroadcast_related?: %d\n", hbbtv_app_event.broadcast_related));
#if defined USING_SDK422 || defined USING_SDK423 || defined USING_SDK424    
    TRACE_INFO(("\torigin: %u\n", hbbtv_app_event.origin));
#endif
    
    if(!NEBULA_SystemInformationProviderIsHbbTvEnabled())    
    {
        TRACE_INFO(("Denying app load request as HbbTv is not enabled...\n"));
        hbbtv_app_event.Deny(omi::evt::HbbTVAITAppLoadRequest::NO_USER_CONSENT);
        return;
    }
#if defined USING_SDK422 || defined USING_SDK423 || defined USING_SDK424
    if (hbbtv_app_event.origin==omi::common::HbbTVApplicationOrigin::COMPANION_SCREEN && m_ui_dialog_helper)
    {
        if (m_ui_dialog_helper->confirm(url, DialogType::pre_approval_dialog))
        {
            TRACE_INFO(("CS Application ui confirm result true....\n")); 
            if (m_last_hbbtv_app_started) 
            {
                post(omi::msg::TerminateApplication(m_window_manager->hbbTVWindow().handle()));
#if defined USING_SDK422 || defined USING_SDK423 || defined USING_SDK424
                post(omi::msg::InvalidateAIT());
#else
                post(omi::msg::InvalidateAIT(m_window_manager->hbbTVWindow().handle()));
#endif
                suspendApplicatonCoordinator(frost_true);
                FROST_Sleep(500);
            }
            suspendMheg();
        }
        else
        {
            TRACE_INFO(("CS Application ui confirm result false....\n")); 
            is_app_denied = true;
            hbbtv_app_event.Deny(omi::evt::HbbTVAITAppLoadRequest::NO_USER_CONSENT);
        }
    }
    else
    {
        TRACE_INFO(("CS Application ui dialog helper is NULL!!!\n")); 
    }
#endif

    m_is_hbbtv_app_starting = true;
    if (!is_app_denied && m_parental_rating_checker)
    {
        for (const auto& info : hbbtv_app_event.parental_ratings)
        {
            TRACE_VERBOSE(("Rating: %s\n", info.rating.c_str()));
            TRACE_VERBOSE(("\tScheme: %s\n", info.scheme.c_str()));
            TRACE_VERBOSE(("\tRegion: %s\n", info.region.c_str()));

            m_parental_rating_checker->processRating( hbbtv_app_event.app_id,
                                                      url,
                                                      info.rating,
                                                      info.scheme,
                                                      info.region );
        }

        if (m_parental_rating_checker->isRatingBlocked( hbbtv_app_event.app_id,
                                                        url ))
        {
            if(!m_parental_rating_checker->authorise( hbbtv_app_event.app_id,
                                                      url ))
            {
                hbbtv_app_event.Deny(omi::evt::HbbTVAITAppLoadRequest::PARENTAL_RATING);

                is_app_denied = true;
            }
            else
            {
                hbbtv_app_event.Allow();
            }
        }
    }

    // Trust FVP All-4
    if (hbbtv_app_event.app_url.find("channel4.com") != std::string::npos)
    {
        TRACE_INFO(("********** Trusting FVP All 4\n"));
        hbbtv_app_event.OverrideSecurityPolicyAndAllow(true);
    }

    //if there is a must to open HBB app anyway, use below function
    //hbbtv_app_event.Allow();

    if(is_app_denied)
    {
        m_is_hbbtv_app_starting = false;
        m_window_manager->hbbTVWindow().setLinearIPService(false);
    }
    else if (m_hbbtv_event_publisher)
    {
        /**
         * During test against VEWD420 onHbbTVAITAppLoadRequest is invoked for any 
         * applications extracted from the AIT XML that is referenced within an  
         * Application.createApplication
         */
        m_hbbtv_event_publisher->notifyHbbTVAITAppLoadRequest(hbbtv_app_event);   
    }
}

void OperaEngine::onOipfApplicationStarted(omi::evt::OIPFApplicationStarted app_started)
{
    const char* url = app_started.url.c_str();
    TRACE_INFO(("Oipf app started URL: %s\n", url));
    TRACE_INFO(("\twin_id            : %s\n", app_started.window_id.str().c_str()));
    TRACE_INFO(("\tapp_id            : %u\n", app_started.application_id));
    TRACE_INFO(("\tparent_id         : %d\n", app_started.parent_id));

    using omi::msg::ActivateOIPFApplication;
    //ActivateOIPFApplication::Result result = post(ActivateOIPFApplication(app_started.window_id,
    //        app_started.application_id, true)).get();
    post(ActivateOIPFApplication(app_started.window_id,
            app_started.application_id, true));

    OperaWindow* window = m_window_manager->getWindowByHandle(app_started.window_id);
    if (window && (window->type() == OperaWindow::WindowType::window_launcher))
    {
        if (m_oipf_event_publisher)
        {
            m_oipf_event_publisher->notifyOipfAppStarted(url);
        }
    }

    //TRACE_INFO(("\tActivation result: %s\n", result.errorString().c_str()));
}

void OperaEngine::onOipfApplicationStopped(omi::evt::OIPFApplicationStopped app_stopped)
{
    TRACE_INFO(("Oipf app stopped.\n"));
    TRACE_INFO(("\twin_id            : %s\n", app_stopped.window_id.str().c_str()));
    TRACE_INFO(("\tapp_id            : %u\n", app_stopped.application_id));
    TRACE_INFO(("\tparent_id         : %d\n", app_stopped.parent_id));

    OperaWindow* window = m_window_manager->getWindowByHandle(app_stopped.window_id);

    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        if (window && window->type() == OperaWindow::WindowType::window_overlay)
        {
            TRACE_INFO(("onOipfApplicationStopped() - For overlay application \n"));
        }
        else if (window && window->type() == OperaWindow::WindowType::window_launcher)
        {
            TRACE_INFO(("onOipfApplicationStopped() - For launcher application \n"));
            if (m_oipf_event_publisher)
            {
                m_oipf_event_publisher->notifyOipfAppStopped();
            }
        }
        else
        {
            if (window)
            {
                TRACE_INFO(("onOipfApplicationStopped() - For custom OIPF application\n"));
                m_window_manager->setActive(*window, false);
                m_window_manager->setVisible(*window, false);
                window->terminateCurrentApp();
            }
            else
            {
                TRACE_ERROR(("onOipfApplicationStopped() - Invalid window handle\n"));
            }
            
        }
    }
    else
    {
        if (window)
        {
            TRACE_INFO(("onOipfApplicationStopped() - For custom OIPF application\n"));
            m_window_manager->setActive(*window, false);
            m_window_manager->setVisible(*window, false);
            window->terminateCurrentApp();
        }
        else
        {
            TRACE_ERROR(("onOipfApplicationStopped() - Invalid window handle\n"));
        }
    }
}

void OperaEngine::onOipfApplicationFailed(omi::evt::OIPFApplicationStartFailed app_fail)
{
    const char* url = app_fail.url.c_str();
    TRACE_INFO(("Oipf app start fail message: %s\n",app_fail.error_message.c_str()));
    TRACE_INFO(("\turl               : %s\n", url));
    TRACE_INFO(("\twin_id            : %s\n", app_fail.window_id.str().c_str()));
    TRACE_INFO(("\tapp_id            : %u\n", app_fail.application_id));
    TRACE_INFO(("\tparent_id         : %d\n", app_fail.parent_id));

    OperaWindow* window = m_window_manager->getWindowByHandle(app_fail.window_id);
    if (window && (window->type() == OperaWindow::WindowType::window_launcher))
    {
        stopBrowserLauncher();
        if (m_oipf_event_publisher)
        {
            m_oipf_event_publisher->notifyOipfAppFailed();
        }
    }
}

void OperaEngine::onOipfApplicationActivated(omi::evt::OIPFApplicationActivated app_activated)
{
    TRACE_INFO(("Oipf app activated \n"));
    TRACE_INFO(("\twin_id            : %s\n", app_activated.window_id.str().c_str()));
    TRACE_INFO(("\tapp_id            : %u\n", app_activated.application_id));
    TRACE_INFO(("\tparent_id         : %d\n", app_activated.parent_id));
}

void OperaEngine::onOipfApplicationDeactivated(omi::evt::OIPFApplicationDeactivated app_deactivated)
{
    TRACE_INFO(("Oipf app deactivated \n"));
    TRACE_INFO(("\twin_id            : %s\n", app_deactivated.window_id.str().c_str()));
    TRACE_INFO(("\tapp_id            : %u\n", app_deactivated.application_id));
    TRACE_INFO(("\tparent_id         : %d\n", app_deactivated.parent_id));
}

void OperaEngine::onUrlChanged(omi::evt::URLChanged url_changed)
{
    TRACE_INFO(("URL Changed to %s\n", url_changed.url.c_str()));
    OperaWindow* window = m_window_manager->getWindowByHandle(url_changed.window_id);

    if (url_changed.url.find("youtube.com/tv") != std::string::npos)
    {
        if (url_changed.url.find("env_isVideoInfoVisible") == std::string::npos)
        {
            if (url_changed.url.find("launch") == std::string::npos &&
                url_changed.url.find("additionalDataUrl") == std::string::npos)
            {
                post(omi::msg::StopLoading(url_changed.window_id));
                cabot::String youtube_url = getYoutubeUrl("menu", url_changed.url.c_str());
                post(omi::msg::LoadURL(url_changed.window_id, youtube_url.charArray()));
            }
        }
    }

    if (m_window_resolution_changed &&
       (url_changed.url.find("youtube.com/tv") != std::string::npos))
    {
        if (window)
        {
            m_window_manager->setVisible(*window, true);
        }
        m_window_resolution_changed = false;
    }

    if (url_changed.url.find("html_applications/digital_ib") != std::string::npos) 
    {
        m_digital_ib_loaded = true;
    }
    else 
    {
        if (window
            && (window->type() != OperaWindow::WindowType::window_opapp))
        {
            m_digital_ib_loaded = false;
        }
    }
    if (window && window->type() == OperaWindow::WindowType::window_hbbtv)
    {
        m_hbbtv_event_publisher->notifyHbbTVURLChanged(url_changed.url.c_str());
    }

#ifndef NOVATEK_PLATFORM
    if (url_changed.url.find("youtube.com/tv") != std::string::npos)
    {
        m_window_manager->setCurrentUrl((cabot::String)url_changed.url.c_str());
    }
#endif

    if (window &&
        window->type() != OperaWindow::WindowType::window_hbbtv &&
        window->type() != OperaWindow::WindowType::window_browser_ui &&
        window->type() != OperaWindow::WindowType::window_media_player &&
          (url_changed.url.find(".bbctvapps.co.uk") != std::string::npos ||
           url_changed.url.find(".bbc.co.uk") != std::string::npos))
    {
        TRACE_WARN(("Stopping BBC app and restarting as HbbTV Application (url '%s')\n",
            url_changed.url.c_str()));

        post(omi::msg::StopLoading(url_changed.window_id));
        createHbbTVApp(cabot::String(url_changed.url.c_str()), false);
    }
}

void OperaEngine::onHbbTvKeysetChanged(omi::evt::HbbTVKeysetChanged keyset_changed)
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("Keyset is changed, the new keyset is %x is_broadcast_related=%d\n", keyset_changed.new_keyset, keyset_changed.broadcast_related));
    OperaHbbTVWindow& hbbtv_window = m_window_manager->hbbTVWindow();

    bool is_autostart = false;
    WebBrowserAitApplication ait_application;
    if (m_ait_handler->getApplication(keyset_changed.org_id, keyset_changed.app_id, ait_application))
    {
        is_autostart = (ait_application.getControlCode() == ait::application_control_code_autostart);
    }

    m_app_keyset.setKeysetValue(&hbbtv_window,
                                keyset_changed.new_keyset,
                                is_autostart,
                                keyset_changed.broadcast_related,
                                hbbtv_window.isActive(),
                                hbbtv_window.isLinearIPService());
                                
#ifdef INCLUDE_HOTELTV
    addSendPlatformMessageParameter("Type", "Event");
    addSendPlatformMessageParameter("Message", "HbbTVKeysetChanged");

    int keyset_value = 0;
    std::vector<int> other_keys;

    m_window_manager->getKeySet(hbbtv_window, &keyset_value, &other_keys);

    if ((m_is_hbbtv_app_starting == true) && (keyset_value > 15)) //check navigation buttons
    {
        //HBB window gets the navigation keys, so HBB is running
        setHbbAppStartedByUser(true);
    }
    else
    {
        //HBB window does not get the navigation keys, so HBB is not running
        setHbbAppStartedByUser(false);
    }
    
    sendPlatformMessageInJsonFormat();
#endif
}

void OperaEngine::onAitParsingStatus(omi::evt::AITParsingStatus ait_parsing_status)
{
    if (ait_parsing_status.status == omi::evt::AITParsingStatus::PARSING_COMPLETED)
    {
        TRACE_INFO(("AIT Parsing is complete\n"));
    }
    else if (ait_parsing_status.status == omi::evt::AITParsingStatus::PARSING_INVALIDATED)
    {
        TRACE_INFO(("AIT Parsing is invalidated\n"));
    }
    else
    {
        TRACE_ERROR(("AIT Parsing error\n"));
    }

    if (m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyAitParsingStatus(ait_parsing_status);
    }
}

void OperaEngine::onAitUpdated(omi::evt::AITUpdated ait_updated)
{
    using omi::evt::AITUpdated;

    m_application_info.clear();
    
    std::vector<ChannelHBBTVURLList> channel_hbbtv_url_list;
    channel_hbbtv_url_list.clear();
    ChannelHBBTVURLList item;
    for (const auto& app : ait_updated.applications)
    {
        std::vector<cabot::String> app_urls;
        app_urls.reserve(app.app_urls.size());
        for (const auto url : app.app_urls)
        {
            app_urls.push_back(url.c_str());
        }
        WebBrowserAitApplication ait_application(app.org_id,
                                                 app.app_id,
                                                 app_urls,
                                                 convertOperaControlCode(app.control_code),
                                                 false, // service_bound
                                                 false, // connection_requirement_flag
                                                 false);// broadcast_related

        ait_application.setStatus(AitApplicationStatus::parsed);
        m_ait_handler->insertApplication(ait_application);
        m_application_info.insert(std::make_pair(app.app_id, app));

        if (app.usage_type == omi::common::ApplicationUsageType::USAGE_DIGITAL_TELETEXT_APP)
        {
            m_is_teletext_available = true;
            m_teletext_org_id = app.org_id;
            m_teletext_app_id = app.app_id;
        }
#ifdef CABOT_TRACE
        TRACE_VERBOSE(("App found: %s\n", app.app_name.c_str()));
        TRACE_VERBOSE(("\torg_id: %u\n", app.org_id));
        TRACE_VERBOSE(("\tapp_id: %u\n", app.app_id));
        TRACE_VERBOSE(("\tcontrol_code: %d\n", app.control_code));
        for (const auto& url : app.app_urls)
        {
            TRACE_VERBOSE(("\turl: %s\n", url.c_str()));
        }
#endif
        item.app_name = UString(app.app_name.c_str());
        item.app_org_id = app.org_id;
        item.app_id = app.app_id;
        for(std::vector<cabot::String>::const_iterator url = app_urls.begin();
                                                       url != app_urls.end(); ++url)
        {
            item.url = UString((*url).c_str());
            channel_hbbtv_url_list.push_back(item);
        }
    }

#ifdef INCLUDE_EXTENDED_LOGGING
    NEBULA_ExtendedLoggingSetChannelHBBTVURLList(channel_hbbtv_url_list);
#endif
}

void OperaEngine::onHbbTVBroadcastRelatedStatusChanged(
        omi::evt::HbbTVBroadcastRelatedStatusChanged status_change)
{
    using omi::evt::HbbTVBroadcastRelatedStatusChanged;
    TRACE_INFO(("HbbTV broadcast related status changed URL: %s\n",
                status_change.app_url.c_str()));
    TRACE_INFO(("\torg_id: %u\n", status_change.org_id));
    TRACE_INFO(("\tapp_id: %u\n", status_change.app_id));
    TRACE_INFO(("\tbroadcast_related: %d\n", status_change.broadcast_related));

    OperaWindow* window = m_window_manager->getWindowByHandle(status_change.window_id);
    if (window && window->type() == OperaWindow::WindowType::window_hbbtv)
    {
        OperaHbbTVWindow* hbbtv_win = static_cast<OperaHbbTVWindow*>(window);
        hbbtv_win->setBroadcastRelated(status_change.broadcast_related);
    }

    if (m_hbbtv_event_publisher)
    {
        m_hbbtv_event_publisher->notifyHbbTVBroadcastRelatedChange(status_change);
    }

#ifdef INCLUDE_HOTELTV
    addSendPlatformMessageParameter("Type", "Event");
    addSendPlatformMessageParameter("Message", "HbbTVBroadcastRelatedStatusChanged");
    sendPlatformMessageInJsonFormat();
#endif
}

void OperaEngine::onScriptRequestCloseWindow(omi::evt::ScriptRequestCloseWindow close_window_request)
{
    TRACE_INFO(("ScriptRequestCloseWindow event received, closing current application.\n"));
    OperaWindow* window = m_window_manager->getWindowByHandle(close_window_request.id);
    if (window)
    {
        if (window->type() == OperaWindow::WindowType::window_csp)
        {
            if (m_digital_ib_loaded)
            {
                m_digital_ib_close_requested = true;
            }
            else
            {
                 if ((window->getCurrentPageUrl().find("app.foxxum.rtl-smart.tv") == std::string::npos)
                    && window->getCurrentPageUrl().find("live.prd.ott.s.joyn.de") == std::string::npos)
                 {
                     onAppHostChanged(close_window_request.id, "browser://exitportal");
                 }
            }
        }
        else
        {
            if (window->type() == OperaWindow::WindowType::window_browser_ui)
            {
                TRACE_INFO(("\n\nonTvBrowserExitRequested\n\n"));
                onTvBrowserExitRequested();
            }

            m_window_manager->setActive(*window, false);
            if (window->type() != OperaWindow::WindowType::window_hbbtv)
            {
                m_window_manager->setVisible(*window, false);
            }

            window->terminateCurrentApp();
        }
    }
}

void OperaEngine::onWindowDestroyed(omi::evt::WindowDestroyed window_destroyed)
{
    TRACE_INFO(("WindowDestroyed event received, closing current application.\n"));
    OperaWindow* window = m_window_manager->getWindowByHandle(window_destroyed.window_id);
    if (window)
    {
        if (window->type() == OperaWindow::WindowType::window_csp)
        {
            if (m_digital_ib_loaded)
            {
                m_digital_ib_close_requested = true;
            }
            else if (m_advertisement_loaded)
            {
                TRACE_INFO(("Web advertisement closed, portal state won't be changed.\n"));
            }
            else
            {
                onAppHostChanged(window_destroyed.window_id, "browser://exitportal");
            }
        }
        else
        {
            if (window->type() == OperaWindow::WindowType::window_browser_ui)
            {
                TRACE_INFO(("\n\nonTvBrowserExitRequested\n\n"));
                onTvBrowserExitRequested();
            }

            m_window_manager->setActive(*window, false);
            m_window_manager->setVisible(*window, false);
            window->terminateCurrentApp();
        }
    }
    if (m_advertisement_loaded)
    {
        m_advertisement_loaded = false;
    }
}

void OperaEngine::onTVStoreWindowClosed(omi::evt::TVStoreWindowClosed close_window_request)
{
    TRACE_INFO(("onTVStoreWindowClosed event received, closing TV Store application.\n"));
    OperaWindow* window = m_window_manager->getWindowByHandle(close_window_request.window_id);
    if (window)
    {
        m_window_manager->setActive(*window, false);
        m_window_manager->setVisible(*window, false);
        window->terminateCurrentApp();
    }
}

void OperaEngine::onCicamFileRequested(omi::evt::CICAMFileRequested file_request)
{
    TRACE_INFO(("CICAMFileRequested event received (window_id %s app_id %u, org_id %u, domain '%s', path '%s', origin '%s'\n",
                file_request.window_id.str().c_str(),
                file_request.app_id,
                file_request.org_id,
                file_request.domain.c_str(),
                file_request.path.c_str(),
                file_request.origin.c_str()
                ));

    std::string full_path = file_request.domain + '/' + file_request.path;

    const unsigned int transaction_id = NEBULA_AuxfsReaderOpen(full_path.c_str());
    if (0 == transaction_id)
    {
        TRACE_WARN(("CICAMFileRequested failed to initiate request for %s\n", full_path.c_str()));
        file_request.not_found();
        return;
    }

    omi::evt::CICAMFileRequested*  an_event = new omi::evt::CICAMFileRequested(file_request);
    bool inserted = false;
    {
        FrostMutexLock lock(m_auxfs_requests_mutex);
        inserted = m_auxfs_requests.insert(std::pair<unsigned int, omi::evt::CICAMFileRequested*>(transaction_id, an_event)).second;
    }

    if (inserted)
    {
        TRACE_INFO(("CICAMFileRequested transaction id %u: requested %s\n", transaction_id, full_path.c_str()));
    }
    else
    {
        TRACE_WARN(("CICAMFileRequested transaction_id %u: failed to save request for %s\n", transaction_id, full_path.c_str()));
        file_request.not_found();
        delete an_event;
    }
}

void OperaEngine::onRequestQuotaPermission(omi::evt::RequestQuotaPermission request_quota_permission)
{
    TRACE_INFO(("Request for quota: %ld from %s\n",
                request_quota_permission.requested_quota,
                request_quota_permission.origin_url.c_str()));

    request_quota_permission.Allow();   // TODO set up a filter based on origin_url
}

void OperaEngine::onFullscreenModeRequest(omi::evt::FullscreenModeRequest request_fullscreen_mode)
{
    CABOT_UNUSED_PARAM(request_fullscreen_mode);
    TRACE_INFO(("FullscreenMode is requested\n"));
}

void OperaEngine::onBrowserMemoryEvent(omi::evt::BrowserMemoryEvent browser_memory_event)
{
    using omi::msg::GetBrowserMemoryInformation;

 /*  GetBrowserMemoryInformation::Result result = post(GetBrowserMemoryInformation()).get();
    int in_use_mib = checkResult(result) ? result.allocated / (1000 * 1000) : -1;
    TRACE_ALWAYS(("Current memory allocation accounted by Opera: %d Mibytes\n", in_use_mib));*/

    post(GetBrowserMemoryInformation());
 
    cabot::String system_mem_limit;
    cabot::String gpu_mem_limit;
    ::getMemoryLimits(system_mem_limit, gpu_mem_limit);

    switch(browser_memory_event.state)
    {
        case omi::evt::BrowserMemoryEvent::OK:
            TRACE_ALWAYS(("Memory is OK (System memory limit in bytes(%s), gpu mem limit in mbytes (%s))\n", system_mem_limit.charArray(), gpu_mem_limit.charArray()));
            break;
        case omi::evt::BrowserMemoryEvent::LOW:
            TRACE_ALWAYS(("Memory is LOW (System memory limit in bytes(%s), gpu mem limit in mbytes (%s))\n", system_mem_limit.charArray(), gpu_mem_limit.charArray()));
            m_is_memory_low = true;
            break;
        case omi::evt::BrowserMemoryEvent::OOM:
        {
            TRACE_ALWAYS(("Memory is OOM. Using memory over the limit. (System memory limit in bytes(%s), gpu mem limit in mbytes (%s))\n", system_mem_limit.charArray(), gpu_mem_limit.charArray()));
            getWindowMemoryInformation();
            if (isTvBrowserRunning())
            {
                m_is_oom_occured = true;
                resetTvBrowserPage();
            }
            break;
        }
        default:
            break;
    }
}

#if defined USING_SDK422 || defined USING_SDK423
void OperaEngine::onBrowserServiceFatal(omi::evt::BrowserServiceFatalError fatal_error)
{
    const auto running = m_opera->IsRunning();
    TRACE_WARN(("%s() Browser is %srunning %s \n", __func__, 
                    running ? "" : "NOT ",
                    fatal_error.ToString(fatal_error.reason).c_str()));
    
    if (!CX_INCLUDE_BROWSER_RESTART_SUPPORT)
    {
        return;
    }

    // Depending on return value, indicate to threadMain() that a browser restart is required
    // in that thread after poll timeout period (5 seconds).
    m_do_restart = m_restart.processNewFatalError(fatal_error);

    if (m_last_hbbtv_app_started)
    {
        // Process a stopped event constructed from the last started event.
        vewd_integ::HbbTVApplicationStopped hbbtv_app_stopped(*m_last_hbbtv_app_started);
        hbbTvApplicationStopped(hbbtv_app_stopped);
        m_last_hbbtv_app_started.reset(nullptr);
    }

    m_window_manager->notifyBrowserProcessStateChange(browser_process_state_shutdown);

    if (m_event_publisher)
    {
        // Ensure that the shutdown notification is distributed and actioned before the browser
        // is restarted.
        const bool block = true;
        m_event_publisher->notifyBrowserProcessStateChange(browser_process_state_shutdown, block);
    }
}
#endif

void OperaEngine::onParentalControlQueryEvent(omi::evt::ParentalControlQueryEvent
                                              parental_control_query_event)
{
    if (parental_control_query_event)
    {
        const char* url = parental_control_query_event.url.c_str();

        //Parental rating ID specified within query. Can be empty. If query is sent from
        //DVB-DASH event, this is the ID value of parental rating event.
        const char* parental_rating_id = parental_control_query_event.id.c_str();

        //Parental control query data specific to source of parental rating.
        const char* query_data = parental_control_query_event.query_data.c_str();

        TRACE_INFO((
            "ParentalControlQueryEvent event received url %s, "
            "parental_rating_id %s, "
            "query_data %s\n",
            url,
            parental_rating_id,
            query_data
        ));
        TRACE_ONLY_PARAM(parental_rating_id);
        TRACE_ONLY_PARAM(query_data);

        //If the system parental rating is not enough, allow the pin to overide
        if (m_parental_rating_checker->isRatingBlocked(
                url,
                parental_control_query_event.query_data)
           )
        {
            if (m_parental_rating_checker->authorise(url))
            {
                parental_control_query_event.Allow();
            }
            else
            {
                parental_control_query_event.Deny();
            }
        }
        else
        {
            parental_control_query_event.Allow();
        }
    }
}

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
bool UVAPlatformResourceHandlerRequestResource2(bool request, uint32_t resource_type, int32_t* index)
{
    int resource_idx = *index;
    TRACE_INFO(("UVAPlatformResourceHandlerRequestResource2() %d res_type: %d, index: %d\n", request, resource_type, resource_idx));
    return g_resource_manager && g_resource_manager->handleResourceRequest(request, resource_type, resource_idx);
}
#else
bool UVAPlatformResourceHandlerRequestResource(bool request, uint32_t resource_type)
{
    TRACE_INFO(("UVAPlatformResourceHandlerRequestResource() %d %d \n", request, resource_type));
    return g_resource_manager && g_resource_manager->handleResourceRequest(request, resource_type);
}
#endif

bool VestelSetVolume(double volume)
{
    if (volume < 0.0 || volume > 1.0)
    {
        return false;
    }

    int volume_perc = static_cast<int>(volume * 100);
    TRACE_INFO(("%s(): Setting volume to %d%%. \n", __FUNCTION__, volume_perc));
    bool volume_set = NEBULA_SetVolumeLevel(volume_perc, frost_true) == frost_true;

    return volume_set ? true : false;
}

bool OperaEngine::handleKeyInput(nebula::KeyCode key_code)
{
    CABOT_ASSERT(m_window_manager);

    using nebula::KeyPressHandlingReason;
    return m_window_manager->handleKeyInput(key_code, KeyPressHandlingReason::pre_terminal_ui_handling);
}

bool OperaEngine::handleKeyInputWithDownUpKeys(nebula::KeyCode key_code, bool key_released)
{
    CABOT_ASSERT(m_window_manager);

    using nebula::KeyPressHandlingReason;
    return m_window_manager->handleKeyInputWithDownUpKeys(key_code, KeyPressHandlingReason::pre_terminal_ui_handling, key_released);
}

void
OperaEngine::handleResourceManagerIsBroadcastStoppedByBrowser()
{
#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    if (m_opera_resource_manager)
    {
        if (m_opera_resource_manager->isBroadcastStoppedByBrowser())
        {
            TRACE_ALWAYS(("terminateCurrentApp(): Broadcast resource is stopped by browser, forcing it to be released !!! CANCELED THIS PART !!!\n"));
            // Disabled this part because of issue LUKE-6767.
            // It is causing to terminate opapp media playback by
            // Neustart at fast channel change and start neustart.
#if 0 
            int wait_count = 0;
            post(omi::msg::ForceRequestBroadcastResource());
            while (m_opera_resource_manager->isBroadcastStoppedByBrowser() && (wait_count < 50))
            {
                TRACE_INFO(("terminateCurrentApp(): Waiting for browser to release broadcast resource wait_count=%d\n", wait_count));
                FROST_Sleep(10);
                wait_count += 1;
            }
            TRACE_INFO(("terminateCurrentApp(): Browser has released broadcast resource wait_count = %d\n", wait_count));
#endif
        }
    }
#endif
}

bool 
OperaEngine::terminateCurrentApp(
    WebBrowserAppType const app_type, 
    bool const invalidate_ait)
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("terminateCurrentApp(app_type = '%d'): Terminating active app\n",
                convertToUnderlyingType(app_type)));
    
    bool result = ((app_type != WebBrowserAppType::user_app) &&
                   (app_type != WebBrowserAppType::op_app)) || 
                  m_window_manager->cspWindow().isLinearIPService();
    if (result)
    {
        handleResourceManagerIsBroadcastStoppedByBrowser();

        if (invalidate_ait)
        {
            // Ensure the application is not restarted.
            sendInvalidateAitMessage(m_window_manager->hbbTVWindow().handle());
        }
        // by default reselect service on termination
        setSelectServiceUponAppTermination(true);
        
        return m_window_manager->terminateApp(app_type);
    }
    else
    {
        SHOULD_NOT_BE_HERE();
        TRACE_ERROR(("WebBrowserAppType user_app and op_app not supported\n"));
    }
    return result;
}

void OperaEngine::terminateCurrentApp(bool reselect_service, bool invalidate_ait)
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("terminateCurrentApp(): Terminating active apps\n"));
    
    handleResourceManagerIsBroadcastStoppedByBrowser();

    if (m_is_teletext_app_started)
    {
        stopTeletext(true);
    }

    if (invalidate_ait)
    {
        // Ensure the application is not restarted.
        sendInvalidateAitMessage(m_window_manager->hbbTVWindow().handle());
    }

    setSelectServiceUponAppTermination(reselect_service);

    m_window_manager->terminateUserApps();
}

void OperaEngine::setApmStarted(bool apm_started)
{
    m_window_manager->setApmStarted(apm_started);
}

bool OperaEngine::isAnyAppStartingClosedURL()
{
    bool ret = false;

    if (m_closed_url_started)
    {
        ret = true;
        m_closed_url_started = false;
    }

    return ret;
}

bool OperaEngine::recreateUIApp()
{
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->reloadUiApp();
}

void OperaEngine::destroyUIApp()
{
    CABOT_ASSERT(m_window_manager);

    m_window_manager->stopUiApp();
}

void OperaEngine::setVisibleUIApp(bool visible)
{
    CABOT_ASSERT(m_window_manager);

    m_window_manager->setVisibleUiApp(visible);
}

bool OperaEngine::isUIAppVisible()
{
    CABOT_ASSERT(m_window_manager);

    return m_window_manager->isVisible(m_window_manager->uiWindow());
}

void
OperaEngine::setVisibleAllWindows(bool visible)
{
    if (m_window_manager != nullptr)
    {
        m_window_manager->setVisibleAllWindows(visible);
    }
}

void OperaEngine::moveInHistory(int steps)
{
    CABOT_ASSERT(m_window_manager);

    OperaWindow* active_window = m_window_manager->getActiveWindow();
    if (active_window)
    {
        m_window_manager->moveInHistory(*active_window, steps);
    }
}

int
OperaEngine::getHistoryCount()
{
    CABOT_ASSERT(m_window_manager);
    int count = 0;
    OperaWindow* active_window = m_window_manager->getActiveWindow();
    if (active_window)
    {
        count = m_window_manager->getHistoryCount(*active_window);
    }
    return count;
}

int
OperaEngine::getCurrentHistoryIndex()
{
    CABOT_ASSERT(m_window_manager);
    int index = 0;
    OperaWindow* active_window = m_window_manager->getActiveWindow();
    if (active_window)
    {
        index = m_window_manager->getCurrentHistoryIndex(*active_window);
    }
    return index;
}

void
OperaEngine::updateHttpAcceptLang(char const* accept_lang_iso)
{
    UString accept_lang(accept_lang_iso);
    if(accept_lang.compare("en"))
    {
        accept_lang.append(",en");
    }
    setPreference("intl.accept_languages", accept_lang.c_str());
}

void
OperaEngine::updateDoNotTrack(UString const& do_not_track)
{
    setPreference("do_not_track", do_not_track);
}

void
OperaEngine::updateAppLocale(char const* app_locale_iso)
{
    setPreference("intl.app_locale", app_locale_iso);
}

void
OperaEngine::setFocusToOpApp(bool focus)
{
    CABOT_ASSERT(m_window_manager);

    m_window_manager->setFocusToOpApp(focus);
}

bool 
OperaEngine::isOpAppFocused() const 
{
    if (m_window_manager)
    {
        return m_window_manager->isOpAppFocused();
    }
    return false;
}

void
OperaEngine::onAppCreated(OperaWindowHandle const& handle,
                            cabot::String const& app_url,
                            WebBrowserAppType app_type,
                            bool linear_ip_service)
{
    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() - URL: %s\n", __FUNCTION__, app_url.charArray()));
    }

    if (m_event_publisher)
    {
        m_event_publisher->notifyAppCreated(app_url, app_type,
                m_window_manager->isApplicationBroadcastRelated(handle.internal_id()), linear_ip_service);
    }

    m_window_manager->notifyAppStateChange(handle, ApplicationCreated);
}

void
OperaEngine::onAppTerminated(OperaWindowHandle const& current_handle,
                             OperaWindowHandle const& terminated_handle,
                             cabot::String const& url,
                             bool app_creation_error,
                             WebBrowserAppType app_type)
{
    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() BEGIN - URL: %s\n", __FUNCTION__, url.charArray()));
    }

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    if (m_opera_resource_manager && m_opera_resource_manager->isMediaPlayerInUse(terminated_handle))
    {
        int count = 0;
        while (m_opera_resource_manager->isMediaPlayerInUse(terminated_handle) && (count < 50))
        {
            FROST_Sleep(100);
            ++count;
        }
        if (Profile_IsOperaDebugsEnabled() != 0)
        {
            TRACE_ALWAYS(("onAppTerminated() Media Player resources released, count: %d\n", count));
        }
    }
#endif

    if (m_media_player_event_publisher && app_type == WebBrowserAppType::media_player)
    {
        m_window_manager->mediaPlayerWindow().setLinearIPService(false);
        m_media_player_event_publisher->notifyPlayerTerminated(url);
    }
    else if (m_event_publisher)
    {
        m_event_publisher->notifyAppTerminated(url, app_creation_error,
                app_type,
                m_window_manager->isApplicationBroadcastRelated(current_handle.internal_id()),
                isLinearIPService() ? false : m_select_service_upon_app_termination);
    }
    if (m_window_manager->cspWindow().isLinearIPService())
    {
        m_window_manager->cspWindow().setLinearIPService(false);
    }
    
    OperaWindow* window = m_window_manager->getWindowByHandle(current_handle);
    if (window)
    {
        m_window_manager->getUrl(*window, m_closed_window_url);
    }
    m_closed_url_started = false;

    if (CabotTimerInitialise())
    {
        if (CabotTimerCreate(::closedURLTimerFunc, this, &m_closed_url_timer,
                cabot_timer_auto_delete))
        {
            if (CabotTimerStart(m_closed_url_timer, closed_url_time_in_ms))
            {
            }
        }
    }

    m_window_manager->notifyAppStateChange(current_handle, ApplicationTerminated);

    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() END - URL: %s\n", __FUNCTION__, url.charArray()));
    }
}

void
OperaEngine::onAppDocDone(OperaWindowHandle const& handle, 
                          cabot::String const& url,
                          int const http_status_code)
{
    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() - URL: %s\n", __FUNCTION__, url.charArray()));
    }

    if (m_event_publisher)
    {
        m_event_publisher->notifyAppDocDone(url, http_status_code);
    }

    m_window_manager->notifyAppStateChange(handle, ApplicationDocDone);
}

void
OperaEngine::onAppHostChanged(OperaWindowHandle const& handle, cabot::String const& url)
{
    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() - URL: %s\n", __FUNCTION__, url.charArray()));
    }

    if (m_event_publisher)
    {
        m_event_publisher->notifyAppHostChanged(url);
    }

    m_window_manager->notifyAppStateChange(handle, ApplicationHostChanged);
}

void
OperaEngine::onAppContentError(OperaWindowHandle const& handle,
                                    int error_code,
                                    cabot::String const& url,
                                    bool is_memory_low,
                                    bool is_video_memory_low)
{
    if (Profile_IsOperaDebugsEnabled() != 0)
    {
        TRACE_ALWAYS(("%s() - URL: %s\n", __FUNCTION__, url.charArray()));
    }

    if (m_event_publisher)
    {
        WebBrowserAppType app_type = getCurrentAppType();
        m_event_publisher->notifyAppContentError(error_code, url,
                              app_type, is_memory_low, is_video_memory_low);
    }

    m_window_manager->notifyAppStateChange(handle, ApplicationContentError);
    OperaWindow* win = m_window_manager->getWindowByHandle(handle);
    if (win)
    {
        if (win->type() == OperaWindow::WindowType::window_hbbtv)
        {
            m_is_hbbtv_app_starting = false;
            m_window_manager->hbbTVWindow().setLinearIPService(false);
        }
        else if (win->type() == OperaWindow::WindowType::window_media_player)
        {
            m_window_manager->mediaPlayerWindow().setLinearIPService(false);
        }
        else if (win->type() == OperaWindow::WindowType::window_csp)
        {
            m_window_manager->cspWindow().setLinearIPService(false);
        }
    }
}

void
OperaEngine::onBrowserInitComplete()
{
    m_window_manager->notifyBrowserProcessStateChange(browser_process_state_initialised);
    
    if (m_event_publisher)
    {
        m_event_publisher->notifyBrowserProcessStateChange(browser_process_state_initialised);
    }
}

void
OperaEngine::setProgressIndicator(AnyWebBrowserProgressIndicator* progress_indicator)
{
    m_progress_indicator = progress_indicator;
}

void
OperaEngine::updateUserAgent()
{
    CABOT_ASSERT(m_user_agent_provider);

    m_user_agent_string = m_user_agent_provider->createUserAgentString();

    if (m_is_browserui_window)
    {
        m_user_agent_string = m_user_agent_provider->updateUserAgentForBrowserUI();
        TRACE_INFO(("updateUserAgent() new UA for Browser UI %s \n", m_user_agent_string.c_str()));
    }
    else
    {
        TRACE_INFO(("updateUserAgent() new UA for HbbTv & OpApp: %s \n", m_user_agent_string.c_str()));
        if (m_window_manager != nullptr)
        {
            m_window_manager->updateUserAgentPreference();
        }
    }
}

cabot::String
OperaEngine::getUserAgentBase()
{
    //User agent base never changes on runtime, so call this once during initialization.
    auto result = post(omi::msg::CreateWindow(false)).get();
    OperaWindowHandle temp_win = getWindowHandle(result);

    using omi::msg::GetPref;
    GetPref::Result get_pref_result = post(GetPref(temp_win, "user_agent")).get();
    cabot::String user_agent_base;
    if (checkResult(get_pref_result))
    {
        user_agent_base = get_pref_result.value.c_str();
    }
    user_agent_base.replaceAll("armv7l", "");
    post(omi::msg::DestroyWindow(temp_win));
    return user_agent_base;
}

void
OperaEngine::setParentalRatingChecker(OperaHbbTVParentalRatingChecker* parental_rating_checker)
{
    m_parental_rating_checker = parental_rating_checker;
}

void
OperaEngine::setUIDialogHelper(OperaHbbTVUIDialogHelper* ui_dialog_helper)
{
    m_ui_dialog_helper = ui_dialog_helper;
    if(m_handset_integration)
    {
        m_handset_integration->setUIDialogHelper(ui_dialog_helper);
    }
}

void
OperaEngine::onConnectionStatusChange(bool connected, ConnectionType connection_type, cabot::String interface_name)
{
    CABOT_ASSERT(m_window_manager);
    CABOT_ASSERT(m_opera);
    CABOT_ASSERT(m_user_agent_provider);

    TRACE_INFO(("onConnectionStatusChange() status: %d type: %d\n", connected, connection_type));

    m_connection_type = connection_type;
    m_user_agent_provider->setConnected(connected);
    m_window_manager->hbbTVWindow().onConnectionStatusChange(connected);
    if(m_handset_integration)
    {
        if(connected)
        {
            if(m_handset_connection != connection_type)
            {
                if(m_handset_connection != connection_type_unknown)
                {
                    m_handset_integration->Stop();
                }
                if(connection_type == connection_type_wireless ||
                   connection_type == connection_type_wired)
                {
                    setenv("HANDSET_IF_NAME",interface_name.charArray(),true);
                }
                m_handset_configuration->SetDefault();

                m_handset_integration->Start(*m_handset_configuration);
                TRACE_INFO(("handset component started\n"));

                m_handset_connection = connection_type;
            }
        }
        else
        {
            if(m_handset_connection != connection_type_unknown)
            {
                m_handset_integration->Stop();
                m_handset_connection = connection_type_unknown;
            }
        }
    }
}

bool 
OperaEngine::isAppRunningByType(WebBrowserAppType const app_type) const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }
    
    CABOT_ASSERT(m_window_manager);
    return (m_window_manager && m_window_manager->isAppRunning(app_type));
}

bool
OperaEngine::isAppRunning() const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }
    
    CABOT_ASSERT(m_window_manager);
    return (m_window_manager && m_window_manager->isAppRunning());
}

std::vector<WebBrowserAppType>
OperaEngine::runningAppTypes() const
{
    CABOT_ASSERT(m_window_manager);
    if (m_window_manager)
    {
        return m_window_manager->runningAppTypes();
    }

    return {};
}

bool 
OperaEngine::isMediaPlaying()
{
    CABOT_ASSERT(m_window_manager);

    const auto active_window = m_window_manager->getActiveWindowForComponents();
    if (active_window)
    {
        OperaWindowHandle handle = active_window->handle();
        omi::msg::GetMediaInfo::Result result = post(omi::msg::GetMediaInfo(handle)).get();
        if (checkResult(result))
        {
            for (const auto& media : result.objects)
            {
                TRACE_INFO(("isMediaPlaying() %d\n", media.playing));
                return media.playing;
            }
        }
    }
    TRACE_INFO(("isMediaPlaying() FALSE\n"));
    return false;
}

bool
OperaEngine::isHbbTvAppStarting() const
{    
    return m_is_hbbtv_app_starting;
}

bool
OperaEngine::isDigitalIbCloseRequested()
{
    CABOT_ASSERT(m_window_manager);

    return m_digital_ib_close_requested;
}

void
OperaEngine::resetDigitalIbCloseRequest()
{
    CABOT_ASSERT(m_window_manager);

    m_digital_ib_close_requested = false;
}

#ifdef INCLUDE_HOTELTV
void
OperaEngine::setVisible(bool visible)
{
    if (m_window_manager != nullptr)
    {
        OperaWindow* active_win = m_window_manager->getActiveWindow();
        if (active_win != nullptr )
        {
            m_window_manager->setVisible(*active_win, visible);
        }
    }
}
#endif

WebBrowserAppType
OperaEngine::getCurrentAppType() const
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        TRACE_ERROR(("%s(): Opera is NOT ready yet!!!\n", __FUNCTION__));
        return WebBrowserAppType::hbbtv_app;
    }

    CABOT_ASSERT(m_window_manager);
    return m_window_manager->getCurrentActiveWindowAppType();
}

void
OperaEngine::clearCookies()
{
    TRACE_INFO(("%s\n", __FUNCTION__));
    post(omi::msg::ClearCookies());
    post(omi::msg::ClearLocalStorage());
}

bool OperaEngine::getActiveWindowHandleByName(OperaWindowHandle& window_id, cabot::String const& name) const
{
    if (m_window_manager)
    {
        OperaWindow* active_window = m_window_manager->getActiveWindowByName(name);
        if (active_window)
        {
            window_id = active_window->handle();
            return true;
        }
    }
    return false;
}

bool 
OperaEngine::deleteCookieByIndex(unsigned int index)
{
    OperaWindowHandle window_id {};
    if (getActiveWindowHandleByName(window_id, OperaWindow::window_name_hbbtv))
    {
        auto get_cookies_result = post(omi::msg::GetCookies(window_id)).get();
        if (!checkResult(get_cookies_result))
        {
            return false;
        }
        std::vector<omi::Cookie> cookies = get_cookies_result.cookies;
        if (index < cookies.size())
        {
            omi::Cookie cookie = cookies.at(index);
            omi::msg::DeleteCookie::Result result = post(omi::msg::DeleteCookie(window_id, cookie)).get();
            return (result.error == omi::msg::DeleteCookie::NO_ERR);
        }
        else
        {
            if (getActiveWindowHandleByName(window_id, OperaWindow::window_name_browser_ui))
            {
                auto get_cookies2_result = post(omi::msg::GetCookies(window_id)).get();
                if (!checkResult(get_cookies2_result))
                {
                    return false;
                }
                std::vector<omi::Cookie> cookies2 = get_cookies2_result.cookies;
                unsigned int new_index = index - cookies.size();

                if (new_index < cookies2.size())
                {
                    omi::Cookie cookie = cookies2.at(new_index);
                    omi::msg::DeleteCookie::Result result = post(omi::msg::DeleteCookie(window_id, cookie)).get();
                    return (result.error == omi::msg::DeleteCookie::NO_ERR);
                }
            }
        }
    }
    else if (getActiveWindowHandleByName(window_id, OperaWindow::window_name_browser_ui))
    {
        auto get_cookies_result = post(omi::msg::GetCookies(window_id)).get();
        if (!checkResult(get_cookies_result))
        {
            return false;
        }
        std::vector<omi::Cookie> cookies = get_cookies_result.cookies;

        if (index < cookies.size())
        {
            omi::Cookie cookie = cookies.at(index);
            omi::msg::DeleteCookie::Result result = post(omi::msg::DeleteCookie(window_id, cookie)).get();
            return (result.error == omi::msg::DeleteCookie::NO_ERR);
        }
    }
    return false;
}

std::vector<WebBrowserCookie>
OperaEngine::getCookies()
{
    std::vector<WebBrowserCookie> operaCookies;

    OperaWindowHandle window_id {};
    if (getActiveWindowHandleByName(window_id, OperaWindow::window_name_hbbtv))
    {
        omi::msg::GetCookies::Result result = post(omi::msg::GetCookies(window_id)).get();
        if (checkResult(result))
        {
            for (omi::Cookie& cookie : result.cookies)
            {
                WebBrowserCookie oc;
                oc.creation_utc = UString::fromLong((long)cookie.creation());
                oc.host_name = UString(cookie.domain().c_str());
                oc.name = UString(cookie.name().c_str());
                operaCookies.push_back(oc);
            }
        }
    }

    OperaWindowHandle window_id2 {};
    if (getActiveWindowHandleByName(window_id2, OperaWindow::window_name_browser_ui))
    {
        omi::msg::GetCookies::Result result = post(omi::msg::GetCookies(window_id2)).get(); 
        if (checkResult(result))
        {
            for (omi::Cookie& cookie : result.cookies)
            {
                WebBrowserCookie oc;
                oc.creation_utc = UString::fromLong((long)cookie.creation());
                oc.host_name = UString(cookie.domain().c_str());
                oc.name = UString(cookie.name().c_str());
                operaCookies.push_back(oc);
            }
        }
    }
    TRACE_INFO(("%s() Found %zu cookies\n", __func__, operaCookies.size()));
    return operaCookies;
}

void
OperaEngine::clearPrivateData(bool clear_cookies)
{
    CABOT_ASSERT(m_opera);

    TRACE_INFO(("%s(): %d\n", __FUNCTION__, clear_cookies));

    if (Profile_HybridUIEnabled())
    {
        // All browser data except 'Application Cache' will be cleared during next reboot.
        // 'Application Cache' should not be deleted since hybrid structure depends on it.
    }
    else
    {
        if (clear_cookies)
        {
            TRACE_INFO(("%s() Cookies are cleared\n",__FUNCTION__));
            clearCookies();
        }
#if !defined USING_SDK422 && !defined USING_SDK423
        post(omi::msg::ClearApplicationCache());
#endif
        post(omi::msg::ClearDiskCache());
        post(omi::msg::ClearWebFormsData());
    }
}

void
OperaEngine::clearPrivateBrowserData()
{
    CABOT_ASSERT(m_opera);

    TRACE_INFO(("%s()\n", __FUNCTION__));

    post(omi::msg::ClearCookies());
    post(omi::msg::ClearDiskCache());
    post(omi::msg::ClearLocalStorage());
    post(omi::msg::ClearWebFormsData());
}

void
OperaEngine::disableCookies()
{
    CABOT_ASSERT(m_opera);
    clearCookies();
    UString maxCookieNumber = "-1";
    getGlobalPreference("max_cookies", maxCookieNumber);

    unsigned int max_cookies = 0;
    setGlobalPreference("max_cookies", UString::fromUInt(max_cookies));
    getGlobalPreference("max_cookies", maxCookieNumber);
}

void
OperaEngine::enableCookies()
{
    CABOT_ASSERT(m_opera);
    UString maxCookieNumber = "-1";
    getGlobalPreference("max_cookies", maxCookieNumber);

    unsigned int max_cookies = 3300;
    setGlobalPreference("max_cookies", UString::fromUInt(max_cookies));
    getGlobalPreference("max_cookies", maxCookieNumber);
}

void
OperaEngine::disableDiskCache()
{
    // The default value of max_disk_cache_size should be zero(0) when it was disabled.
    // By changing it to 1, we intended to limit disk access of opera side, but let it use disk cache.
    unsigned int max_disk_cache_size = 1;
    setGlobalPreference("max_disk_cache_size", UString::fromUInt(max_disk_cache_size));
}

void
OperaEngine::onMouseEvent(MouseEventData event)
{
    using omi::msg::MouseInput;
    int changed_buttons = 0;
    MouseInput::Button button = MouseInput::NONE;
    MouseInput::Type event_type = MouseInput::MOVE;

    TRACE_VERBOSE(("Mouse event received x: %d, y: %d, button: %d\n",
                    event.x, event.y, event.buttons));

    if (event.x != 0 || event.y != 0)
    {
#ifdef _LINUX_PC_PLATFORM
        // No scaling with magic numbers required here
        if (event.is_position_absolute)
        {
            m_mouse_cursor_position.set(event.x, event.y);
        }
        else
        {
            m_mouse_cursor_position.move(event.x, event.y);
        }
#else
        if (event.is_position_absolute)
        {
            m_mouse_cursor_position.set(round(event.x*0.9f), round(event.y*0.9f));
        }
        else
        {
            m_mouse_cursor_position.move(round(event.x*0.7f), round(event.y*0.7f));
        }
#endif
        post(MouseInput(m_mouse_cursor_position.x(),
                                m_mouse_cursor_position.y(),
                                MouseInput::MOVE));
    }

    if (event.buttons != m_mouse_active_buttons)
    {
        if (event.buttons > m_mouse_active_buttons)
        {
            changed_buttons = (event.buttons ^ m_mouse_active_buttons);
            event_type = MouseInput::PRESS;
            if (changed_buttons & MouseEventData::mouse_button_left)
            {
                button = MouseInput::LEFT;
            }
            else if (changed_buttons & MouseEventData::mouse_button_right)
            {
                button = MouseInput::RIGHT;
            }
            else if (changed_buttons & MouseEventData::mouse_button_middle)
            {
                button = MouseInput::MIDDLE;
            }
        }
        else
        {
            changed_buttons = (m_mouse_active_buttons ^ event.buttons);
            event_type = MouseInput::RELEASE;
            if (changed_buttons & MouseEventData::mouse_button_left)
            {
                button = MouseInput::LEFT;
            }
            else if (changed_buttons & MouseEventData::mouse_button_right)
            {
               button = MouseInput::RIGHT;
            }
            else if (changed_buttons & MouseEventData::mouse_button_middle)
            {
                button = MouseInput::MIDDLE;
            }
        }

        post(MouseInput(m_mouse_cursor_position.x(),
                                m_mouse_cursor_position.y(),
                                event_type, button));
        TRACE_INFO(("Sending mouse %s\n", event_type == MouseInput::RELEASE ? "UP" : "DOWN"));
        m_mouse_active_buttons = event.buttons;
    }
}

void
OperaEngine::setUsbKeyboardStateAttached(bool attached)
{
    m_window_manager->setUsbKeyboardStateAttached(attached);
}

void
OperaEngine::sendMouseClick()
{
    using omi::msg::MouseInput;
    post(MouseInput(m_mouse_cursor_position.x(),
                            m_mouse_cursor_position.y(),
                            MouseInput::PRESS,
                            MouseInput::LEFT));
    post(MouseInput(m_mouse_cursor_position.x(),
                            m_mouse_cursor_position.y(),
                            MouseInput::RELEASE,
                            MouseInput::LEFT));
}

bool OperaEngine::getCursorPosition(cabot::String& cursor_position) const
{
    cursor_position = "X:";
    cursor_position += cabot::String::fromInt32(m_mouse_cursor_position.x());
    cursor_position += " Y:";
    cursor_position += cabot::String::fromInt32(m_mouse_cursor_position.y());
    return true;
}

void
OperaEngine::clearAit()
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("clearAit() current app: isLinearIPService() = '%d'\n", 
        (m_window_manager && m_window_manager->hbbTVWindow().isLinearIPService())));

    if ((m_window_manager && !m_window_manager->hbbTVWindow().isLinearIPService()))
    {
        sendInvalidateAitMessage(m_window_manager->hbbTVWindow().handle());
    }
}

bool OperaEngine::onAitUpdate(AitSectionPtr ait_section, bool pid_changed, bool empty_pmt)
{
    CABOT_UNUSED_PARAM(pid_changed);
    CABOT_ASSERT(m_opera);
    CABOT_ASSERT(m_window_manager);

    if (!m_hbbtv_enabled)
    {
        return false;
    }

    bool ait_passed_to_browser = false;

    NEBULA_Channel channel_info;
    if (getChannelInfo(m_last_channel_id, channel_info))
    {
        OperaWindowHandle const & window_handle = m_window_manager->hbbTVWindow().handle();
        std::vector<uint8_t> ait_data;

        if (ait_section.get() && ait_section->body())
        {
            if (empty_pmt)
            {
                clearAit();
            }

            ait_data = std::vector<uint8_t>(
                                ait_section->body(),
                                ait_section->body() + ait_section->length());

            TRACE_INFO(("Sending AIT to browser to window(%s), channel: %s\n",
                        window_handle.str().c_str(), channel_info.ccid));
        }
        else
        {
            TRACE_INFO(("Sending null AIT to browser to window(%s), channel: %s\n",
                        window_handle.str().c_str(), channel_info.ccid));
        }

        if(m_is_hbbtv_app_starting)
        {
            m_is_hbbtv_app_starting = false;
        }
        // Always sent ParseAIT, even if the data is null (as per documentation).
        using omi::msg::ParseAIT;
#ifdef USING_SDK423
        // Workaround for browser hang using get() sometimes on channel change.
        // TODO: Revert when fixed by Vewd - STEALTH-1800.
        post(ParseAIT(channel_info.onid,
                      channel_info.tsid,
                      channel_info.sid,
                      channel_info.ccid,
                      ait_data));
        ait_passed_to_browser = true;
#else
#ifdef USING_SDK422
        ParseAIT::Result parse_result = post(
                ParseAIT(channel_info.onid,
                         channel_info.tsid,
                         channel_info.sid,
                         channel_info.ccid,
                         ait_data)).get();
#else
        ParseAIT::Result parse_result = post(
                ParseAIT(window_handle,
                         channel_info.onid,
                         channel_info.tsid,
                         channel_info.sid,
                         channel_info.ccid,
                         ait_data)).get();
#endif
        TRACE_INFO(("%s(%d) parse_result.errorString(): %s\n",
                    __FUNCTION__, __LINE__, parse_result.errorString().c_str()));
        ait_passed_to_browser = (parse_result.error == ParseAIT::NO_ERR);
#endif
    }
    else
    {
        TRACE_ERROR(("%s(%d) Channel Info couldn' t be retrieved, so ait can not be sent to Opera\n", __FUNCTION__, __LINE__));
    }
    return ait_passed_to_browser;
}

void OperaEngine::onServiceSelect(NEBULA_Channel_ID channel_id,
        NEBULA_Tuner_ID ts_source_id,
        NEBULA_ChannelChangeResult channel_change_result)
{
    CABOT_UNUSED_PARAM(channel_change_result);
    TRACE_INFO(("Service selected notification is received\n"));
    if (channel_id != -1 &&
        (m_last_channel_id   != channel_id ||
         m_last_ts_source_id != ts_source_id))
    {
        m_is_teletext_available = false;
        m_last_channel_id   = channel_id;
        m_last_ts_source_id = ts_source_id;
    }
    // Send a ChannelChanged message even if the channel has not changed to
    // allow transitions between broadcast-related and broadcast-independent
    // status.
    sendChannelChangedMessage(channel_id);
}

bool
OperaEngine::isInCache(cabot::String const & url)
{
    //TODO: we should check whether url is in the cache list
    CABOT_UNUSED_PARAM(url);
    return false;
}

void
OperaEngine::enableHbbTv(bool const hbbtv_enabled)
{
    enableHbbTv(hbbtv_enabled, true);
}

void
OperaEngine::enableHbbTv(bool const hbbtv_enabled, bool const terminate_hbbtv_service)
{
    TRACE_INFO(("enableHbbTv() %d\n", hbbtv_enabled));
    // only if there is a change in state
    if (m_hbbtv_enabled != hbbtv_enabled)
    {
        if (m_hbbtv_event_publisher)
        {
            if (hbbtv_enabled)
            {
                m_hbbtv_event_publisher->notifyHbbTVResumed();
            }
            else
            {
                m_hbbtv_event_publisher->notifyHbbTVSuspended();
            }
        }
    }

    m_hbbtv_enabled = hbbtv_enabled;
    m_window_manager->hbbTVWindow().setRestartAfterTermination(hbbtv_enabled);
    if (!hbbtv_enabled)
    {
        if (!m_window_manager->hbbTVWindow().isLinearIPService() || terminate_hbbtv_service)
        {
            m_window_manager->hbbTVWindow().terminateCurrentApp();
#ifndef INCLUDE_HOTELTV
            int wait_count = 0;

            TRACE_VERBOSE(("enableHbbTv(): isAppRunning: %d, currentAppType: %d, m_is_hbbtv_app_starting: %d\n",
                    isAppRunning(), convertToUnderlyingType(getCurrentAppType()),
                    m_is_hbbtv_app_starting));

            while ((isAppRunning() && 
                        (getCurrentAppType() == WebBrowserAppType::hbbtv_app || 
                         getCurrentAppType() == WebBrowserAppType::hbbtv_txt_app)) ||
                        (m_is_hbbtv_app_starting
                            && (Profile_IsTiVoBrowserLauncherEnabled() == tivo_browser_launcher_disabled)))
            {
                wait_count +=1;
                FROST_Sleep(10);
                if(wait_count > 1000)
                {
                    break;
                }
                if (wait_count % 100 == 0)
                {
                    TRACE_INFO(("Waited %d seconds for application to stop before disabling hbbtv\n", 
                        wait_count/100));
                }
            }

            TRACE_VERBOSE(("enableHbbTv(): Waited %d seconds for application to stop before disabling hbbtv, wait_count: %d\n",
                wait_count/100, wait_count));
#endif
        }
        else
        {
            TRACE_INFO(("Calling method requested termination of Linear HbbTV Service "
                "to be managed by lifecycle management\n"));
        }
    }
}

void
OperaEngine::setSelectServiceUponAppTermination(bool select_upon_termination)
{
    TRACE_INFO(("Select service after termination: %d\n", select_upon_termination));
    m_select_service_upon_app_termination = select_upon_termination;
}

void
OperaEngine::setHbbTVWindowActive(bool active)
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("Make hbbtv window %sactive \n",active ? "": "in"));
    m_window_manager->setActive(m_window_manager->hbbTVWindow(), active);
}

cabot::String const&
OperaEngine::userAgentString()
{
    return m_user_agent_string;
}

OperaWindowManager const*
OperaEngine::getWindowManager()
{
    return m_window_manager;
}

void
OperaEngine::raiseStreamEvent(unsigned int listener_id,
        unsigned int video_id,
        const std::string& event_name,
        const std::string& event_data,
        const std::string& event_text,
        const std::string& event_status)
{
/*
    omi::msg::RaiseDSMCCStreamEvent::Result ret = post(omi::msg::RaiseDSMCCStreamEvent(
                        m_window_manager->hbbTVWindow().handle(),
                        listener_id,
                        video_id,
                        event_name,
                        event_data,
                        event_text,
                        event_status)).get();
*/
    post(omi::msg::RaiseDSMCCStreamEvent(
                        m_window_manager->hbbTVWindow().handle(),
                        listener_id,
                        video_id,
                        event_name,
                        event_data,
                        event_text,
                        event_status));

    //TRACE_INFO((" OperaEngine::raiseStreamEvent ret.errorString() %s\n", ret.errorString().c_str()));
}

void OperaEngine::setMouseMaxBounds(int width, int height)
{
    m_mouse_cursor_position.setMaxBounds(width, height);
}

void OperaEngine::printCurrentMemoryUsage()
{
    using omi::msg::GetBrowserMemoryInformation;

    GetBrowserMemoryInformation::Result result = post(GetBrowserMemoryInformation()).get();
    if (checkResult(result))
    {
        TRACE_INFO(("Current browser system memory usage: %" PRIi64 "\n", result.allocated));
        TRACE_INFO(("Current browser GPU memory usage: %" PRIi64 "\n", result.allocated_gpu));
    }
}

void OperaEngine::closedURLTimeout()
{
    m_closed_url_started = false;
    m_closed_window_url = ("");
}

void OperaEngine::startMemoryAccounting(unsigned int print_every_msec)
{
#if defined(MEMORY_ACCOUNTING) && defined(CABOT_TRACE)
    CABOT_ASSERT(print_every_msec > 0);

    if (CabotTimerInitialise())
    {
        if (CabotTimerCreate(::traceMemoryUsageTimerFunc, this, &m_memory_acct_timer,
                cabot_timer_auto_repeat))
        {
            if (CabotTimerStart(m_memory_acct_timer, print_every_msec))
            {
                TRACE_INFO(("Memory accounting is started, every %d msecs\n", print_every_msec));
            }
        }
    }
#else
    CABOT_UNUSED_PARAM(print_every_msec);
#endif
}

void
OperaEngine::threadMain()
{
    m_engine_running = true;
    while (m_engine_running)
    {
        if (omi::Event event = m_opera->poll(1000 * 5))
        {
            const uint64_t event_type = static_cast<uint64_t>(event.type());
            TRACE_VERBOSE(("Received event: 0x%" PRIx64 " %s\n",
                                event_type, EventNames::getName(event_type)));
            std::pair<EventProcessors::const_iterator,
                    EventProcessors::const_iterator> suitable_processors =
                    m_event_processors.equal_range(event.type());

            bool processed = false;
            for (EventProcessors::const_iterator it = suitable_processors.first;
                    it != suitable_processors.second; ++it)
            {
                it->second->processEvent(event);
                processed = true;;
            }
            if (!processed)
            {
                TRACE_WARN(("Unprocessed Event: 0x%" PRIx64 " %s\n",
                                event_type, EventNames::getName(event_type)));
            }
            m_last_received_event = event_type;
        }
#if defined USING_SDK422 || defined USING_SDK423
        else if (CX_INCLUDE_BROWSER_RESTART_SUPPORT && m_do_restart)
        {
            m_restart.applyRestartDelay();

            m_do_restart = false;

            if (m_opera->IsRunning())
            {
                const bool shutdown_ok = m_opera->Shutdown();
                TRACE_WARN(("%s() Shutdown OK: %s\n", __func__, 
                            shutdown_ok ? "true" : "false"));
            }

            const bool start_ok = m_opera->Start();
            TRACE_WARN(("%s() Start OK: %s\n", __func__, start_ok ? "true" : "false"));

            CABOT_ASSERT(m_opera->IsRunning());

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
            //Register as a broadcast resource user
            //post(omi::msg::RegisterBroadcastResource(0, { 0 })).get();
            post(omi::msg::RegisterBroadcastResource(0, { 0 }));
#endif

            m_window_manager->notifyBrowserProcessStateChange(
                                            browser_process_state_restarted);
            if (m_event_publisher)
            {
                m_event_publisher->notifyBrowserProcessStateChange(
                                                browser_process_state_restarted);
            }
        }
#endif
    }
}

void
OperaEngine::enableUrlFiltering()
{
    setPreference("urlfilter.dynamic_enabled", "true");
}

void OperaEngine::disableCertificateErrorPage(OperaWindowHandle window_handle)
{
    const UString value = "false";
    const UString key = "default_cert_error_page_enabled";
    OperaWindow* window = m_window_manager->getWindowByHandle(window_handle);
    if (window)
    {
        m_window_manager->setPreferenceValue(*window, key, value);
    }
}

void OperaEngine::enableNotifySriptsClosingWindows(OperaWindowHandle window_handle)
{
    const UString value = "true";
    const UString key = "notify_scripts_closing_windows";
    OperaWindow* window = m_window_manager->getWindowByHandle(window_handle);
    if (window)
    {
        m_window_manager->setPreferenceValue(*window, key, value);
    }
}

void OperaEngine::setXhrOriginCheck(OperaWindowHandle window_handle, const UString& value)
{
    const UString key = "xhr_origin_check_enabled";
    UString value_org;
    OperaWindow* window = m_window_manager->getWindowByHandle(window_handle);
    if (window)
    {
        m_window_manager->setPreferenceValue(*window, key, value);
    }
}

void OperaEngine::setStorageLimits()
{
    bool result = true;

    unsigned int max_disk_cache_size      = 20 * 1024; // Opera's default settings noted in 250 MB.
    unsigned int local_storage_limit      = 10 * 1024; // Opera's default settings noted in 64 MB. each app can at most use 10MB of the shared pool
    unsigned int persistent_storage_limit = 10 * 1024; // Opera's default settings noted in 64 MB.
    unsigned int temporary_storage_limit  = 64 * 1024; // Opera's default settings noted in 16 MB. %20 of it is used for App Cache so we had to keep it high for Vestel Portal

    // Opera has an internal disk cache limit of 512 MB.
    // If a value greater than 512 MB is set, Opera disables the cache.
    unsigned int total_storage_limit      =   512 * 1024; // 512 MB

    if (::getPersistentStorageLimit(&total_storage_limit))
    {
        if(total_storage_limit > (local_storage_limit + persistent_storage_limit + temporary_storage_limit))
        {
            max_disk_cache_size = total_storage_limit - ( local_storage_limit + persistent_storage_limit + temporary_storage_limit );
        }
        else
        {
            max_disk_cache_size = 0;
        }
        TRACE_INFO((" - total_storage_limit: %u kB\n", total_storage_limit));
        TRACE_INFO((" - max_disk_cache_size: %u kB\n", max_disk_cache_size));
        TRACE_INFO((" - local_storage_limit: %u kB\n", local_storage_limit));
        TRACE_INFO((" - persistent_storage_limit: %u kB\n", persistent_storage_limit));
        TRACE_INFO((" - temporary_storage_limit: %u kB\n", temporary_storage_limit));
    }
    else
    {
        max_disk_cache_size      = 512 * 1024; // 512 MB
        local_storage_limit      = 512 * 1024; // 512 MB
        persistent_storage_limit = 512 * 1024; // 512 MB
        temporary_storage_limit  = 512 * 1024; // 512 MB

        TRACE_INFO((" - setStorageLimits: Undefined, and limited to %u kB\n", local_storage_limit));
    }

    result = result && setGlobalPreference("max_disk_cache_size", UString::fromUInt(max_disk_cache_size));
    result = result && setGlobalPreference("local_storage_limit", UString::fromUInt(local_storage_limit));
#if !defined USING_SDK422 && !defined USING_SDK423
    result = result && setGlobalPreference("persistent_storage_limit", UString::fromUInt(persistent_storage_limit));
#endif
    result = result && setGlobalPreference("temporary_storage_limit", UString::fromUInt(temporary_storage_limit));

    CABOT_ASSERT(result);
}

void OperaEngine::setMseBufferMemoryLimits()
{
    const UString video_limit_key = "media_source_buffer_memory_limit.default_video_limit";
    const UString video_limit_value = "80000000";

    const UString audio_limit_key = "media_source_buffer_memory_limit.default_audio_limit";
    const UString audio_limit_value = "10000000";

    bool result = setGlobalPreference(video_limit_key, video_limit_value);
    result = result && setGlobalPreference(audio_limit_key, audio_limit_value);

    TRACE_INFO(("setMseBufferMemoryLimits() video_limit: %s - result: %d\n", video_limit_value.charArray(), result));

    const UString max_stream_bitrate_key = "media_caps_max_stream_bitrate";
    const UString max_stream_bitrate_value = "60000000";

    result = result && setGlobalPreference(max_stream_bitrate_key, max_stream_bitrate_value);

    TRACE_INFO(("setMseBufferMemoryLimits() setting %s:%s - result: %d\n", max_stream_bitrate_key.charArray(), max_stream_bitrate_value.charArray(), result));

    CABOT_ASSERT(result);
}

bool
OperaEngine::setPreference(UString const& key, UString const& value)
{
    CABOT_ASSERT(m_window_manager);

    bool retval = false;

    OperaWindow* active_window = m_window_manager->getActiveWindow();
    if (!active_window)
    {
        // Some preferences' scopes are 'Global' that means they are window independent
        // Therefore try to set key->value even if there is not any active window.
        retval = setGlobalPreference(key, value);
    }
    else
    {
        retval = m_window_manager->setPreferenceValue(*active_window, key, value);
    }

    return retval;
}

bool
OperaEngine::getGlobalPreference(UString const& key, UString & value)
{
    CABOT_ASSERT(m_window_manager);

    OperaWindow* temp_window = m_window_manager->createGenericWindow("TEMP");
    bool result = m_window_manager->getPreferenceValue(*temp_window, key, value);
    m_window_manager->destroyWindow(temp_window);

    return result;
}

bool
OperaEngine::setGlobalPreference(UString const& key, UString const& value)
{
    CABOT_ASSERT(m_window_manager);

    OperaWindow* temp_window = m_window_manager->createGenericWindow("TEMP");
    bool result = m_window_manager->setPreferenceValue(*temp_window, key, value);
    m_window_manager->destroyWindow(temp_window);

    return result;
}

bool OperaEngine::setHttpProxy(UString addr)
{
    return setPreference("proxy", addr);
}

void OperaEngine::sendInvalidateAitMessage(OperaWindowHandle window_handle)
{
    // Note: This may cause "Unexpected InvalidateAIT event received" trace from
    //       the browser.  However, if we try to avoid this, the sequence of
    //       events received changes, causing incorrect behaviour.
    using omi::msg::InvalidateAIT;
#ifdef USING_SDK423
    // Workaround for browser hang using get() sometimes on channel change.
    // TODO: Remove when fixed by Vewd - STEALTH-1800.
    post(InvalidateAIT());
#else
#ifdef USING_SDK422
    InvalidateAIT::Result result = post(InvalidateAIT()).get();
#else
    InvalidateAIT::Result result = post(InvalidateAIT(window_handle)).get();
#endif
    TRACE_VERBOSE(("%s(%d) result.errorString(): %s\n",
                   __FUNCTION__, __LINE__, result.errorString().c_str()));
#endif
}

void OperaEngine::sendChannelChangedMessage(NEBULA_Channel_ID channel_id)
{
    TRACE_INFO(("sendChannelChangedMessage(%d)\n", channel_id));

    NEBULA_Channel channel_info;
    if (NEBULA_GetChannelInfoByID(channel_id, &channel_info) == frost_false)
    {
        // For correct behaviour (e.g. org.hbbtv_00000450), ChannelChanged must
        // still be sent.  Setting the ccid to an empty string may be sufficient.
        memset(&channel_info, 0, sizeof channel_info);
    }

    if (NEBULA_IsLinearIPService(channel_info.networkType) == frost_false)
    {
        OperaWindowHandle const & window_handle = m_window_manager->hbbTVWindow().handle();

        TRACE_INFO(("Sending ChannelChanged to browser window(%s), channel: %s\n",
                    window_handle.str().c_str(),
                    strlen(channel_info.ccid) > 0 ? channel_info.ccid : "none"));

        using omi::msg::ChannelChanged;
#ifdef USING_SDK423
        // Workaround for browser hang using get() sometimes on channel change.
        // TODO: Remove when fixed by Vewd - STEALTH-1800.
        post(ChannelChanged(window_handle,
                            channel_info.onid,
                            channel_info.tsid,
                            channel_info.sid,
                            channel_info.ccid));
#else
        ChannelChanged::Result result = post(ChannelChanged(window_handle,
                                                            channel_info.onid,
                                                            channel_info.tsid,
                                                            channel_info.sid,
                                                            channel_info.ccid)).get();
        TRACE_VERBOSE(("%s(%d) result.errorString(): %s\n",
                    __FUNCTION__, __LINE__, result.errorString().c_str()));
#endif
    }
    else
    {
        TRACE_INFO(("Not forwarding Linear IP Service information for channel id '%d' \n", 
            channel_id));
    }
}

bool OperaEngine::handleXmlAitUrl(cabot::String const& url, bool parse_ait_xml, bool broadcast_related, bool allow_auto_transit)
{
    //TODO: "As defined in ETSI TS 102 809 V1.2.1 point "5.4 XML-based syntax": The file extension
    //shall be '.aitx'." BBC does not comply to standards. See DOM-280 for details.
    CABOT_UNUSED_PARAM(broadcast_related);
    if (parse_ait_xml ||
        url.endsWith(".aitx") || url.find("ait/launch") != cabot::String::npos)
    {
        TRACE_INFO(("Loading broadcast independent application "
                    "from XML AIT (%s) using hbbtv handle %s, allow_auto_transit: %d\n",
                    url.charArray(),
                    m_window_manager->hbbTVWindow().handle().str().c_str(), allow_auto_transit));

        post(omi::msg::LoadURLApplication(
                                    m_window_manager->hbbTVWindow().handle(),
                                    url.charArray(),
                                    allow_auto_transit));
        // SUNSTONE-747 to ensure isAppRunning returning true
        m_window_manager->setActive(m_window_manager->hbbTVWindow(), true);
        return true;
    }

    return false;
}

bool OperaEngine::handleTVStoreUrl(cabot::String const& url)
{
    if (url.find("tvstore.opera") != cabot::String::npos
            || url.find("team.opera.com/tvstore-test") != cabot::String::npos
            || url.find("appstore://") != cabot::String::npos)
    {
        return m_window_manager->startTVStoreApp(url);
    }

    return false;
}

/*static*/ void
OperaEngine::setGPUMemAsFreed(void* /*user_data*/)
{
    m_gpu_mem_freeing = false;
}

void
OperaEngine::focusHbbtvApplication(bool const focus_application)
{
    OperaHbbTVWindow& hbbtv_window = m_window_manager->hbbTVWindow();
    m_window_manager->setFocusable(hbbtv_window, focus_application);
}

bool
OperaEngine::accessCriteriaChanged(cabot::String const& message)  const
{
#ifdef INCLUDE_XPCA
    TRACE_INFO(("accessCriteriaChanged notification is received\n"));
    UVANotifyPlatformAccessCriteriaChanged(message.charArray());

    return true;
#else
    CABOT_UNUSED_PARAM(message);

    return false;
#endif
}

void
OperaEngine::onCicamStatusChange(bool available)
{
    CABOT_ASSERT(m_window_manager);
    CABOT_ASSERT(m_opera);
    TRACE_INFO(("Cicam %s available\n", available ? "" : "not"));

    m_window_manager->hbbTVWindow().onCicamStatusChange(available);
}

void
OperaEngine::onAuxfsContentReady(unsigned int transaction_id, const unsigned char* data, unsigned int data_length)
{
    FrostMutexLock lock(m_auxfs_requests_mutex);
    AuxfsRequests::iterator it = m_auxfs_requests.find(transaction_id);
    {
        if (it == m_auxfs_requests.end())
        {
             TRACE_WARN(("CICAMFileResponse transaction id %u: no matching request\n", transaction_id));
        }
        else
        {
            omi::evt::CICAMFileRequested* an_event = it->second;
            if (data)
            {
                TRACE_INFO(("CICAMFileResponse transaction id %u: content found with length %u\n", transaction_id, data_length));
                std::vector<unsigned char> payload(data, data + data_length);
                an_event->ok(payload);
            }
            else
            {
                TRACE_INFO(("CICAMFileResponse transaction id %u: content not found\n", transaction_id));
                an_event->not_found();
            }
            delete it->second;
            m_auxfs_requests.erase(it);
        }
    }
}

void OperaEngine::onMediaObjectChanged(omi::evt::MediaObjectChanged media_object_changed)
{
    TRACE_VERBOSE(("MediaObjectChanged event received (window_id %s, reason %s, media_object %d, %s\n",
                media_object_changed.window_id.str().c_str(),
                media_object_changed.ToString(media_object_changed.reason).c_str(),
                media_object_changed.media_object.id,
                media_object_changed.media_object.playing ? "Playing" : "NOT Playing"
              ));

    for (auto&& track : media_object_changed.media_object.tracks)
    {
        TRACE_VERBOSE(("    MediaObject Track %s %s %s\n",
                       track.id.c_str(),
                       omi::common::ToString(track.track_type).c_str(),
                       track.active ? "active" : "inactive"));
    }
    if (media_object_changed.reason == omi::evt::MediaObjectChanged::REMOVED)
    {
        //Inform the browser of the current media components preferences.
        NEBULA_WebBrowserSetMediaComponentsPreferences();
    }

#if defined (ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    if (m_opera_resource_manager)
    {
        m_opera_resource_manager->onMediaObjectChanged(media_object_changed);
    }
#endif
    if (m_window_manager && m_window_manager->mediaPlayerWindow().isLinearIPService()
        && media_object_changed.media_object.playing)
    {
        TRACE_INFO(("Media Player notified that Media Object is playing\n"));
        m_media_player_event_publisher->notifyPlayingState(media_object_changed.media_object.playing);
    }
}

#ifdef INCLUDE_OPAPP_SUPPORT
void
OperaEngine::onOpAppApplicationStartResult(omi::evt::OpAppApplicationStartResult start_result)
{
    TRACE_INFO(("HbbTV OpApp %x.%x %sstarted\n",
                start_result.app_id, start_result.org_id,
                start_result.started ? "" : "not "));

    if (m_window_manager)
    {
        OperaWindow* window = m_window_manager->getWindowByHandle(start_result.window_id);
        if (window)
        {
            if (start_result.started)
            {
                if (!isBrowserLauncherVisible())
                {            
                    m_window_manager->setActive(*window, true);
                    if (!m_window_manager->isVisible(*window))
                    {
                        m_window_manager->setVisible(*window, true);
                    }
                    m_opapp_active_postponed = false;
                }
                else
                {
                    m_opapp_active_postponed = true;
                }
            }
            else
            {
                m_window_manager->reset(*window);
            }
        }

        if (m_opapp_event_publisher)
        {
            m_opapp_event_publisher->notifyOpAppApplicationStartResult(start_result);
        }
    }
}

void
OperaEngine::onOpAppApplicationStopped(omi::evt::OpAppApplicationStopped stopped)
{
    TRACE_INFO(("HbbTV OpApp stopped\n"));

    m_window_manager->opAppWindow().stopped(stopped);
    OperaWindow* window = m_window_manager->getWindowByHandle(stopped.window_id);
    if (window)
    {
        m_window_manager->reset(*window);
    }

    if (m_opapp_event_publisher)
    {
        m_opapp_event_publisher->notifyOpAppApplicationStopped(stopped);
    }
}

void
OperaEngine::onOpAppStateChanged(omi::evt::OpAppStateChanged state_changed)
{
    TRACE_INFO(("HbbTV OpApp state changed from %d to %d\n",
                static_cast<int>(state_changed.old_state),
                static_cast<int>(state_changed.new_state)));

    m_window_manager->opAppWindow().stateChanged(state_changed);
    m_window_manager->updateActiveWindows();

    if (m_opapp_event_publisher)
    {
        m_opapp_event_publisher->notifyOpAppStateChanged(state_changed);
    }
}

void
OperaEngine::onOpAppKeysetChanged(omi::evt::OpAppKeySetChanged keyset_changed)
{
    std::vector<int> others;
    for (unsigned int i = 0; i < keyset_changed.other_keys.size(); ++i)
    {
        others.push_back(static_cast<int>(keyset_changed.other_keys[i]));
    }
    TRACE_INFO(("HbbTV OpApp keyset changed - app_id = %x, org_id = %x, "
                                             "keyset = %#x, other_keys = %s\n",
                keyset_changed.app_id,
                keyset_changed.org_id,
                keyset_changed.new_keyset,
                otherKeysToString(others).c_str()));

    CABOT_ASSERT(m_window_manager);

    OperaWindow* window = m_window_manager->getWindowByHandle(keyset_changed.window_id);
    if (window)
    {
        window->setKeySet(keyset_changed.new_keyset, others);
    }
}

namespace
{

std::string
toString(std::vector<omi::common::OperatorApplicationId> const & ids)
{
    std::vector<std::string> id_strings;
    std::transform(ids.begin(), ids.end(), std::back_inserter(id_strings),
                   [&](omi::common::OperatorApplicationId id)
                   {
                        std::ostringstream buffer;
                        buffer << std::hex << id.app_id << "." << id.org_id;
                        return buffer.str();
                   });
    return cabot::join(id_strings.begin(), id_strings.end(), " ");
}

} // namespace

#ifdef USING_SDK420
void
OperaEngine::onOpAppApplicationsDiscovered(
                            omi::evt::OpAppApplicationsDiscovered discovered)
{
    auto* window = m_window_manager->getWindowByName(OperaWindow::window_name_opapp);
    if (!window)
    {
        TRACE_ERROR(("OpApp window is not valid!\n"));
        return;
    }

    TRACE_INFO(("HbbTV OpApp discovered - %s, %s\n",
                toString(discovered.applications).c_str(),
                ToString(discovered.discovery_reason).c_str()));
    using SendStatus = OpAppDiscoveryMonitor::OpAppNotificationSendStatus;
    SendStatus send_status = 
            m_window_manager->opAppWindow().applicationsDiscovered(discovered);

    if (send_status == SendStatus::notification_needed &&
        m_opapp_event_publisher)
    {
        std::vector<OpAppDiscoveryMonitor::DiscoveredOpAppData> added_opapps, uninstalled_opapps;
        
        auto& window = m_window_manager->opAppWindow();
        window.notifyOpAppApplicationsDiscovered(discovered,
                                                 added_opapps,
                                                 uninstalled_opapps);

        for (const auto& app : added_opapps)
        {
            m_opapp_event_publisher->notifyOpAppDiscovered(app.m_app_id,
                                                           app.m_org_id,
                                                           app.m_version);
        }

        for (const auto& app : uninstalled_opapps)
        {
            m_opapp_event_publisher->notifyOpAppUninstalled(app.m_app_id, app.m_org_id);
        }
    }
}

void
OperaEngine::onOpAppApplicationsNotDiscovered(
                        omi::evt::OpAppApplicationsNotDiscovered not_discovered)
{
    auto* window = m_window_manager->getWindowByName(OperaWindow::window_name_opapp);
    if (!window)
    {
        TRACE_ERROR(("OpApp window is not valid!\n"));
        return;
    }

    TRACE_INFO(("HbbTV OpApp not discovered - %s\n",
                ToString(not_discovered.discovery_reason).c_str()));
    using SendStatus = OpAppDiscoveryMonitor::OpAppNotificationSendStatus;
    SendStatus send_status = 
        m_window_manager->opAppWindow().applicationsNotDiscovered(not_discovered);

    if (send_status == SendStatus::notification_needed &&
        m_opapp_event_publisher)
    {
        m_opapp_event_publisher->notifyOpAppNotDiscovered();
    }
}

void
OperaEngine::onOpAppApplicationInstallRequest(omi::evt::OpAppApplicationInstallRequest request)
{
    TRACE_INFO(("HbbTV OpApp Install Request - %x.%x v%d\n",
                request.app.app_id, request.app.org_id, request.app.version));
    m_window_manager->opAppWindow().installRequest(request);
}
#endif

#ifdef USING_SDK423
void
OperaEngine::onOpAppDiscoveryServiceBroadbandDiscoveryStarted(
    omi::evt::OpAppDiscoveryServiceBroadbandDiscoveryStarted discovery_started)
{
    // Not currently used.
    for (auto const & started_discovery : discovery_started.started_discoveries)
    {
        TRACE_INFO(("HbbTV OpApp Discovery Started - %x method: %x\n",
                    started_discovery.org_id,
                    started_discovery.discovery_method_mask));
    }
}

void
OperaEngine::onOpAppDiscoveryServiceBroadbandDiscoveryPartialResult(
            omi::evt::OpAppDiscoveryServiceBroadbandDiscoveryPartialResult partial_result)
{
    TRACE_INFO(("HbbTV OpApp Discovery Partial Result - %x method: %x status: %s\n",
                partial_result.org_id,
                partial_result.discovery_method,
                partial_result.ToString(partial_result.status).c_str()));

    m_window_manager->opAppWindow().discoveryResult(partial_result);
}

void
OperaEngine::onOpAppDiscoveryServiceInstallRequest(
    omi::evt::OpAppDiscoveryServiceInstallRequest request)
{
    TRACE_INFO(("HbbTV OpApp Install Request - %x.%x v%d\n",
                request.app.app_id, request.app.org_id, request.app.version));
    m_window_manager->opAppWindow().installRequest(request);
}

void
OperaEngine::onOpAppDiscoveryServiceAvailableApplications(
            omi::evt::OpAppDiscoveryServiceAvailableApplications available_applications)
{
    TRACE_INFO(("HbbTV OpApp Available Applications - %s, %s\n",
                toString(available_applications.applications).c_str(),
                available_applications.ToString(available_applications.reason).c_str()));

    using SendStatus = OpAppDiscoveryMonitor::OpAppNotificationSendStatus;
    SendStatus send_status = 
            m_window_manager->opAppWindow().availableApplications(available_applications);

    if (send_status == SendStatus::notification_needed &&
        m_opapp_event_publisher)
    {
        std::vector<OpAppDiscoveryMonitor::DiscoveredOpAppData> added_opapps, uninstalled_opapps;
        
        auto& window = m_window_manager->opAppWindow();
        window.notifyOpAppApplicationsDiscovered(available_applications,
                                                 added_opapps,
                                                 uninstalled_opapps);
        if (added_opapps.size() == 0 && uninstalled_opapps.size() == 0)
        {
            // Uninstalling an OpApp should not trigger this notification.
            m_opapp_event_publisher->notifyOpAppNotDiscovered();
        }
        for (const auto& app : added_opapps)
        {
            m_opapp_event_publisher->notifyOpAppDiscovered(app.m_app_id,
                                                           app.m_org_id,
                                                           app.m_version);
        }
        for (const auto& app : uninstalled_opapps)
        {
            m_opapp_event_publisher->notifyOpAppUninstalled(app.m_app_id, app.m_org_id);
        }
    }
}
#endif

void
OperaEngine::onOpAppReplaceUiElements(omi::evt::OpAppReplaceUiElements replace_ui_elements)
{
    TRACE_INFO(("OperaEngine::onOpAppReplaceUiElements\n"));

    auto opapp_ui_elements_manager = nebula::opAppUIElementsManager();
    if (opapp_ui_elements_manager)
    {
        std::vector<int> replaced_elements;

        opapp_ui_elements_manager->replaceUIElements(
                            replace_ui_elements.elements_to_replace,
                            replaced_elements);
        TRACE_INFO(("OperaEngine::onOpAppReplaceUiElements() replaced_elements size = %zu\n", replaced_elements.size()));
        replace_ui_elements.ElementsReplaced(replaced_elements);
    }
}
#endif // INCLUDE_OPAPP_SUPPORT

void
OperaEngine::setMediaComponentsPreferences(
    std::vector<std::string> const & preferred_subtitle_languages,
    bool subtitles_enabled,
    std::vector<std::string> const & preferred_audio_languages,
    bool audio_description_enabled)
{
    TRACE_INFO(("setMediaComponentsPreferences(subtitles: %s [%s]; audio: [%s], AD %s)\n",
                subtitles_enabled ? "on" : "off",
                cabot::join(preferred_subtitle_languages, ", ").c_str(),
                cabot::join(preferred_audio_languages, ", ").c_str(),
                audio_description_enabled ? "on" : "off"));

    const auto active_window = m_window_manager->getActiveWindowForComponents();

    m_component_preferences.enable_subtitle = subtitles_enabled;
    m_component_preferences.preferred_subtitle_languages = preferred_subtitle_languages;
    m_component_preferences.preferred_audio_languages = preferred_audio_languages;
    setAudioDescriptionPreference(audio_description_enabled);

    OperaWindowHandle hbbtv_window_handle = m_window_manager->hbbTVWindow().handle();
    bool apply_preferences_to_hbbtv_window = (active_window == nullptr);

    if (active_window)
    {
        OperaWindowHandle handle = active_window->handle();
        m_component_preferences.window_id = handle;

        if(active_window->type() == OperaWindow::WindowType::window_csp || active_window->type() == OperaWindow::WindowType::window_browser_ui)
        {
           TRACE_VERBOSE(("setMediaComponentsPreferences subtitle mode is enabled for launcher and browser apps\n"));
           m_component_preferences.enable_subtitle = true;
        }            

        post(m_component_preferences);
        apply_preferences_to_hbbtv_window = (handle != hbbtv_window_handle);
    }

    TRACE_INFO(("setMediaComponentsPreferences apply_preferences_to_hbbtv_window: [%d] \n", apply_preferences_to_hbbtv_window));
    // Make sure that preferences are always applied to HbbTv window to fix LUKE-6989.
    if (apply_preferences_to_hbbtv_window)
    {
        m_component_preferences.window_id = hbbtv_window_handle;
        post(m_component_preferences);
    }
}

namespace
{
cabot::String getCertificateFolder()
{
    cabot::String certificates_dir = getenv("BROWSER_HOME");
    certificates_dir += "/certificates/";
#ifdef INCLUDE_G31    
    cabot::String folder_name = "oem_g31";
#else
    cabot::String folder_name = "oem";
#endif

    certificates_dir += folder_name;

    TRACE_INFO(("Certificate path: (%s)\n", certificates_dir.charArray()));
    return certificates_dir;
}

#ifdef INCLUDE_HOTELTV
cabot::String getUserDataDirectory()
{
    cabot::String user_data_dir = getenv("BROWSER_HOME");

    return user_data_dir;
}

cabot::String getTrustedOrigin()
{
    cabot::String trusted_origin;
    cabot::Url base_url;

    base_url.parse(DEFAULT_HOTELTV_URL);// Default hoteltv url will be used if there is no starturl

    EASIFilePath starturl_path(STARTURL_FILE_PATH);
    ICE_FsHandle* starturl_handle;

    if (EASIFilePath::result_ok == starturl_path.openAbsoluteNameFile(EASIFilePath::read_only, &starturl_handle))
    {
        //Starturl.txt file exists
        unsigned int bytes_read = 0;
        unsigned char startUrl[100] = {0};

        starturl_path.readFile(starturl_handle, startUrl, 100, &bytes_read);

        cabot::String start_url(startUrl,bytes_read);

        if(!start_url.isEmpty() && start_url.find("localhost") == cabot::String::npos)
        {
            base_url.parse(start_url.charArray());
        }

        starturl_path.closeFile(starturl_handle);
    }

    trusted_origin = base_url.getOrigin();

    trusted_origin.replaceAll("https","http");
    trusted_origin.replaceAll(":443",":80");

    TRACE_INFO(("Trusted Origin: (%s)\n", trusted_origin.charArray()));

    return trusted_origin;
}
#endif

#if defined(MEMORY_ACCOUNTING) && defined(CABOT_TRACE)
void traceMemoryUsageTimerFunc(void* user_data)
{
    OperaEngine* engine = static_cast<OperaEngine*>(user_data);
    if (engine)
    {
        engine->printCurrentMemoryUsage();
    }
}
#endif

void closedURLTimerFunc(void* user_data)
{
    OperaEngine* engine = static_cast<OperaEngine*>(user_data);
    if (engine)
    {
        engine->closedURLTimeout();
    }
}

void getMemoryLimits(cabot::String& system_mem_limit, cabot::String& gpu_mem_limit)
{
    char board_name[32] = {0};
    Profile_GetBoardName(board_name, 32);
    cabot::String board_name_str(board_name);
    if (board_name_str == "MB230")
    {
        system_mem_limit = cabot::String::fromUInt32(450*1024*1024);
        gpu_mem_limit = "150";
    }
    else if (board_name_str == "MB181" ||
             board_name_str == "MB180" ||
             board_name_str == "MB180R" ||
             board_name_str == "MB181R")
    {
#ifdef INCLUDE_G31
        system_mem_limit = cabot::String::fromUInt32(750*1024*1024);
        gpu_mem_limit = "240";
#else
        system_mem_limit = cabot::String::fromUInt32(550*1024*1024);
        if (board_name_str == "MB181" ||
            board_name_str == "MB181R")
        {
            gpu_mem_limit = "150";
        }
        else // MB180 - G32
        {
            gpu_mem_limit = "180";
        }
#endif
    }
    else if (board_name_str == "TCS9700")
    {
        system_mem_limit = cabot::String::fromUInt32(550*1024*1024);
        gpu_mem_limit = "150";
    }
    else if (board_name_str == "MB281")
    {
        system_mem_limit = cabot::String::fromUInt32(380*1024*1024);
        gpu_mem_limit = "150";
    }
    else
    {
        system_mem_limit = cabot::String::fromUInt32(350*1024*1024);
        gpu_mem_limit = "120";
    }


    TRACE_INFO(("System memory limit in bytes(%s), gpu mem limit in mbytes (%s) for platform (%s)\n",
                    system_mem_limit.charArray(),
                    gpu_mem_limit.charArray(), board_name_str.charArray()));
}

bool getPersistentStorageLimit(unsigned int* storage_limit_kb)
{
    if (NEBULA_SystemInformationProviderQueryBrowserPersistentStorageLimit(storage_limit_kb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void cicamStatusCallback(NEBULA_CallbackUser user, frost_bool available)
{
    OperaEngine* engine = static_cast<OperaEngine*>(user);
    if (engine)
    {
        engine->onCicamStatusChange(available==frost_true);
    }
}

void auxfsReaderCallback(NEBULA_CallbackUser user, frost_uint transaction_id, const unsigned char* data, frost_uint data_length)
{
    OperaEngine* engine = static_cast<OperaEngine*>(user);
    if (engine)
    {
        engine->onAuxfsContentReady(transaction_id, data, data_length);
    }
}

#ifdef INCLUDE_XPCA
void xpcaOperatorCallback(const char* key, const char* value)
{
    NEBULA_ReturnOperatorActions(key, value);
}
#endif

ait::ApplicationControlCodes convertOperaControlCode(
                omi::evt::AITUpdated::HbbTVApplicationControlCode control_code_to_convert)
{
    ait::ApplicationControlCodes control_code = ait::application_control_code_low_future_use;
    switch (control_code_to_convert)
    {
    case omi::evt::AITUpdated::AUTOSTART:
        control_code = ait::application_control_code_autostart;
        break;

    case omi::evt::AITUpdated::PRESENT:
        control_code = ait::application_control_code_present;
        break;

    case omi::evt::AITUpdated::KILL:
        control_code = ait::application_control_code_kill;
        break;

    case omi::evt::AITUpdated::DISABLED:
        control_code = ait::application_control_code_disabled;
        break;
    }
    return control_code;
}
}

std::uint16_t
OperaEngine::getHbbTvApplicationProfile() const
{
    return m_hbbtv_application_profile;
}

int
OperaEngine::getHbbTvProfile() const
{
    return m_hbbtv_profile;
}

void OperaEngine::tracePost(const omi::Message& msg) const
{
    const uint64_t msg_type = static_cast<uint64_t>(msg.type());
    TRACE_VERBOSE(("Posted message: 0x%" PRIx64 " %s\n",
                        msg_type, MessageNames::getName(msg_type)));
}

void OperaEngine::onBrowserGPUMemoryEvent(omi::evt::BrowserGPUMemoryEvent browser_gpu_memory_event)
{
#ifdef CABOT_TRACE
    using omi::msg::GetBrowserMemoryInformation;

    GetBrowserMemoryInformation::Result result = post(GetBrowserMemoryInformation()).get();
    int in_use_mib = checkResult(result) ? result.allocated_gpu / (1000 * 1000) : -1;

    cabot::String system_mem_limit;
    cabot::String gpu_mem_limit;
    ::getMemoryLimits(system_mem_limit, gpu_mem_limit);

    TRACE_INFO(("Current GPU memory allocation accounted by Vewd: %d Mbytes, GPULimit: %s\n", in_use_mib, gpu_mem_limit.charArray()));
#endif

    switch(browser_gpu_memory_event.state)
    {
        case omi::evt::BrowserGPUMemoryEvent::OK:
            TRACE_INFO(("GPU Memory is OK \n"));
            break;
        case omi::evt::BrowserGPUMemoryEvent::LOW:
        {
            TRACE_INFO(("GPU Memory is LOW \n"));
            OperaWindow* browser_ui_window = m_window_manager->getActiveWindowByName(
                                                    OperaWindow::window_name_browser_ui);
            if (browser_ui_window)
            {
                OperaWindowHandle handle = browser_ui_window->handle();
                cabot::String const url = browser_ui_window->getCurrentPageUrl();
                onAppContentError(handle,
                                  0,
                                  url.c_str(),
                                  false,
                                  true);

                TRACE_ALWAYS(("Terminating open browser due to insufficient GPU memory!!!\n"));
                resetTvBrowserPage();
            }
            break;
        }
        case omi::evt::BrowserGPUMemoryEvent::OOM:
            TRACE_ALWAYS(("GPU Memory is OOM. \n"));
            if (isAppRunning())
            {
                TRACE_ALWAYS(("Terminating application due to insufficient GPU memory!!!\n"));
                terminateCurrentApp();
            }
            break;
        default:
            break;
    }
}

void OperaEngine::onEglContextLostError(omi::evt::EGLContextLost egl_context_lost)
{
    TRACE_ALWAYS(("EGL context lost event has been received, the reason is %d \n",
       egl_context_lost.reason));
}

void OperaEngine::onRendererProcessDied(omi::evt::RendererProcessDied renderer_process_died)
{
    switch (renderer_process_died.status)
    {
        case omi::evt::RendererProcessDied::UNKNOWN:
            TRACE_ALWAYS(("Renderer Process Status: Unknown\n"));
            break;
        case omi::evt::RendererProcessDied::WAS_KILLED:
            TRACE_ALWAYS(("Renderer Process Status: Killed\n"));
            break;
        case omi::evt::RendererProcessDied::CRASHED:
            TRACE_ALWAYS(("Renderer Process Status: Crashed\n"));
            break;
        default:
            TRACE_ALWAYS(("Unknown status\n"));
            break;
    }
}

void
OperaEngine::setStarturlOpened(bool value)
{
    m_window_manager->setStarturlOpened(value);
}

bool
OperaEngine::isStarturlOpened() const
{
    return m_window_manager->isStarturlOpened();
}

void
OperaEngine::setCookieActiveWindow(bool activate)
{
    if (activate)
    { 
        OperaBrowserUIWindow& window = m_window_manager->getBrowserUIWindow("about:blank");

        m_window_manager->setVisible(window, false);
        m_window_manager->setActive(window, true);
    }
    else
    {
        OperaWindow* browser_ui_win = m_window_manager->getWindowByName(OperaWindow::window_name_browser_ui);
        if (browser_ui_win)
        {
            onTvBrowserExitRequested();
            m_window_manager->setVisible(*browser_ui_win, false);
            stopTvBrowser();
        }
    }
}

OIPF_ComponentType OperaEngine::convertTrackTypeToNebulaType(
                             const omi::common::TrackType track_type)
{
    OIPF_ComponentType result = OIPF_ComponentTypeNone;

    switch(track_type)
    {
        case omi::common::TrackType::TRACK_TYPE_VIDEO:
            result = OIPF_ComponentType::OIPF_ComponentTypeVideo;
            break;
        case omi::common::TrackType::TRACK_TYPE_AUDIO:
            result = OIPF_ComponentType::OIPF_ComponentTypeAudio;
            break;
        case omi::common::TrackType::TRACK_TYPE_SUBTITLE:
            result = OIPF_ComponentType::OIPF_ComponentTypeSubtitle;
            break;
        case omi::common::TrackType::TRACK_TYPE_UNKNOWN:
            break;
        default:
            // A new TrackType has been added by VEWD and should be handled.
            TRACE_WARN(("Given TrackType has no comparative Nebula_ComponentType value!\n"));
            SHOULD_NOT_BE_HERE();
    }

    return result;
}

bool OperaEngine::getComponents(OIPF_ComponentType type,
                                NEBULA_AVComponentList* component_list)
{
    TRACE_INFO(("getComponents()\n"));
    CABOT_ASSERT(m_window_manager);

    const auto active_window = m_window_manager->getActiveWindowForComponents();
    if (!active_window)
    {
        if (component_list)
        {
            TRACE_WARN(("OperaEngine::getComponents() no active window\n"));
            component_list->number_of_current_components = 0;
        }
        return false;
    }
    OperaWindowHandle handle = active_window->handle();
    omi::msg::GetMediaInfo::Result result = post(omi::msg::GetMediaInfo(handle)).get();
    if (!checkResult(result))
    {
        component_list->number_of_current_components = 0;
        return false;
    }

    int index = 0;
    for (const auto& media : result.objects)
    {
        TRACE_INFO(("Media id %d is playing %d\n", media.id, media.playing));
        for (const auto track : media.tracks)
        {
            TRACE_INFO(("Track id= %s type= %s lang= %s active= %d\n",
                        track.id.c_str(),
                        omi::common::ToString(track.track_type).c_str(),
                        track.language.c_str(),
                        track.active));
            if(media.playing && convertTrackTypeToNebulaType(track.track_type) == type)
            {
                component_list->components[index].type = type;
                strncpy(component_list->components[index].id,
                        track.id.c_str(),
                        NEBULA_AVComponent_id_length);
                component_list->components[index].is_active =
                        track.active ? frost_true : frost_false;
                strncpy(component_list->components[index].language_code,
                        track.language.c_str(), NEBULA_LANG_CODE_LEN);
                component_list->components[index].kind = NEBULA_KindNone;
                strncpy(component_list->components[index].label,
                        track.label.c_str(), NEBULA_AVComponent_label_length);
                component_list->components[index].audio_description = track.audio_service_type ? frost_true : frost_false;

                if ((component_list->components[index].kind == NEBULA_KindNone)
                                     && (type == OIPF_ComponentTypeSubtitle))
                {
                    component_list->components[index].kind = NEBULA_KindSubtitles;
                }

                index++;
            }
        }
    }
    component_list->number_of_current_components = index;
    return true;
}

bool OperaEngine::selectComponent(NEBULA_AVComponent const* component)
{
    TRACE_INFO(("selectComponent()\n"));
    CABOT_ASSERT(m_window_manager);

    const auto active_window = m_window_manager->getActiveWindowForComponents();
    if (!active_window)
    {
        return false;
    }
    omi::common::MediaObject::Track media_track;
    OperaWindowHandle handle = active_window->handle();
    if (findCorrespondingTrack(handle, component, media_track))
    {
        setMediaTrack(handle, media_track, true);
        return true;
    }
    return false;
}

bool OperaEngine::deselectComponent(NEBULA_AVComponent const* component)
{
    TRACE_INFO(("deselectComponent()\n"));
    CABOT_ASSERT(m_window_manager);

    const auto active_window = m_window_manager->getActiveWindowForComponents();
    if (!active_window)
    {
        return false;
    }
    omi::common::MediaObject::Track media_track;
    OperaWindowHandle handle = active_window->handle();
    if (findCorrespondingTrack(handle, component, media_track))
    {
        setMediaTrack(handle, media_track, false);
        return true;
    }
    return false;
}

void OperaEngine::setMediaTrack(OperaWindowHandle& handle,
                                const omi::common::MediaObject::Track& track,
                                bool activate)
{
    TRACE_INFO(("%s track with id : %s, type: %s and language: %s AD:%s\n",
                activate ? "Activate" : "Deactivate",
                track.id.c_str(),
                omi::common::ToString(track.track_type).c_str(),
                track.language.c_str(),
#ifndef USING_SDK423
                track.audio_service_type ? "enabled" : "disabled"
#else
                track.audio_description ? "enabled" : "disabled"
#endif
               ));

    if(strlen(track.label.c_str()) != 0)  
    {
         omi::msg::GetMediaInfo::Result result = post(omi::msg::GetMediaInfo(handle)).get();

        for (const auto& media : result.objects)   
        {
             //omi::msg::SwitchMediaObjectTrack::Result result = post(omi::msg::SwitchMediaObjectTrack(handle,media.id,
             //               track.id.c_str(),
             //                track.track_type,
             //                true)).get();
             post(omi::msg::SwitchMediaObjectTrack(handle,media.id,
                             track.id.c_str(),
                             track.track_type,
                             true));
             
         }
    }
    else
    {
        m_component_preferences.window_id = handle;
        if (track.track_type == omi::common::TrackType::TRACK_TYPE_AUDIO)
        {
            m_component_preferences.preferred_audio_languages.clear();
            if (activate)
            {
                m_component_preferences.preferred_audio_languages.push_back(track.language);
            }
        }
        else if (track.track_type == omi::common::TrackType::TRACK_TYPE_SUBTITLE)
        {
            m_component_preferences.preferred_subtitle_languages.clear();
            m_component_preferences.enable_subtitle = false;
            if (activate)
            {
                m_component_preferences.enable_subtitle = true;
                m_component_preferences.preferred_subtitle_languages.push_back(track.language);
            }
        }
        if(activate && track.track_type == omi::common::TrackType::TRACK_TYPE_AUDIO)
        {
            setAudioDescriptionPreference(track.audio_service_type);
        }
        post(m_component_preferences);
    }
}

void OperaEngine::setAudioDescriptionPreference(bool audio_description)
{
    if (audio_description)
    {
        m_component_preferences.audio_selection_option =
                   omi::msg::SetMediaComponentsPreferences::NORMAL_AUDIO_AND_AUDIO_DESCRIPTIONS;
    }
    else
    {
        m_component_preferences.audio_selection_option =
                    omi::msg::SetMediaComponentsPreferences::NORMAL_AUDIO_ONLY;
    }
}

bool OperaEngine::findCorrespondingTrack(OperaWindowHandle handle,
                                         NEBULA_AVComponent const* component,
                                         omi::common::MediaObject::Track& media_track)
{
    TRACE_INFO(("findComponent() track id= %s type= %d lang= %s active= %d\n",
                                                                component->id,
                                                                component->type,
                                                                component->language_code,
                                                                component->is_active));
    bool ret_val = false;
    omi::msg::GetMediaInfo::Result result = post(omi::msg::GetMediaInfo(handle)).get();
    if (!checkResult(result))
    {
        return ret_val;
    }

    for (const auto& media : result.objects)
    {
        TRACE_INFO(("Media id %d is playing %d\n", media.id, media.playing));
        for (const auto track : media.tracks)
        {
            if (media.playing
                    && convertTrackTypeToNebulaType(track.track_type) == component->type)
            {
                if ((strncmp(component->id, track.id.c_str(), NEBULA_AVComponent_id_length)==0)
                    || (strlen(component->id) == 0  && strncmp(component->language_code,
                                                               track.language.c_str(),
                                                               NEBULA_LANG_CODE_LEN)==0)
                        && (strncmp(component->label, track.label.c_str(), NEBULA_AVComponent_label_length)==0)
                        && ((component->audio_description == frost_true) == track.audio_service_type)
                    )
                {
                    TRACE_INFO(("Found matching track\n"));
                    media_track = track;
                    ret_val = true;
                }
            }
        }
    }
    return ret_val;
}

void
OperaEngine::resumeApplicatonCoordinator()
{
    NEBULA_WebBrowserResumeApplicationCoordinator();
}

void
OperaEngine::suspendApplicatonCoordinator(frost_bool activate)
{
    NEBULA_WebBrowserSuspendApplicationCoordinator(activate);
}

void
OperaEngine::resumeMheg()
{
#ifdef INCLUDE_MHEG_ENGINE
    NEBULA_MhegResume();
#endif
}

void
OperaEngine::suspendMheg()
{
#ifdef INCLUDE_MHEG_ENGINE
    NEBULA_MhegSuspend();
#endif
}

void
OperaEngine::sendPlatformMessage(const char* message)
{
#ifdef INCLUDE_HOTELTV
    OperaWindow* menu_win = m_window_manager->getActiveWindowByName(OperaWindow::window_name_csp);

    if (menu_win)
    {
        post(omi::msg::SendPlatformEvent(menu_win->handle(), message));
        TRACE_INFO(("Platform messsage is posted to CSP Window"));
    }
    else
    {
        menu_win = m_window_manager->getActiveWindowByName("SETTINGS_MENU");

        if (menu_win)
        {
            post(omi::msg::SendPlatformEvent(menu_win->handle(), message));
            TRACE_INFO(("Platform messsage is posted to SETTINGS_MENU Window"));
        }
        else
        {
            TRACE_ERROR(("Message could not be sent. No active page is available!\n"));
        }
    }
#else
    OperaWindow* active_window = m_window_manager->getActiveWindow();
    using omi::msg::SendPlatformEvent;

    if (active_window)
    {
        post(omi::msg::SendPlatformEvent(active_window->handle(), message));
        TRACE_INFO(("Platform messsage is posted to active page\n"));
    }
    else
    {
        TRACE_ERROR(("Message could not be sent. No active page is available!\n"));
    }
#endif
}

#if defined (ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
void
OperaEngine::onBroadcastResourceGranted(omi::evt::BroadcastResourceGranted granted_event)
{
    int granted_resource_id = granted_event.resource_id;
    TRACE_INFO(("onBroadcastResourceGranted(): Browser released broadcast resource with id %d\n", granted_resource_id));
    if (m_opera_resource_manager)
    {
        m_opera_resource_manager->handleBroadcastResourceGranted(granted_resource_id);
    }
    TRACE_INFO(("onBroadcastResourceGranted(): Browser released broadcast resource with id %d - DONE\n", granted_resource_id));
}

void
OperaEngine::onBroadcastResourceReleaseRequested(omi::evt::BroadcastResourceReleaseRequested release_request_event)
{
    TRACE_INFO(("onBroadcastResourceReleaseRequested(): Browser requested broadcast resource, Linear IP Service: %d\n", 
        isLinearIPService()));
    if (m_opera_resource_manager)
    {
        m_opera_resource_manager->handleBroadcastResourceReleaseRequested(isLinearIPService());
    }
    release_request_event.Completed();
    TRACE_INFO(("onBroadcastResourceReleaseRequested(): Browser requested broadcast resource - DONE\n"));
}
#endif

#ifdef USE_MEDIA_PLAYER_WINDOW
void
OperaEngine::onMediaPlayerError(omi::evt::MediaPlayerError event)
{
    const auto& media_player = m_window_manager->mediaPlayerWindow();
    CABOT_ASSERT(event.window_id == media_player.handle());
    const auto url = media_player.getCurrentPageUrl();

    TRACE_INFO(("%s(url=%s)\n", __func__, url.c_str()));

    m_window_manager->resetMediaPlayer();

    if (m_media_player_event_publisher)
    {
        m_media_player_event_publisher->notifyPlayerError(url);
    }
}

void
OperaEngine::onMediaPlayerLoadFinished(omi::evt::MediaPlayerLoadFinished event)
{
    const auto& media_player = m_window_manager->mediaPlayerWindow();
    CABOT_ASSERT(event.window_id == media_player.handle());
    const auto url = media_player.getCurrentPageUrl();

    TRACE_INFO(("%s(url=%s)\n", __func__, url.c_str()));

    if (m_media_player_event_publisher)
    {
        m_media_player_event_publisher->notifyUrlLoadingFinished(url);
    }
}

void
OperaEngine::onMediaPlayerPlaybackRevoked(omi::evt::MediaPlayerPlaybackRevoked event)
{
    const auto& media_player = m_window_manager->mediaPlayerWindow();
    CABOT_ASSERT(event.window_id == media_player.handle());
    const auto url = media_player.getCurrentPageUrl();

    TRACE_INFO(("%s(url=%s)\n", __func__, url.c_str()));

    // Not currently used.
}
#endif
#ifndef NOVATEK_PLATFORM
#ifdef USING_SDK423
void
OperaEngine::onWebAudioCreated(omi::evt::WebAudioCreated event)
{
    if (m_opera_resource_manager)
    {
        m_opera_resource_manager->playWebAudio(0, 0, 0, 0, true, false);
    }
}

void
OperaEngine::onWebAudioDeleted(omi::evt::WebAudioDeleted event)
{
    if (m_opera_resource_manager)
    {
        m_opera_resource_manager->playWebAudio(0, 0, 0, 0, false, true);
    }
}
#endif
#endif

void
OperaEngine::showWindowMemoryInformation(OperaWindow* win, std::string name)
{
    if(win)
    {
        omi::msg::GetWindowMemoryInformation::Result win_result = 
                post(omi::msg::GetWindowMemoryInformation(win->handle())).get();
        if (checkResult(win_result))
        {
            TRACE_ALWAYS(("%s window system memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated));
            TRACE_ALWAYS(("%s window generic memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_generic));
            TRACE_ALWAYS(("%s window V8 memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_v8));
            TRACE_ALWAYS(("%s window partition memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_partition_alloc));
            TRACE_ALWAYS(("%s window discardable memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_discardable));
            TRACE_ALWAYS(("%s window oilpan memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_oilpan));
            TRACE_ALWAYS(("%s window GPU transfer buffer memory usage: %" PRIi64 "\n", name.c_str(),
                          win_result.allocated_gpu_transfer_buffer));
        }
    }
}

void
OperaEngine::getWindowMemoryInformation()
{
    using omi::msg::GetBrowserMemoryInformation;

    GetBrowserMemoryInformation::Result result = post(GetBrowserMemoryInformation()).get();
    if (checkResult(result))
    {
        TRACE_ALWAYS(("Current browser system memory usage: %" PRIi64 "\n", result.allocated));
        TRACE_ALWAYS(("Current browser generic memory usage: %" PRIi64 "\n", result.allocated_generic));
        TRACE_ALWAYS(("Current browser V8 memory usage: %" PRIi64 "\n", result.allocated_v8));
        TRACE_ALWAYS(("Current browser partition memory usage: %" PRIi64 "\n", result.allocated_partition_alloc));
        TRACE_ALWAYS(("Current browser discardable memory usage: %" PRIi64 "\n", result.allocated_discardable));
        TRACE_ALWAYS(("Current browser oilpan memory usage: %" PRIi64 "\n", result.allocated_oilpan));
        TRACE_ALWAYS(("Current browser GPU memory usage: %" PRIi64 "\n", result.allocated_gpu));
        TRACE_ALWAYS(("Current browser GPU transfer buffer memory usage: %" PRIi64 "\n", result.allocated_gpu_transfer_buffer));
        TRACE_ALWAYS(("Current browser total memory limit: %" PRIi64 "\n", result.limit));
    }

    OperaWindow* win = m_window_manager->getWindowByName(OperaWindow::window_name_ui);
    showWindowMemoryInformation(win, "UI");

    win = m_window_manager->getWindowByName(OperaWindow::window_name_browser_ui);
    showWindowMemoryInformation(win, "BROWSER UI");

    win = m_window_manager->getWindowByName(OperaWindow::window_name_csp);
    showWindowMemoryInformation(win, "CSP");


    win = m_window_manager->getWindowByName(OperaWindow::window_name_hbbtv);
    showWindowMemoryInformation(win, "HBBTV");

    win = m_window_manager->getWindowByName(OperaWindow::window_name_tv_store);
    showWindowMemoryInformation(win, "TV Store");

    win = m_window_manager->getWindowByName(OperaWindow::window_name_opapp);
    showWindowMemoryInformation(win, "OPAPP");
}

bool
OperaEngine::startAdvertisementBrowser(cabot::String const& url)
{
    CABOT_ASSERT(m_window_manager);

    m_app_creation_started = true;
    m_is_exit_from_tv_browser_requested = false;

    bool result = m_window_manager->startAdvertisementBrowser(url);
    if (result)
    {
        m_advertisement_loaded = true;
    }
    return result;
}

bool
OperaEngine::stopAdvertisementBrowser()
{
    CABOT_ASSERT(m_window_manager);

    m_app_creation_started = false;
    m_window_manager->stopAdvertisementBrowser();
    return true;
}

bool
OperaEngine::startBrowserLauncher(cabot::String const& url)
{
    TRACE_INFO(("startBrowserLauncher() [%s] \n", url.charArray()));
    if (m_window_manager)
    {
        return m_window_manager->startBrowserLauncher(url);
    }
    return false;
}

bool 
OperaEngine::startMediaPlayer(cabot::String const& url, bool const linear_ip_service)
{
    CABOT_ASSERT(m_window_manager);

    TRACE_INFO(("%s() [%s] \n", __func__, url.charArray()));
    if (m_window_manager)
    {
        if (m_window_manager->startMediaPlayer(url))
        {
            m_window_manager->mediaPlayerWindow().setLinearIPService(linear_ip_service);
            return true;
        }
    }
    return false;

}

void
OperaEngine::stopBrowserLauncher()
{
    TRACE_INFO(("stopBrowserLauncher() \n"));
    if (m_window_manager)
    {
        m_window_manager->stopBrowserLauncher();
    }
}

void
OperaEngine::reloadBrowserLauncher()
{
    TRACE_INFO(("reloadBrowserLauncher() \n"));
    if (m_window_manager)
    {
        m_window_manager->reloadBrowserLauncher();
    }
}

bool
OperaEngine::setBrowserLauncherVisible(bool visible)
{
    TRACE_INFO(("setBrowserLauncherVisible() visible: [%d] \n", visible));

    if (m_window_manager)
    {
        m_window_manager->setBrowserLauncherVisible(visible);
        return true;
    }
    return false;
}

bool
OperaEngine::isBrowserLauncherFocused() const
{
    if (m_window_manager)
    {
        return m_window_manager->isBrowserLauncherFocused();
    }
    return false;
}

bool
OperaEngine::isBrowserLauncherVisible() const
{
    if (m_window_manager)
    {
        return m_window_manager->isBrowserLauncherVisible();
    }
    return false;
}

bool
OperaEngine::setFocusToBrowserLauncher(bool focus)
{
    if (m_window_manager)
    {
#ifdef INCLUDE_OPAPP_SUPPORT        
        if (!focus)
        {
            if (m_opapp_active_postponed)
            {
                OperaOpAppWindow& opapp_window = m_window_manager->opAppWindow();
                m_window_manager->setActive(opapp_window, true);
                if (!m_window_manager->isVisible(opapp_window))
                {
                    m_window_manager->setVisible(opapp_window, true);
                }
                m_opapp_active_postponed = false;            
            }
        }
#endif                        
        return m_window_manager->setFocusToBrowserLauncher(focus);
    }
    return false;
}

bool 
OperaEngine::isLinearIPService() const
{
    CABOT_ASSERT(m_window_manager);
    
    return (m_window_manager && 
            (m_window_manager->hbbTVWindow().isLinearIPService() || 
             m_window_manager->mediaPlayerWindow().isLinearIPService() ||
             m_window_manager->cspWindow().isLinearIPService()));
}

bool
OperaEngine::getMediaPlayerPosition(double &x,
                                    double &y,
                                    double &width,
                                    double &height)
{
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }

    CABOT_ASSERT(m_window_manager);

    if (m_window_manager)
    {
        auto& window = m_window_manager->mediaPlayerWindow();
        return window.getVideoPosition(x, y, width, height);
    }

    return false;
}

bool
OperaEngine::setMediaPlayerPosition(double x,
                                    double y,
                                    double width,
                                    double height)
{
	printf("+++++++++++++++ OperaEngine::setMediaPlayerPosition : width: %d, height: %d ++++++++++++++++\n", width, height);
    FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return false;
    }

    CABOT_ASSERT(m_window_manager);

    if (m_window_manager)
    {
        auto& window = m_window_manager->mediaPlayerWindow();
        return window.setVideoPosition(x, y, width, height);
    }

    return false;
}

void 
OperaEngine::setRestartAfterTermination(bool const restart_flag)
{
   FrostReadMutexLock lock(m_initialisation_mutex);

    if (!initialisationCompletedLocked())
    {
        return;
    }
    CABOT_ASSERT(m_window_manager);
    if (m_window_manager)
    {
        TRACE_INFO(("%s (%s)\n",  
            __func__, 
            TraceUtility::toString(restart_flag)));

        m_window_manager->hbbTVWindow().setRestartAfterTermination(restart_flag);
    }
}

#ifdef INCLUDE_HOTELTV
void OperaEngine::sendPlatformMessageInJsonFormat()
{
    if (m_spm_str.isEmpty())
    {
        TRACE_ERROR(("\nPlatform Message is Empty\n"));
        return;
    }

#ifndef WIN32 
    //This format has a meaning in Html application
    //First part of the parameter includes the Type
    //Second part includes the message

    char src[2048];
    memset(src, 0, 2048);
    sprintf(src, m_spm_str.charArray());

    char *next = NULL;
    char *first = strtok_r(src, "<", &next);
    
    cabot::String msg_list;

    //Json format
    //[{"param1":"value1","param2":"value2"}]

    //start SPM with Json format
    msg_list.append("[");
    msg_list.append("{ \"");

    //split message blocks, seperated with comma
    char *part = NULL;
    char *posn = NULL;

    do
    {
        part = strtok_r(first, ">", &posn);

        do
        {
            msg_list.append(part);
            part = strtok_r(NULL, ">", &posn);

            if (part != NULL)
            {
                //add a seperator if the message is not finished yet
                msg_list.append("\": \"");
            }
        }
        while (part != NULL);

        first = strtok_r(NULL, "<", &next);

        if (first != NULL)
        {
            msg_list.append("\", \"");
        }
    }
    while (first != NULL);
    
    //finish SPM with Json format
    msg_list.append("\" }");
    msg_list.append("]");

    sendPlatformMessage(msg_list.charArray());

    TRACE_ALWAYS(("Platform Message Sent To Browser: %s\n", msg_list.charArray()));
    msg_list.clear();
    m_spm_str.clear();
#endif
}

void OperaEngine::addSendPlatformMessageParameter(cabot::String first_param, cabot::String second_param)
{
    if (!m_spm_str.isEmpty())
    {
        //add a comma if this is not the first string in the list
        m_spm_str.append("<");
    }

    m_spm_str.append(first_param);
    m_spm_str.append(">");

    m_spm_str.append(second_param);
}

void OperaEngine::clearSendPlatformMessageParameter()
{
    if (!m_spm_str.isEmpty())
    {
        m_spm_str.clear();
    }
}

void OperaEngine::setHbbAppStartedByUser(bool enable)
{
    m_hbb_app_started_by_user = enable;
}

bool OperaEngine::isHbbAppStartedByUser()
{
    return m_hbb_app_started_by_user;
}
#endif
