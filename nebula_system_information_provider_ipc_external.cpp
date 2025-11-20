/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_system_information_provider.
 */

#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include "nebula_src/adaptation_layer/ipc/NebulaSystemInformationProviderIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

namespace 
{
    const char* copyBuffer(const std::string& src)
    {
        if (src.empty())
        {
            return nullptr; 
        }
       // Allocate memory for the destination buffer
        char* buffer = new char[src.length() + 1];

        // Copy the contents of the source string to the buffer
        strcpy(buffer, src.c_str());

        return buffer;
    }
}

extern "C" frost_uint
NEBULA_SystemInformationProviderGetPanelMaxBPC()
{
    return nebulaRpcCall<frost_uint>(IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxBPC));
}

extern "C" frost_uint
NEBULA_SystemInformationProviderGetPanelMaxRefreshRate()
{
    return nebulaRpcCall<frost_uint>(
            IPC_NAME(NEBULA_SystemInformationProviderGetPanelMaxRefreshRate));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsUhdHevcSupported()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsUhdHevcSupported));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsCableSupportEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsCableSupportEnabled));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsSatelliteSupportEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsSatelliteSupportEnabled));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsDVBIServiceDiscoveryEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsDVBIServiceDiscoveryEnabled));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsFreeviewPlayEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsFreeviewPlayEnabled));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderGetCIPlusEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderGetCIPlusEnabled));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderGetCicamPlayerEnabled()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderGetCicamPlayerEnabled));
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetIFA(char* ifa, const int length)
{
    std::string ifa_str;
    const auto result = nebulaRpcCall<frost_bool, std::string>(ifa_str,
                                IPC_NAME(NEBULA_SystemInformationProviderGetIFA), length);
    if (result == frost_true)
    {
        strncpy(ifa, ifa_str.c_str(), length);
        ifa[length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetVendorName(char* vendor_name)
{
    std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                                IPC_NAME(NEBULA_SystemInformationProviderGetVendorName));
    if (result == frost_true)
    {
        const int buffer_length = 128;
        strncpy(vendor_name, name.c_str(), buffer_length);
        vendor_name[buffer_length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetModelName(char* model_name, const int length)
{
    std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                                IPC_NAME(NEBULA_SystemInformationProviderGetModelName),
                                length);
    if (result == frost_true)
    {
        strncpy(model_name, name.c_str(), length);
        model_name[length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetDeviceFamilyName(char* device_family_name,
                                                               const int length)
{
    std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                            IPC_NAME(NEBULA_SystemInformationProviderGetDeviceFamilyName),
                            length);
    if (result == frost_true)
    {
        strncpy(device_family_name, name.c_str(), length);
        device_family_name[length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetSoftwareVersion(char* software_version)
{
    std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                            IPC_NAME(NEBULA_SystemInformationProviderGetSoftwareVersion));
    if (result == frost_true)
    {
        const int buffer_length = 30;
        strncpy(software_version, name.c_str(), buffer_length);
        software_version[buffer_length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetHardwareVersion(char* hardware_version,
                                                              const int length)
{
    std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                            IPC_NAME(NEBULA_SystemInformationProviderGetHardwareVersion),
                            length);
    if (result == frost_true)
    {
        strncpy(hardware_version, name.c_str(), length);
        hardware_version[length - 1] = '\0';
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetSerialNumber(char*& serial_number)
{
    static std::string name;
    const auto result = nebulaRpcCall<frost_bool, std::string>(name,
                            IPC_NAME(NEBULA_SystemInformationProviderGetSerialNumber));
    if (result == frost_true)
    {
        serial_number = const_cast<char*>(name.c_str());
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetSecureStore(char** secure_store, frost_int* length)
{
    std::vector<char> tmp_secure_store;
    const auto result = nebulaRpcCall<frost_bool, std::vector<char>>(
            tmp_secure_store, IPC_NAME(NEBULA_SystemInformationProviderGetSecureStore));

    if (result == frost_true)
    {
        *length = static_cast<frost_int>(tmp_secure_store.size());
        *secure_store = static_cast<char*>(malloc(*length));
        std::copy(tmp_secure_store.begin(), tmp_secure_store.begin() + *length, *secure_store);
    }
    return result;
}

extern "C" frost_uint
NEBULA_SystemInformationProviderGetDeviceUpTime()
{
    return nebulaRpcCall<frost_uint>(IPC_NAME(NEBULA_SystemInformationProviderGetDeviceUpTime));
}

extern "C" frost_uint
NEBULA_SystemInformationProviderGetActivityUpTime()
{
    return nebulaRpcCall<frost_uint>(IPC_NAME(NEBULA_SystemInformationProviderGetActivityUpTime));
}

extern "C" frost_uint
NEBULA_SystemInformationProviderGetManufactureTime()
{
    return nebulaRpcCall<frost_uint>(IPC_NAME(NEBULA_SystemInformationProviderGetManufactureTime));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderGetLimitAdTracking()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderGetLimitAdTracking));
}

extern "C" frost_uint8
NEBULA_SystemInformationProviderGetHDMIDisplayType()
{
    return nebulaRpcCall<frost_uint8>(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayType));
}

extern "C" frost_uint8
NEBULA_SystemInformationProviderGetHDMIDisplayWidth()
{
    return nebulaRpcCall<frost_uint8>(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayWidth));
}

extern "C" frost_uint8
NEBULA_SystemInformationProviderGetHDMIDisplayHeight()
{
    return nebulaRpcCall<frost_uint8>(IPC_NAME(NEBULA_SystemInformationProviderGetHDMIDisplayHeight));
}

extern "C" frost_int
NEBULA_SystemInformationProviderGetHomeScreenSaverTimeout()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SystemInformationProviderGetHomeScreenSaverTimeout));
}

extern "C" frost_bool
NEBULA_SystemInformationProviderGetLoggingRegistrationTime(frost_uint* registration_time)
{
    unsigned int reg_time = 0;
    const auto result = nebulaRpcCall<frost_bool, unsigned int>(
            reg_time, IPC_NAME(NEBULA_SystemInformationProviderGetLoggingRegistrationTime));

    if (result == frost_true)
    {
        *registration_time = static_cast<frost_uint>(reg_time);
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetDeviceSessionID(char** session_id, frost_int* length)
{
    std::vector<char> tmp_session_id;
    const auto result = nebulaRpcCall<frost_bool, std::vector<char>>(
            tmp_session_id, IPC_NAME(NEBULA_SystemInformationProviderGetDeviceSessionID));

    if (result == frost_true)
    {
        *length = static_cast<frost_int>(tmp_session_id.size());
        *session_id = static_cast<char*>(malloc(*length));
        std::copy(tmp_session_id.begin(), tmp_session_id.begin() + *length, *session_id);
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetActivitySessionID(char** session_id, frost_int* length)
{
    std::vector<char> tmp_session_id;
    const auto result = nebulaRpcCall<frost_bool, std::vector<char>>(
            tmp_session_id, IPC_NAME(NEBULA_SystemInformationProviderGetActivitySessionID));

    if (result == frost_true)
    {
        *length = static_cast<frost_int>(tmp_session_id.size());
        *session_id = static_cast<char*>(malloc(*length));
        std::copy(tmp_session_id.begin(), tmp_session_id.begin() + *length, *session_id);
    }
    return result;
}

extern "C"
frost_bool NEBULA_SystemInformationProviderGetNativeUIAvailabilityData(
                NEBULA_NativeUIAvailabilityData * availability_data)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_NativeUIAvailabilityData>(*availability_data,
                        IPC_NAME(NEBULA_SystemInformationProviderGetNativeUIAvailabilityData));
}

extern "C"
frost_bool NEBULA_SystemInformationProviderUpdateAppsList(const char* app_id, const char* app_name, const char* app_url, frost_bool app_on_my_apps)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderUpdateAppsList),
                                     app_id, app_name, app_url, app_on_my_apps);
}

extern "C"
frost_bool NEBULA_SystemInformationProviderSetAITUrl(const char* ait_url, const char* launch_location, frost_bool ait_type)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderSetAITUrl),
                                     ait_url, launch_location, ait_type);
}

extern "C"
frost_bool NEBULA_SystemInformationProviderUpdateNTP()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderUpdateNTP));
}

extern "C"
frost_bool NEBULA_SystemInformationProviderResetIFA()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderResetIFA));
}

extern "C" 
frost_bool NEBULA_SystemInformationProviderGetProfile(const char* profile_name, const char** profile_type, const char** profile_value)
{
    StringPair profile_data;
    
    const frost_bool result = nebulaRpcCall<frost_bool, StringPair>(profile_data,
                                            IPC_NAME(NEBULA_SystemInformationProviderGetProfile), profile_name); 

    *profile_type = copyBuffer(profile_data.first);
    *profile_value = copyBuffer(profile_data.second);

    return result;
}

extern "C" frost_bool
NEBULA_SystemInformationProviderIsBrowserLauncherVisible()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsBrowserLauncherVisible));
}


extern "C" frost_bool
NEBULA_SystemInformationProviderIsFVPTCAccepted()
{
    return nebulaRpcCall<frost_bool>(
            IPC_NAME(NEBULA_SystemInformationProviderIsFVPTCAccepted));
}

extern "C"
frost_bool NEBULA_SystemInformationProviderSetDeviceLoggingID(const char* dlid)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SystemInformationProviderSetDeviceLoggingID), dlid);
}

