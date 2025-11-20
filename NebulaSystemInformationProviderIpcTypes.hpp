/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_system_information_provider.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASYSTEMINFORMATIONPROVIDERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASYSTEMINFORMATIONPROVIDERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

using StringPair = std::pair<std::string, std::string>;

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxBPC);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxRefreshRate);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsUhdHevcSupported);

DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsCableSupportEnabled);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsSatelliteSupportEnabled);

DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsDVBIServiceDiscoveryEnabled);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsFreeviewPlayEnabled);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetCIPlusEnabled);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetCicamPlayerEnabled);

DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetVendorName);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetModelName);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetDeviceFamilyName);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetSoftwareVersion);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetHardwareVersion);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetSerialNumber);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetSecureStore);  
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetDeviceUpTime);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetActivityUpTime);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderSetGeoLocationEventCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGeolocationDataCallback);   
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderSetConsentsUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderConsentsDataCallback); 
DECLARE_IPC_NAME(NEBULA_RegisterCurrentChannelDetailsUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_CurrentChannelDetailsUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterFreeviewPlayDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_FreeviewPlayDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterDetTokenUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_DetTokenUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterChannelListChangeDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_ChannelListChangeDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterAITUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_AITUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetManufactureTime);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayType);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayWidth);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayHeight);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetHomeScreenSaverTimeout);
DECLARE_IPC_NAME(NEBULA_RegisterIFAInfoChangeCallback);
DECLARE_IPC_NAME(NEBULA_IFAInfoChangeCallback);
DECLARE_IPC_NAME(NEBULA_RegisterAdvertisingDeviceIDResetCallback);
DECLARE_IPC_NAME(NEBULA_AdvertisingDeviceIDResetCallback);
DECLARE_IPC_NAME(NEBULA_RegisterNTPUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_NTPUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterSourceTypeChangedCallback);
DECLARE_IPC_NAME(NEBULA_SourceTypeChangedCallback);
DECLARE_IPC_NAME(NEBULA_LauncherRegisterVisibilityCallback);
DECLARE_IPC_NAME(NEBULA_HomeLauncherVisibilityCallback);
DECLARE_IPC_NAME(NEBULA_RegisterAppStateChangedCallback);
DECLARE_IPC_NAME(NEBULA_AppStateChangedCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetLoggingRegistrationTime);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetIFA); 
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetLimitAdTracking); 
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetDeviceSessionID);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetActivitySessionID);
DECLARE_IPC_NAME(NEBULA_RegisterSoftwareUpdateDataChangedCallback);
DECLARE_IPC_NAME(NEBULA_SoftwareUpdateDataChangedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterRCULowBatteryCallback);
DECLARE_IPC_NAME(NEBULA_RCULowBatteryCallback);
DECLARE_IPC_NAME(NEBULA_RegisterPowerUpDataCallback);
DECLARE_IPC_NAME(NEBULA_PowerUpDataCallback);
DECLARE_IPC_NAME(NEBULA_RegisterInternetSpeedTestResultCallback);
DECLARE_IPC_NAME(NEBULA_InternetSpeedTestResultCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderSetAITUrl);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderResetIFA);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderUpdateNTP);
DECLARE_IPC_NAME(NEBULA_RegisterHomeScreenSaverTimeoutChangedCallback);
DECLARE_IPC_NAME(NEBULA_HomeScreenSaverTimeoutChangedCallback);
DECLARE_IPC_NAME(NEBULA_RegisterNativeUIAvailabilityDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_NativeUIAvailabilityDataUpdatedCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetNativeUIAvailabilityData);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderGetProfile);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsBrowserLauncherVisible);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderSetDeviceLoggingID);
DECLARE_IPC_NAME(NEBULA_RegisterFVPTCAcceptedCallback);
DECLARE_IPC_NAME(NEBULA_FVPTCAcceptedCallback);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderIsFVPTCAccepted);
DECLARE_IPC_NAME(NEBULA_SystemInformationProviderUpdateAppsList);

