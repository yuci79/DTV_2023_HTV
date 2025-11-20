/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaBluetoothSettingsCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULABLUETOOTHSETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULABLUETOOTHSETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_bluetooth_settings.h"

#include <vector>

class MP_NEBULA_BluetoothScanInfo;

/**
 * @brief Define the parameters of a NEBULA_BluetoothSettingsEventCallback callback.
 */
struct OnBluetoothSettingsEventCallback
{
    NEBULA_BluetoothSettingsEventCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnBluetoothSettingsEventCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_BluetoothSettingsAudioDataCallback callback.
 */
struct OnBluetoothSettingsAudioDataCallback
{
    NEBULA_BluetoothSettingsAudioDataCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnBluetoothSettingsAudioDataCallback& other) const;
};

#endif
