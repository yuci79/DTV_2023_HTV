/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaSystemInformationProviderCallbacksIpcExternal class and some associated
 *        functions.
 */

#include "NebulaSystemInformationProviderCallbacksIpcExternal.hpp"
#include "NebulaSystemInformationProviderCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaSystemInformationProviderIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"
#include <stdio.h>
#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaSystemInformationProviderCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers.
RegisteredCallbacks<OnSystemInformationProviderGeolocationDataCallback> geolocation_updated_event_register;
RegisteredCallbacks<OnSystemInformationProviderConsentsDataCallback> consents_updated_event_register;
RegisteredCallbacks<OnCurrentChannelDetailsUpdatedCallback> current_ch_details_updated_event_register;
RegisteredCallbacks<OnChannelListChangeDataUpdatedCallback> cl_change_data_updated_event_register;
RegisteredCallbacks<OnFreeviewPlayDataUpdatedCallback> freeviewplay_data_updated_event_register;
RegisteredCallbacks<OnDetTokenUpdatedCallback> det_token_updated_event_register;
RegisteredCallbacks<OnAITUpdatedCallback> ait_updated_event_register;
RegisteredCallbacks<OnAdvertisingDeviceIDResetCallback> advertising_device_id_reset_callback_register;
RegisteredCallbacks<OnIFAInfoChangeCallback> ifa_info_change_callback_register;
RegisteredCallbacks<OnNTPUpdatedCallback> ntp_updated_callback_register;
RegisteredCallbacks<OnSourceTypeChangedCallback> source_type_changed_callback_register;
RegisteredCallbacks<OnSystemInformationProviderVisibilityChangedCallback> visibility_callback_register;
RegisteredCallbacks<OnAppStateChangedCallback> app_state_changed_callback_register;
RegisteredCallbacks<OnSoftwareUpdateDataChangedCallback> software_update_data_changed_callback_register;
RegisteredCallbacks<OnRCULowBatteryCallback> rcu_low_battery_event_callback_register;
RegisteredCallbacks<OnPowerUpDataCallback> power_up_data_callback_register;
RegisteredCallbacks<OnInternetSpeedTestResultCallback> internet_speed_test_result_callback_register;
RegisteredCallbacks<OnHomeScreenSaverTimeoutChangedCallback> home_screen_saver_timeout_callback_register;
RegisteredCallbacks<OnNativeUIAvailabilityDataUpdatedCallback> native_ui_availability_data_updated_callback_register;
RegisteredCallbacks<OnFVPTCAcceptedCallback> fvp_tc_accepted_callback_register;

// Proxy external callbacks.
void
proxyOnGeolocationUpdated(MP_NEBULA_GeolocationInfo geolocation_info, RegisteredCallbacksConfig::TokenType token)
{
    OnSystemInformationProviderGeolocationDataCallback callback;
    if (geolocation_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&geolocation_info, user_data);
        }
    }
} 

void
proxyOnConsentsUpdated(MP_NEBULA_ConsentDictionary consents_data, RegisteredCallbacksConfig::TokenType token)
{
    OnSystemInformationProviderConsentsDataCallback callback;
    if (consents_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&consents_data, user_data);
        }
    }
}

void
proxyOnCurrentChannelDetailsUpdated(MP_NEBULA_CurrentChannelDetails current_ch_details_data, RegisteredCallbacksConfig::TokenType token)
{
    OnCurrentChannelDetailsUpdatedCallback callback;
    if (current_ch_details_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&current_ch_details_data, user_data);
        }
    }
}

void
proxyOnChannelListChangeDataUpdated(MP_NEBULA_ChannelListChangeData cl_change_data, RegisteredCallbacksConfig::TokenType token)
{
    OnChannelListChangeDataUpdatedCallback callback;
    if (cl_change_data_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&cl_change_data, user_data);
        }
    }
}

