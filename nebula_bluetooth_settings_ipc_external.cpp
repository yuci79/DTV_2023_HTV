/**
 * Copyright 2022 Vestel A.S.  All rights reserved.
 *
 * @file
 * @brief NEBULA implementation for bluetooth settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaBluetoothSettingsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcTypesExternal.hpp"
#include "NebulaIpcHelper.hpp"

#include "nebula/adaptation_layer/nebula_bluetooth_settings.h"
#include "utilities_debug/unused.h"

#include <vector>

extern "C" frost_bool
NEBULA_IsBluetoothEnabled(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsBluetoothEnabled));
}

extern "C" frost_bool
NEBULA_BluetoothEnable(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothEnable));

}

extern "C" frost_bool
NEBULA_BluetoothDisable(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothDisable));
}

extern "C" frost_bool
NEBULA_BluetoothScanDevices(frost_int timeout)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothScanDevices), timeout);
}

extern "C" frost_bool
NEBULA_GetBluetoothScanResults(NEBULA_BluetoothScanInfo*** scan_info, frost_uint32* count)
{
    std::vector<MP_NEBULA_BluetoothScanInfo> scan_info_vector;
    const auto result = nebulaRpcCall<frost_bool, std::vector<MP_NEBULA_BluetoothScanInfo>>(
                                     scan_info_vector,
                                     IPC_NAME(NEBULA_GetBluetoothScanResults));

    createNebulaArrayFromIpcVector(scan_info_vector, *count, *scan_info);

    return result;
}

extern "C" frost_bool
NEBULA_GetBluetoothPairedDevices(NEBULA_BluetoothScanInfo*** scan_info, frost_uint32* count)
{
    std::vector<MP_NEBULA_BluetoothScanInfo> scan_info_vector;
    const auto result = nebulaRpcCall<frost_bool, std::vector<MP_NEBULA_BluetoothScanInfo>>(
                                     scan_info_vector,
                                     IPC_NAME(NEBULA_GetBluetoothPairedDevices));
                                     
    createNebulaArrayFromIpcVector(scan_info_vector, *count, *scan_info);

    return result;
}

extern "C" frost_bool
NEBULA_BluetoothConnectDevice(frost_int device_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothConnectDevice), device_index);
}

extern "C" frost_bool
NEBULA_BluetoothDisconnectDevice(frost_int device_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothDisconnectDevice), device_index);
}

extern "C" frost_bool
NEBULA_BluetoothRemoveDevice(frost_int device_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothRemoveDevice), device_index);
}
