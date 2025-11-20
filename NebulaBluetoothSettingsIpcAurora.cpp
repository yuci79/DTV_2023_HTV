/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for the NebulaBluetoothSettingsIpcAurora class.
 */

#include "NebulaBluetoothSettingsIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_bluetooth_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaBluetoothSettingsIpcTypes.hpp"
#include <rpc/server.h>


namespace
{
void
populateScanInfoVector(std::vector<MP_NEBULA_BluetoothScanInfo>& output_params,
                       NEBULA_BluetoothScanInfo**& scan_info_array,
                       unsigned int count)
{
    if (scan_info_array)
    {
        for (unsigned int index = 0; index < count; ++index)
        {
            NEBULA_BluetoothScanInfo* scan_info = scan_info_array[index];
            if (scan_info)
            {
                output_params.push_back(*scan_info);
                free(scan_info);
            }
        }
        free(scan_info_array);
    }
}
}

void
NebulaBluetoothSettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_IsBluetoothEnabled), NEBULA_IsBluetoothEnabled);
    server.bind(IPC_NAME(NEBULA_BluetoothEnable), NEBULA_BluetoothEnable);
    server.bind(IPC_NAME(NEBULA_BluetoothDisable), NEBULA_BluetoothDisable);
    server.bind(IPC_NAME(NEBULA_BluetoothScanDevices), NEBULA_BluetoothScanDevices);
    server.bind(IPC_NAME(NEBULA_BluetoothConnectDevice), NEBULA_BluetoothConnectDevice);
    server.bind(IPC_NAME(NEBULA_BluetoothDisconnectDevice), NEBULA_BluetoothDisconnectDevice);
    server.bind(IPC_NAME(NEBULA_BluetoothRemoveDevice), NEBULA_BluetoothRemoveDevice);

    server.bind(IPC_NAME(NEBULA_GetBluetoothScanResults), []()
        {
            NebulaIpcType<frost_bool, std::vector<MP_NEBULA_BluetoothScanInfo>> result;

            NEBULA_BluetoothScanInfo** scan_info_array = nullptr;
            frost_uint32 count = 0;
            result.return_value = NEBULA_GetBluetoothScanResults(&scan_info_array, &count);

            populateScanInfoVector(result.output_params, scan_info_array, count);

            return result;
        });

    server.bind(IPC_NAME(NEBULA_GetBluetoothPairedDevices), []()
        {
            NebulaIpcType<frost_bool, std::vector<MP_NEBULA_BluetoothScanInfo>> result;

            NEBULA_BluetoothScanInfo** scan_info_array = nullptr;
            frost_uint32 count = 0;
            result.return_value = NEBULA_GetBluetoothPairedDevices(&scan_info_array, &count);

            populateScanInfoVector(result.output_params, scan_info_array, count);
   
            return result;
        });
}