struct MP_NEBULA_GeolocationInfo : public NEBULA_GeolocationInfo
{
    MP_NEBULA_GeolocationInfo() = default;

    MP_NEBULA_GeolocationInfo(NEBULA_GeolocationInfo& other)
        : NEBULA_GeolocationInfo(other)
    {
    }

    MSGPACK_DEFINE(country,
                   city,
                   region);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_GeolocationInfo,
                   "",
                   "",
                   "");
};

struct MP_NEBULA_ConsentDictionary : public NEBULA_ConsentDictionary
{
    MP_NEBULA_ConsentDictionary() = default;

    MP_NEBULA_ConsentDictionary(NEBULA_ConsentDictionary& other)
        : NEBULA_ConsentDictionary(other)
    {
    }

    MSGPACK_DEFINE(voice_commands,
                     personalization_viewing,
                     personalized_advertising,
                     viewership_information_services,
                     hbbtv,
                     automatic_software_updates_and_modifications,
                     internet_connection_policy,
                     data_transfer,
                     gdpr,
                     consent_timestamp,
                     consent_version);

    MEMBER_CHECKED_NEBULA_TYPE(
                     NEBULA_ConsentDictionary,
                     voice_commands,
                     personalization_viewing,
                     personalized_advertising,
                     viewership_information_services,
                     hbbtv,
                     automatic_software_updates_and_modifications,
                     internet_connection_policy,
                     data_transfer,
                     gdpr,
                     consent_timestamp,
                     "");
};

struct MP_NEBULA_CurrentChannelDetails : public NEBULA_CurrentChannelDetails
{
    MP_NEBULA_CurrentChannelDetails() = default;

    MP_NEBULA_CurrentChannelDetails(NEBULA_CurrentChannelDetails& other)
        : NEBULA_CurrentChannelDetails(other)
    {
    }

    MEMBER_CHECKED_MSGPACK_DEFINE(
                     NEBULA_CurrentChannelDetails,
                     presenting,
                     channel_change_time,
                     onid,
                     tsid,
                     sid);
};

struct MP_NEBULA_ChannelListChangeData : public NEBULA_ChannelListChangeData
{
    MP_NEBULA_ChannelListChangeData() = default;

    MP_NEBULA_ChannelListChangeData(NEBULA_ChannelListChangeData& other)
        : NEBULA_ChannelListChangeData(other)
    {
    }

    MSGPACK_DEFINE(timestamp);
};

struct MP_NEBULA_FreeviewPlayData : public NEBULA_FreeviewPlayData
{
    MP_NEBULA_FreeviewPlayData() = default;

    MP_NEBULA_FreeviewPlayData(NEBULA_FreeviewPlayData& other)
        : NEBULA_FreeviewPlayData(other)
    {
    }

    MEMBER_CHECKED_MSGPACK_DEFINE(
                     NEBULA_FreeviewPlayData,
                     ip_virtual_network_id,
                     ondemand_vnid,
                     original_network_id);
}; 

struct MP_NEBULA_DetToken : public NEBULA_DetToken
{
    MP_NEBULA_DetToken() = default;

    MP_NEBULA_DetToken(NEBULA_DetToken& other)
        : NEBULA_DetToken(other)
    {
    }

    MSGPACK_DEFINE(det);
}; 

struct MP_NEBULA_AITDeepLinkInfo : public NEBULA_AITDeepLinkInfo
{
    MP_NEBULA_AITDeepLinkInfo() = default;

    MP_NEBULA_AITDeepLinkInfo(NEBULA_AITDeepLinkInfo& other)
        : NEBULA_AITDeepLinkInfo(other)
    {
    }

    MSGPACK_DEFINE(ait_url,
                     launch_location,
                     ait_type,
                     deeplink,
                     error,
                     expiration_timestamp);

