/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_bluetooth_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULABLUETOOTHSETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULABLUETOOTHSETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_bluetooth_settings.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_IsBluetoothEnabled);
DECLARE_IPC_NAME(NEBULA_BluetoothEnable);
DECLARE_IPC_NAME(NEBULA_BluetoothDisable);
DECLARE_IPC_NAME(NEBULA_BluetoothScanDevices);
DECLARE_IPC_NAME(NEBULA_BluetoothConnectDevice);
DECLARE_IPC_NAME(NEBULA_BluetoothDisconnectDevice);
DECLARE_IPC_NAME(NEBULA_BluetoothRemoveDevice);
DECLARE_IPC_NAME(NEBULA_GetBluetoothScanResults);
DECLARE_IPC_NAME(NEBULA_GetBluetoothPairedDevices);
DECLARE_IPC_NAME(NEBULA_SetBluetoothSettingsEventCallback);
DECLARE_IPC_NAME(NEBULA_BluetoothOpenMic);
DECLARE_IPC_NAME(NEBULA_BluetoothCloseMic);

DECLARE_IPC_NAME(NEBULA_BluetoothSettingsEventCallback);
DECLARE_IPC_NAME(NEBULA_BluetoothSettingsAudioDataCallback);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_BluetoothEventType);


struct MP_NEBULA_BluetoothScanInfo : public NEBULA_BluetoothScanInfo
{
    MP_NEBULA_BluetoothScanInfo() = default;

    MP_NEBULA_BluetoothScanInfo(const NEBULA_BluetoothScanInfo& other)
        : NEBULA_BluetoothScanInfo(other)
    {}

    MSGPACK_DEFINE(device_name,
                   index,
                   is_connected);
    
    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_BluetoothScanInfo,
                   "",
                   index,
                   is_connected);    
};

struct MP_NEBULA_BluetoothSettingsEventData : public NEBULA_BluetoothSettingsEventData
{
    MP_NEBULA_BluetoothSettingsEventData() = default;

    MP_NEBULA_BluetoothSettingsEventData(const NEBULA_BluetoothSettingsEventData& other)
        : NEBULA_BluetoothSettingsEventData(other)
    {}

    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_BluetoothSettingsEventData, event_type);
};

#endif
