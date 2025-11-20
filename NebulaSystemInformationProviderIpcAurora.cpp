/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaSystemInformationProviderIpcAurora class.
 */

#include "NebulaSystemInformationProviderIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaSystemInformationProviderIpcTypes.hpp"

#include <rpc/server.h>

#include <vector>

void
NebulaSystemInformationProviderIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_system_information_provider 'C' functions.

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxBPC),
                &NEBULA_SystemInformationProviderGetPanelMaxBPC);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxRefreshRate),
                &NEBULA_SystemInformationProviderGetPanelMaxRefreshRate);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsUhdHevcSupported),
                &NEBULA_SystemInformationProviderIsUhdHevcSupported);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsCableSupportEnabled),
                &NEBULA_SystemInformationProviderIsCableSupportEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsSatelliteSupportEnabled),
                &NEBULA_SystemInformationProviderIsSatelliteSupportEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsDVBIServiceDiscoveryEnabled),
                &NEBULA_SystemInformationProviderIsDVBIServiceDiscoveryEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsFreeviewPlayEnabled),
                &NEBULA_SystemInformationProviderIsFreeviewPlayEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetCIPlusEnabled),
                &NEBULA_SystemInformationProviderGetCIPlusEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetCicamPlayerEnabled),
                &NEBULA_SystemInformationProviderGetCicamPlayerEnabled);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetVendorName), []() {
        NebulaIpcType<frost_bool, std::string> type;
        char vendor_name_str[128] = {0};
        type.return_value = NEBULA_SystemInformationProviderGetVendorName(vendor_name_str);
        if (type.return_value == frost_true)
        {
            type.output_params = vendor_name_str;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetModelName), [](int length) {
        NebulaIpcType<frost_bool, std::string> type;
        std::vector<char> buffer(length, 0);
        char* model_name_str = buffer.data();
        type.return_value = NEBULA_SystemInformationProviderGetModelName(model_name_str, 
                                                                         length);
        if (type.return_value == frost_true)
        {
            type.output_params = model_name_str;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetDeviceFamilyName), [](int length) {
        NebulaIpcType<frost_bool, std::string> type;
        std::vector<char> buffer(length, 0);
        char* family_name_str = buffer.data();
        type.return_value = NEBULA_SystemInformationProviderGetDeviceFamilyName(family_name_str,
                                                                                length);
        if (type.return_value == frost_true)
        {
            type.output_params = family_name_str;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetSoftwareVersion), []() {
        NebulaIpcType<frost_bool, std::string> type;
        const int software_version_length = 30;
        char software_version_str[software_version_length] = {0};
        type.return_value = NEBULA_SystemInformationProviderGetSoftwareVersion(software_version_str);
        if (type.return_value == frost_true)
        {
            type.output_params = software_version_str;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetHardwareVersion), [](int length) {
        NebulaIpcType<frost_bool, std::string> type;
        std::vector<char> buffer(length, 0);
        char* hardware_version = buffer.data();
        type.return_value = NEBULA_SystemInformationProviderGetHardwareVersion(hardware_version,
                                                                               length);
        if (type.return_value == frost_true)
        {
            type.output_params = hardware_version;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetSerialNumber), []() {
        NebulaIpcType<frost_bool, std::string> type;
        char* name = nullptr;
        type.return_value = NEBULA_SystemInformationProviderGetSerialNumber(name);
        if (type.return_value == frost_true)
        {
            type.output_params = name;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetSecureStore), []() {
        NebulaIpcType<frost_bool, std::vector<char>> tmp_secure_store;
        char* secure_store_data = nullptr;
        frost_int length = 0;
        tmp_secure_store.return_value = NEBULA_SystemInformationProviderGetSecureStore(&secure_store_data, &length);
        if (tmp_secure_store.return_value == frost_true)
        {
            tmp_secure_store.output_params = std::vector<char>(secure_store_data,
                    secure_store_data + length);
        }
        free(secure_store_data);
        return tmp_secure_store; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetDeviceSessionID), []() {
        NebulaIpcType<frost_bool, std::vector<char>> tmp_device_session_id;
        char* session_id_data = nullptr;
        frost_int length = 0;
        tmp_device_session_id.return_value = NEBULA_SystemInformationProviderGetDeviceSessionID(&session_id_data, &length);
        if (tmp_device_session_id.return_value == frost_true)
        {
            tmp_device_session_id.output_params = std::vector<char>(session_id_data,
                    session_id_data + length);
        }
        free(session_id_data);
        return tmp_device_session_id; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetActivitySessionID), []() {
        NebulaIpcType<frost_bool, std::vector<char>> tmp_active_session_id;
        char* session_id_data = nullptr;
        frost_int length = 0;
        tmp_active_session_id.return_value = NEBULA_SystemInformationProviderGetActivitySessionID(&session_id_data, &length);
        if (tmp_active_session_id.return_value == frost_true)
        {
            tmp_active_session_id.output_params = std::vector<char>(session_id_data,
                    session_id_data + length);
        }
        free(session_id_data);
        return tmp_active_session_id; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderUpdateAppsList), [](std::string& app_id, std::string& app_name, std::string& app_url, frost_bool app_on_my_apps){
        return NEBULA_SystemInformationProviderUpdateAppsList(app_id.c_str(), app_name.c_str(), app_url.c_str(), app_on_my_apps); });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderSetAITUrl), [](std::string& ait_url, std::string& launch_location, frost_bool ait_type){
        return NEBULA_SystemInformationProviderSetAITUrl(ait_url.c_str(), launch_location.c_str(), ait_type); });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderResetIFA), [](){
        return NEBULA_SystemInformationProviderResetIFA(); });
    
    server.bind(IPC_NAME(NEBULA_SystemInformationProviderUpdateNTP), [](){
        return NEBULA_SystemInformationProviderUpdateNTP(); });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetDeviceUpTime),
            &NEBULA_SystemInformationProviderGetDeviceUpTime);  

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetActivityUpTime),
            &NEBULA_SystemInformationProviderGetActivityUpTime);  

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetManufactureTime),
            &NEBULA_SystemInformationProviderGetManufactureTime);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayType),
                &NEBULA_SystemInformationProviderGetHDMIDisplayType);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayWidth),
            &NEBULA_SystemInformationProviderGetHDMIDisplayWidth);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayHeight),
            &NEBULA_SystemInformationProviderGetHDMIDisplayHeight);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetHomeScreenSaverTimeout),
            &NEBULA_SystemInformationProviderGetHomeScreenSaverTimeout);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetLoggingRegistrationTime), []() {
        NebulaIpcType<frost_bool, frost_uint> reg_time;
        frost_uint registration_time = 0;
        reg_time.return_value = NEBULA_SystemInformationProviderGetLoggingRegistrationTime(&registration_time);
        if (reg_time.return_value == frost_true)
        {
            reg_time.output_params = registration_time;
        }
        return reg_time; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetIFA), [](int length) {
        NebulaIpcType<frost_bool, std::string> ifa_info;
        std::vector<char> buffer(length, 0);
        char* ifa_str = buffer.data();
        ifa_info.return_value = NEBULA_SystemInformationProviderGetIFA(ifa_str, length);
        if (ifa_info.return_value == frost_true)
        {
            ifa_info.output_params = ifa_str;
        }
        return ifa_info; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetLimitAdTracking),
            &NEBULA_SystemInformationProviderGetLimitAdTracking);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetNativeUIAvailabilityData), []() {
            NebulaIpcType<frost_bool, MP_NEBULA_NativeUIAvailabilityData> availability_data;
            availability_data.return_value =
                NEBULA_SystemInformationProviderGetNativeUIAvailabilityData(&availability_data.output_params);
            return availability_data; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetProfile), [](std::string& profile_name) {
        NebulaIpcType<frost_bool,StringPair> result;

        const char* profile_type = nullptr;
        const char* profile_value = nullptr;

        result.return_value = NEBULA_SystemInformationProviderGetProfile(profile_name.c_str(), &profile_type, &profile_value);

        if(profile_type && profile_value)
        {
            result.output_params.first.assign(profile_type);
            result.output_params.second.assign(profile_value);
        }

        delete [] profile_type;
        delete [] profile_value;

        return result; });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsBrowserLauncherVisible),
            &NEBULA_SystemInformationProviderIsBrowserLauncherVisible);

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderSetDeviceLoggingID), [](std::string& dlid){
        return NEBULA_SystemInformationProviderSetDeviceLoggingID(dlid.c_str()); });

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsFVPTCAccepted),
            &NEBULA_SystemInformationProviderIsFVPTCAccepted);
}
