/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_network_settings.
 */

#include "nebula/adaptation_layer/nebula_network_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaNetworkSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/cabot_assert.h"

NEBULA_IPDevice
NEBULA_GetSelectedNetworkDevice(void)
{
    return nebulaRpcCall<NEBULA_IPDevice>(IPC_NAME(NEBULA_GetSelectedNetworkDevice));
}

frost_bool
NEBULA_NetworkSettingsGetDeviceMacID(frost_int device_index, char* mac_address)
{
    std::string address;
    const auto result = nebulaRpcCall<frost_bool, std::string>(address,
                            IPC_NAME(NEBULA_NetworkSettingsGetDeviceMacID), device_index);
    if (result == frost_true)
    {
        strncpy(mac_address, address.c_str(), NEBULA_MAC_ADDRESS_NAME_LENGTH);
        mac_address[NEBULA_MAC_ADDRESS_NAME_LENGTH - 1] = '\0';
    }
    return result;
}

frost_bool
NEBULA_NetworkSettingsCheckInternetConnection(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_NetworkSettingsCheckInternetConnection));
}

frost_bool
NEBULA_NetworkSettingsEnableNetworkInterface(frost_int device_index, frost_bool enable)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_NetworkSettingsEnableNetworkInterface),
                                     device_index,
                                     enable);
}

frost_bool
NEBULA_NetworkSettingsGetDeviceIPParams(frost_int device_index, NEBULA_IPParams* settings)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_IPParams>(*settings,
                        IPC_NAME(NEBULA_NetworkSettingsGetDeviceIPParams),
                        device_index);
}

frost_bool
NEBULA_NetworkSettingsGetNumberOfDevices(frost_int* number_of_devices)
{
    return nebulaRpcCall<frost_bool, frost_int>(*number_of_devices,
                                                IPC_NAME(NEBULA_NetworkSettingsGetNumberOfDevices));
}

frost_bool
NEBULA_NetworkSettingsSetDynamicIPAddress(frost_int ip_device)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_NetworkSettingsSetDynamicIPAddress),
                                     ip_device);
}

frost_bool
NEBULA_NetworkSettingsSetStaticIPAddress(const NEBULA_IPParams ip_params)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_NetworkSettingsSetStaticIPAddress),
                                     MP_NEBULA_IPParams(ip_params));
}

frost_bool
NEBULA_AnyWiFiProfileSaved()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AnyWiFiProfileSaved));
}

frost_bool
NEBULA_IsDongleDetected(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsDongleDetected));
}