void
proxyOnFreeviewPlayDataUpdated(MP_NEBULA_FreeviewPlayData freeviewplay_data, RegisteredCallbacksConfig::TokenType token)
{
    OnFreeviewPlayDataUpdatedCallback callback;
    if (freeviewplay_data_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&freeviewplay_data, user_data);
        }
    }
}

void
proxyOnDetTokenUpdated(MP_NEBULA_DetToken det_token, RegisteredCallbacksConfig::TokenType token)
{
    OnDetTokenUpdatedCallback callback;
    if (det_token_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&det_token, user_data);
        }
    }
}

void 
proxyOnAITUpdated(MP_NEBULA_AITDeepLinkInfo ait_deeplink_info, RegisteredCallbacksConfig::TokenType token)
{
    OnAITUpdatedCallback callback;
    if (ait_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&ait_deeplink_info, user_data);
        }
    }
}

void
proxyOnResetAdvertisingDeviceID(frost_bool reset, RegisteredCallbacksConfig::TokenType token)
{
    OnAdvertisingDeviceIDResetCallback callback;
    if (advertising_device_id_reset_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(reset, user_data);
        }
    }
}

void
proxyOnIFAInfoChange(frost_bool changed, RegisteredCallbacksConfig::TokenType token)
{
    OnIFAInfoChangeCallback callback;
    if (ifa_info_change_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(changed, user_data);
        }
    }
}

void
proxyOnNTPUpdated(frost_bool updated, RegisteredCallbacksConfig::TokenType token)
{
    OnNTPUpdatedCallback callback;
    if (ntp_updated_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(updated, user_data);
        }
    }
}

void
proxyOnSourceTypeChanged(MP_NEBULA_SourceChangeInfo source_change_info,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnSourceTypeChangedCallback callback;
    if (source_type_changed_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&source_change_info, user_data);
        }
    }
}

void
proxyOnVisibilityChanged(frost_bool visible, frost_int reason, RegisteredCallbacksConfig::TokenType token)
{
    OnSystemInformationProviderVisibilityChangedCallback callback;
    if (visibility_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(visible, reason, user_data);
        }
    }
}

void proxyOnAppStateChanged(
                  std::string app_url,
                  frost_int app_type,
                  frost_int app_status,
                  std::string app_id,
                  RegisteredCallbacksConfig::TokenType token)
{
    OnAppStateChangedCallback callback;
    if (app_state_changed_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(app_url.c_str(), app_type, app_status, app_id.c_str(), user_data);
        }
    }
}

void
proxyOnSoftwareUpdateDataChanged(MP_NEBULA_SoftwareUpdateData software_update_data,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnSoftwareUpdateDataChangedCallback callback;
    if (software_update_data_changed_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&software_update_data, user_data);
        }
    }
}

void
proxyOnRegisterRCULowBatteryEvent(MP_NEBULA_RCULowBatteryEvent rcu_low_battery_event,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnRCULowBatteryCallback callback;
    if (rcu_low_battery_event_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&rcu_low_battery_event, user_data);
        }
    }
}

void
proxyOnPowerUpData(MP_NEBULA_PowerUpData power_up_data,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnPowerUpDataCallback callback;
    if (power_up_data_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&power_up_data, user_data);
        }
    }
}

void
proxyOnInternetSpeedTestResult(MP_NEBULA_InternetSpeedTestResult speed_test_result,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnInternetSpeedTestResultCallback callback;
    if (internet_speed_test_result_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&speed_test_result, user_data);
        }
    }
}

void
proxyOnHomeScreenSaverTimeoutChanged(frost_int screen_saver_timeout,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnHomeScreenSaverTimeoutChangedCallback callback;
    if (home_screen_saver_timeout_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(screen_saver_timeout, user_data);
        }
    }
}

void
proxyOnFVPTCAccepted(frost_bool fvp_tc_accepted,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnFVPTCAcceptedCallback callback;
    if (fvp_tc_accepted_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(fvp_tc_accepted, user_data);
        }
    }
}

