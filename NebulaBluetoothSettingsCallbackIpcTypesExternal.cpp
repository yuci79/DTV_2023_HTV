/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaBluetoothSettingsCallbacksIpcExternal structures.
 */

#include "NebulaBluetoothSettingsCallbackIpcTypesExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaBluetoothSettingsIpcTypes.hpp"
      
bool
OnBluetoothSettingsEventCallback::operator==(
        const OnBluetoothSettingsEventCallback& other) const
{
    // Just compare user_data.
    return (m_user_data == other.m_user_data);
}
        
bool
OnBluetoothSettingsAudioDataCallback::operator==(
        const OnBluetoothSettingsAudioDataCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}