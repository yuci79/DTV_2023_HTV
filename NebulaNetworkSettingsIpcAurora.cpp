/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaNetworkSettingsIpcAurora class.
 */

#include "NebulaNetworkSettingsIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaNetworkSettingsIpcTypes.hpp"

#include <rpc/server.h>

void
NebulaNetworkSettingsIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_network_settings 'C' functions.

    server.bind(IPC_NAME(NEBULA_GetSelectedNetworkDevice),
                &NEBULA_GetSelectedNetworkDevice);

    server.bind(IPC_NAME(NEBULA_NetworkSettingsGetDeviceMacID), [](frost_int device_index) {
        NebulaIpcType<frost_bool, std::string> type;
        char mac_address[NEBULA_MAC_ADDRESS_NAME_LENGTH] = {0};
        type.return_value = NEBULA_NetworkSettingsGetDeviceMacID(device_index, mac_address);
        if (type.return_value == frost_true)
        {
            type.output_params = mac_address;
        }
        return type; });

    server.bind(IPC_NAME(NEBULA_NetworkSettingsCheckInternetConnection),
                &NEBULA_NetworkSettingsCheckInternetConnection);

    server.bind(IPC_NAME(NEBULA_NetworkSettingsEnableNetworkInterface),
                &NEBULA_NetworkSettingsEnableNetworkInterface);

    server.bind(IPC_NAME(NEBULA_NetworkSettingsGetDeviceIPParams), [](frost_int device_index)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_IPParams> result;
        result.return_value = NEBULA_NetworkSettingsGetDeviceIPParams(device_index,
                                                                      &result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_NetworkSettingsGetNumberOfDevices), []()
    {
        NebulaIpcType<frost_bool, frost_int> result;
        result.return_value = NEBULA_NetworkSettingsGetNumberOfDevices(&result.output_params);
        return result;
    });
    
    server.bind(IPC_NAME(NEBULA_NetworkSettingsSetDynamicIPAddress),
                &NEBULA_NetworkSettingsSetDynamicIPAddress);

    server.bind(IPC_NAME(NEBULA_NetworkSettingsSetStaticIPAddress), [](MP_NEBULA_IPParams ip_params)
    {
        return NEBULA_NetworkSettingsSetStaticIPAddress(ip_params);
    });

    server.bind(IPC_NAME(NEBULA_AnyWiFiProfileSaved), &NEBULA_AnyWiFiProfileSaved);

    server.bind(IPC_NAME(NEBULA_IsDongleDetected), &NEBULA_IsDongleDetected);
}
