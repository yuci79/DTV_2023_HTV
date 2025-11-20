/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaSystemInformationProviderCallbacksIpcAurora class.
 */

#include "NebulaSystemInformationProviderCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include "nebula_src/adaptation_layer/ipc/NebulaSystemInformationProviderIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>
#include <stdio.h>
TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaSystemInformationProviderCallbacksIpcAurora");

namespace
{
extern "C"
void proxyOnGeolocationUpdated(NEBULA_GeolocationInfo* geolocation_info, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_GeolocationInfo mp_geolocation_info(*geolocation_info);    
    const bool ok = client.call(IPC_NAME(NEBULA_SystemInformationProviderGeolocationDataCallback), 
                                mp_geolocation_info,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {    
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnConsentsUpdated(NEBULA_ConsentDictionary* consents_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_ConsentDictionary mp_consents_data(*consents_data);    
    const bool ok = client.call(IPC_NAME(NEBULA_SystemInformationProviderConsentsDataCallback), 
                                mp_consents_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {    
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnCurrentChannelDetailsUpdated(NEBULA_CurrentChannelDetails* current_ch_details_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_CurrentChannelDetails mp_current_ch_details_data(*current_ch_details_data);    
    const bool ok = client.call(IPC_NAME(NEBULA_CurrentChannelDetailsUpdatedCallback), 
                                mp_current_ch_details_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnChannelListChangeDataUpdated(NEBULA_ChannelListChangeData* cl_change_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_ChannelListChangeData mp_cl_change_data(*cl_change_data);
    const bool ok = client.call(IPC_NAME(NEBULA_ChannelListChangeDataUpdatedCallback),
                                mp_cl_change_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnFreeviewPlayDataUpdated(NEBULA_FreeviewPlayData* freeviewplay_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_FreeviewPlayData mp_freeviewplay_data(*freeviewplay_data);
    const bool ok = client.call(IPC_NAME(NEBULA_FreeviewPlayDataUpdatedCallback), 
                                mp_freeviewplay_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnDetTokenUpdated(NEBULA_DetToken* det_token, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_DetToken mp_det_token(*det_token);
    const bool ok = client.call(IPC_NAME(NEBULA_DetTokenUpdatedCallback),
                                mp_det_token,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnAITUpdated(NEBULA_AITDeepLinkInfo* ait_deeplink_info, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_AITDeepLinkInfo mp_ait_deeplink_info(*ait_deeplink_info);
    const bool ok = client.call(IPC_NAME(NEBULA_AITUpdatedCallback), 
                                mp_ait_deeplink_info,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnResetAdvertisingDeviceID(frost_bool reset, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_AdvertisingDeviceIDResetCallback),
                                reset,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnIFAInfoChange(frost_bool changed, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_IFAInfoChangeCallback),
                                changed,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnNTPUpdated(frost_bool updated, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_NTPUpdatedCallback),
                                updated,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnSourceTypeChanged(NEBULA_SourceChangeInfo* source_info, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_SourceChangeInfo mp_source_info(*source_info);
    const bool ok = client.call(IPC_NAME(NEBULA_SourceTypeChangedCallback),
                                mp_source_info,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnVisibilityChanged(frost_bool visible, frost_int reason, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_HomeLauncherVisibilityCallback),
                                visible,
                                reason,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnAppStateChanged(
                  const char* app_url,
                  frost_int app_type,
                  frost_int app_status,
                  const char* app_id,
                  void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    std::string app_url_string = app_url;
    std::string app_id_string = app_id;
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_AppStateChangedCallback),
                                app_url_string,
                                app_type,
                                app_status,
                                app_id_string,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnSoftwareUpdateDataChanged(NEBULA_SoftwareUpdateData* sw_update_data,
                                    void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_SoftwareUpdateData mp_sw_update_data(*sw_update_data);
    const bool ok = client.call(IPC_NAME(NEBULA_SoftwareUpdateDataChangedCallback),
                                mp_sw_update_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnRegisterRCULowBatteryEvent(NEBULA_RCULowBatteryEvent* rcu_low_battery_event,
                                    void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_RCULowBatteryEvent mp_rcu_low_battery_event(*rcu_low_battery_event);
    const bool ok = client.call(IPC_NAME(NEBULA_RCULowBatteryCallback),
                                mp_rcu_low_battery_event,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnPowerUpData(NEBULA_PowerUpData* power_up_data,
                                    void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_PowerUpData mp_power_up_data(*power_up_data);
    const bool ok = client.call(IPC_NAME(NEBULA_PowerUpDataCallback),
                                mp_power_up_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnInternetSpeedTestResult(NEBULA_InternetSpeedTestResult* speed_test_result, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_InternetSpeedTestResult mp_speed_test_result(*speed_test_result);
    const bool ok = client.call(IPC_NAME(NEBULA_InternetSpeedTestResultCallback),
                                mp_speed_test_result,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnHomeScreenSaverTimeoutChanged(frost_int screen_saver_timeout, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_HomeScreenSaverTimeoutChangedCallback),
                                screen_saver_timeout,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnNativeUIAvailabilityDataUpdated(NEBULA_NativeUIAvailabilityData* availability_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const MP_NEBULA_NativeUIAvailabilityData mp_availability_data(*availability_data);
    const bool ok = client.call(IPC_NAME(NEBULA_NativeUIAvailabilityDataUpdatedCallback),
                                mp_availability_data,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnFVPTCAccepted(frost_bool accepted, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_FVPTCAcceptedCallback),
                                accepted,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}
}

void
NebulaSystemInformationProviderCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SystemInformationProviderSetGeoLocationEventCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_SystemInformationProviderSetGeoLocationEventCallback(
                proxyOnGeolocationUpdated,
                RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderSetConsentsUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_SystemInformationProviderSetConsentsUpdatedCallback(
                  proxyOnConsentsUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterCurrentChannelDetailsUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterCurrentChannelDetailsUpdatedCallback(
                  proxyOnCurrentChannelDetailsUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });  

    server.bind(IPC_NAME(NEBULA_RegisterChannelListChangeDataUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterChannelListChangeDataUpdatedCallback(
                  proxyOnChannelListChangeDataUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterFreeviewPlayDataUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterFreeviewPlayDataUpdatedCallback(
                  proxyOnFreeviewPlayDataUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    }); 
    
    server.bind(IPC_NAME(NEBULA_RegisterDetTokenUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterDetTokenUpdatedCallback(
                  proxyOnDetTokenUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    }); 

    server.bind(IPC_NAME(NEBULA_RegisterAITUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterAITUpdatedCallback(
                  proxyOnAITUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });  

    server.bind(IPC_NAME(NEBULA_RegisterAdvertisingDeviceIDResetCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterAdvertisingDeviceIDResetCallback(
                  proxyOnResetAdvertisingDeviceID,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterIFAInfoChangeCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterIFAInfoChangeCallback(
                  proxyOnIFAInfoChange,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterNTPUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterNTPUpdatedCallback(
                  proxyOnNTPUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });    

    server.bind(IPC_NAME(NEBULA_RegisterSourceTypeChangedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterSourceTypeChangedCallback(
                  proxyOnSourceTypeChanged,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_LauncherRegisterVisibilityCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_LauncherRegisterVisibilityCallback(
                  proxyOnVisibilityChanged,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterAppStateChangedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterAppStateChangedCallback(
                  proxyOnAppStateChanged,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterSoftwareUpdateDataChangedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterSoftwareUpdateDataChangedCallback(
                  proxyOnSoftwareUpdateDataChanged,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterRCULowBatteryCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterRCULowBatteryCallback(
                  proxyOnRegisterRCULowBatteryEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterPowerUpDataCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterPowerUpDataCallback(
                  proxyOnPowerUpData,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterInternetSpeedTestResultCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterInternetSpeedTestResultCallback(
                  proxyOnInternetSpeedTestResult,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback(
                  proxyOnHomeScreenSaverTimeoutChanged,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback(
                  proxyOnNativeUIAvailabilityDataUpdated,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterFVPTCAcceptedCallback),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterFVPTCAcceptedCallback(
                  proxyOnFVPTCAccepted,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

}

void
NebulaSystemInformationProviderCallbacksIpcAurora::reset()
{
    NEBULA_SystemInformationProviderSetGeoLocationEventCallback(nullptr, nullptr);
    NEBULA_SystemInformationProviderSetConsentsUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterCurrentChannelDetailsUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterChannelListChangeDataUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterFreeviewPlayDataUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterDetTokenUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterAITUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterAdvertisingDeviceIDResetCallback(nullptr, nullptr);
    NEBULA_RegisterIFAInfoChangeCallback(nullptr, nullptr);
    NEBULA_RegisterNTPUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterSourceTypeChangedCallback(nullptr, nullptr);
    NEBULA_LauncherRegisterVisibilityCallback(nullptr, nullptr);
    NEBULA_RegisterAppStateChangedCallback(nullptr, nullptr);
    NEBULA_RegisterSoftwareUpdateDataChangedCallback(nullptr, nullptr);
    NEBULA_RegisterRCULowBatteryCallback(nullptr, nullptr);
    NEBULA_RegisterPowerUpDataCallback(nullptr, nullptr);
    NEBULA_RegisterInternetSpeedTestResultCallback(nullptr, nullptr);
    NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback(nullptr, nullptr);
    NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback(nullptr, nullptr);
    NEBULA_RegisterFVPTCAcceptedCallback(nullptr, nullptr);
}