    MEMBER_CHECKED_NEBULA_TYPE(
                     NEBULA_AITDeepLinkInfo,
                     "",
                     "",
                     ait_type,
                     "",
                     error,
                     expiration_timestamp);
}; 

struct MP_NEBULA_SourceChangeInfo: public NEBULA_SourceChangeInfo
{
    MP_NEBULA_SourceChangeInfo() = default;

    MP_NEBULA_SourceChangeInfo(NEBULA_SourceChangeInfo& other)
        : NEBULA_SourceChangeInfo(other)
    {
    }

    MSGPACK_DEFINE(source_type,
                     source_name,
                     port_name,
                     timestamp);

    MEMBER_CHECKED_NEBULA_TYPE(
                     NEBULA_SourceChangeInfo,
                     source_type,
                     "",
                     "",
                     timestamp);
};

struct MP_NEBULA_SoftwareUpdateData: public NEBULA_SoftwareUpdateData
{
    MP_NEBULA_SoftwareUpdateData() = default;

    MP_NEBULA_SoftwareUpdateData(NEBULA_SoftwareUpdateData& other)
        : NEBULA_SoftwareUpdateData(other)
    {
    }

    MSGPACK_DEFINE(http_update_type,
                 current_sw_version,
                 new_sw_version,
                 sw_auto_update_status,
                 usb_update_count,
                 http_update_count,
                 side_load_update_count,
                 timestamp);

    MEMBER_CHECKED_NEBULA_TYPE(
                 NEBULA_SoftwareUpdateData,
                 http_update_type,
                 "",
                 "",
                 sw_auto_update_status,
                 usb_update_count,
                 http_update_count,
                 side_load_update_count,
                 timestamp);
};

struct MP_NEBULA_RCULowBatteryEvent: public NEBULA_RCULowBatteryEvent
{
    MP_NEBULA_RCULowBatteryEvent() = default;

    MP_NEBULA_RCULowBatteryEvent(NEBULA_RCULowBatteryEvent& other)
        : NEBULA_RCULowBatteryEvent(other)
    {
    }

    MSGPACK_DEFINE(timestamp);

    MEMBER_CHECKED_NEBULA_TYPE(
                 NEBULA_RCULowBatteryEvent,
                 timestamp);
};

struct MP_NEBULA_PowerUpData: public NEBULA_PowerUpData
{
    MP_NEBULA_PowerUpData() = default;

    MP_NEBULA_PowerUpData(NEBULA_PowerUpData& other)
        : NEBULA_PowerUpData(other)
    {
    }

    MSGPACK_DEFINE(power_up_reason,
                    lifetime_mins,
                    timestamp);

    MEMBER_CHECKED_NEBULA_TYPE(
                 NEBULA_PowerUpData,
                 power_up_reason,
                 lifetime_mins,
                 timestamp);
};

struct MP_NEBULA_InternetSpeedTestResult : public NEBULA_InternetSpeedTestResult
{
    MP_NEBULA_InternetSpeedTestResult() = default;

    MP_NEBULA_InternetSpeedTestResult(NEBULA_InternetSpeedTestResult& other)
        : NEBULA_InternetSpeedTestResult(other)
    {
    }

    MEMBER_CHECKED_MSGPACK_DEFINE(
                   NEBULA_InternetSpeedTestResult,
                   network_speed_mbps,
                   speed_test_reason,
                   device_type,
                   timestamp);
};

struct MP_NEBULA_NativeUIAvailabilityData : public NEBULA_NativeUIAvailabilityData
{
    MP_NEBULA_NativeUIAvailabilityData() = default;

    MP_NEBULA_NativeUIAvailabilityData(NEBULA_NativeUIAvailabilityData& other)
        : NEBULA_NativeUIAvailabilityData(other)
    {
    }

    MEMBER_CHECKED_MSGPACK_DEFINE(
                   NEBULA_NativeUIAvailabilityData,
                   guide,
                   sources,
                   channel_list,
                   recordings,
                   timers,
                   settings,
                   network_settings,
                   software_upgrade);
};

#endif