void
proxyOnNativeUIAvailabilityDataUpdated(MP_NEBULA_NativeUIAvailabilityData availability_data,
                    RegisteredCallbacksConfig::TokenType token)
{
    OnNativeUIAvailabilityDataUpdatedCallback callback;
    if (native_ui_availability_data_updated_callback_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;

        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&availability_data, user_data);
        }
    }
}
}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool
NEBULA_SystemInformationProviderSetGeoLocationEventCallback(
                                        NEBULA_SystemInformationProviderGeolocationDataCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return geolocation_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_SystemInformationProviderSetGeoLocationEventCallback));
}

frost_bool
NEBULA_SystemInformationProviderSetConsentsUpdatedCallback(
                                        NEBULA_SystemInformationProviderConsentsDataCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return consents_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_SystemInformationProviderSetConsentsUpdatedCallback));
}

frost_bool
NEBULA_RegisterCurrentChannelDetailsUpdatedCallback(
                                        NEBULA_CurrentChannelDetailsUpdatedCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return current_ch_details_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterCurrentChannelDetailsUpdatedCallback));
}  

frost_bool
NEBULA_RegisterChannelListChangeDataUpdatedCallback(
                                        NEBULA_ChannelListChangeDataUpdatedCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return cl_change_data_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterChannelListChangeDataUpdatedCallback));
}  

frost_bool
NEBULA_RegisterFreeviewPlayDataUpdatedCallback(
                                        NEBULA_FreeviewPlayDataUpdatedCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return freeviewplay_data_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterFreeviewPlayDataUpdatedCallback));
}

frost_bool
NEBULA_RegisterDetTokenUpdatedCallback(
                                        NEBULA_DetTokenUpdatedCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return det_token_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterDetTokenUpdatedCallback));
}

frost_bool
NEBULA_RegisterAITUpdatedCallback(
                                        NEBULA_AITUpdatedCallback* callback, void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                callback, user_data));

    return ait_updated_event_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterAITUpdatedCallback));
}                               

frost_bool
NEBULA_RegisterAdvertisingDeviceIDResetCallback(
        NEBULA_AdvertisingDeviceIDResetCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return advertising_device_id_reset_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterAdvertisingDeviceIDResetCallback));
}

frost_bool
NEBULA_RegisterIFAInfoChangeCallback(
        NEBULA_IFAInfoChangeCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return ifa_info_change_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterIFAInfoChangeCallback));
}

frost_bool
NEBULA_RegisterNTPUpdatedCallback(
        NEBULA_NTPUpdatedCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return ntp_updated_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterNTPUpdatedCallback));
}

frost_bool
NEBULA_RegisterSourceTypeChangedCallback(
        NEBULA_SourceTypeChangedCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return source_type_changed_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterSourceTypeChangedCallback));
}

frost_bool
NEBULA_LauncherRegisterVisibilityCallback(
                  NEBULA_HomeLauncherVisibilityCallback* callback,
                  void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return visibility_callback_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_LauncherRegisterVisibilityCallback));
}

frost_bool
NEBULA_RegisterAppStateChangedCallback(
                  NEBULA_AppStateChangedCallback* callback,
                  void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return app_state_changed_callback_register.registerCallback(
                {callback, user_data},
                IPC_NAME(NEBULA_RegisterAppStateChangedCallback));
}

frost_bool
NEBULA_RegisterSoftwareUpdateDataChangedCallback(
        NEBULA_SoftwareUpdateDataChangedCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return software_update_data_changed_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterSoftwareUpdateDataChangedCallback));
}

frost_bool
NEBULA_RegisterRCULowBatteryCallback(
        NEBULA_RCULowBatteryCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return rcu_low_battery_event_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterRCULowBatteryCallback));
}

frost_bool
NEBULA_RegisterPowerUpDataCallback(
        NEBULA_PowerUpDataCallback* callback,
        void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return power_up_data_callback_register.registerCallback(
            {callback, user_data},
            IPC_NAME(NEBULA_RegisterPowerUpDataCallback));
}

frost_bool
NEBULA_RegisterInternetSpeedTestResultCallback(
                  NEBULA_InternetSpeedTestResultCallback* callback,
                  void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return internet_speed_test_result_callback_register.registerCallback(
                  {callback, user_data},
                  IPC_NAME(NEBULA_RegisterInternetSpeedTestResultCallback));
}

frost_bool
NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback(
                      NEBULA_HomeScreenSaverTimeoutChangedCallback* callback,
                      void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return home_screen_saver_timeout_callback_register.registerCallback(
                  {callback, user_data},
                  IPC_NAME(NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback));
}

frost_bool
NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback(
                      NEBULA_NativeUIAvailabilityDataUpdatedCallback* callback,
                      void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return native_ui_availability_data_updated_callback_register.registerCallback(
                  {callback, user_data},
                  IPC_NAME(NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback));
}

frost_bool
NEBULA_RegisterFVPTCAcceptedCallback(
                      NEBULA_FVPTCAcceptedCallback* callback,
                      void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__,
                callback, user_data));

    return fvp_tc_accepted_callback_register.registerCallback(
                  {callback, user_data},
                  IPC_NAME(NEBULA_RegisterFVPTCAcceptedCallback));
}

#ifdef __cplusplus
}
#endif

void
NebulaSystemInformationProviderCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGeolocationDataCallback), proxyOnGeolocationUpdated);
    server.bind(IPC_NAME(NEBULA_SystemInformationProviderConsentsDataCallback), proxyOnConsentsUpdated);
    server.bind(IPC_NAME(NEBULA_CurrentChannelDetailsUpdatedCallback), proxyOnCurrentChannelDetailsUpdated);
    server.bind(IPC_NAME(NEBULA_ChannelListChangeDataUpdatedCallback), proxyOnChannelListChangeDataUpdated);
    server.bind(IPC_NAME(NEBULA_FreeviewPlayDataUpdatedCallback), proxyOnFreeviewPlayDataUpdated);
    server.bind(IPC_NAME(NEBULA_DetTokenUpdatedCallback), proxyOnDetTokenUpdated);    
    server.bind(IPC_NAME(NEBULA_AITUpdatedCallback), proxyOnAITUpdated);
    server.bind(IPC_NAME(NEBULA_AdvertisingDeviceIDResetCallback), proxyOnResetAdvertisingDeviceID);
    server.bind(IPC_NAME(NEBULA_IFAInfoChangeCallback), proxyOnIFAInfoChange);    
    server.bind(IPC_NAME(NEBULA_NTPUpdatedCallback), proxyOnNTPUpdated);    
    server.bind(IPC_NAME(NEBULA_SourceTypeChangedCallback), proxyOnSourceTypeChanged);
    server.bind(IPC_NAME(NEBULA_HomeLauncherVisibilityCallback), proxyOnVisibilityChanged);
    server.bind(IPC_NAME(NEBULA_AppStateChangedCallback), proxyOnAppStateChanged);
    server.bind(IPC_NAME(NEBULA_SoftwareUpdateDataChangedCallback), proxyOnSoftwareUpdateDataChanged);
    server.bind(IPC_NAME(NEBULA_RCULowBatteryCallback), proxyOnRegisterRCULowBatteryEvent);
    server.bind(IPC_NAME(NEBULA_PowerUpDataCallback), proxyOnPowerUpData);
    server.bind(IPC_NAME(NEBULA_InternetSpeedTestResultCallback), proxyOnInternetSpeedTestResult);
    server.bind(IPC_NAME(NEBULA_HomeScreenSaverTimeoutChangedCallback), proxyOnHomeScreenSaverTimeoutChanged);
    server.bind(IPC_NAME(NEBULA_NativeUIAvailabilityDataUpdatedCallback), proxyOnNativeUIAvailabilityDataUpdated);
    server.bind(IPC_NAME(NEBULA_FVPTCAcceptedCallback), proxyOnFVPTCAccepted);
}
