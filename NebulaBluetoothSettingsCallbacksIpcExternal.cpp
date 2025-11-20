/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for the NebulaBluetoothSettingsCallbacksIpcExternal class 
 *        and some associated functions.
 */
#include "NebulaBluetoothSettingsCallbacksIpcExternal.hpp"
#include "NebulaBluetoothSettingsCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaBluetoothSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>
#include <vector>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaBluetoothSettingsCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<OnBluetoothSettingsEventCallback> local_event_register;
RegisteredCallbacks<OnBluetoothSettingsAudioDataCallback> local_audio_data_register;

// Proxy external callbacks
void
proxyEventCallback(MP_NEBULA_BluetoothSettingsEventData event,
                   RegisteredCallbacksConfig::TokenType user_token)
{
    OnBluetoothSettingsEventCallback callback;
    if (local_event_register.find(user_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, user_token, user_data));

        if (function_pointer)
        {
            function_pointer(&event, user_data);
        }
    }
}

void proxyAudioDataCallback(std::vector<std::uint8_t> audio_data,
                            int audio_data_length,
                            RegisteredCallbacksConfig::TokenType user_token)
{
    OnBluetoothSettingsAudioDataCallback callback;
    if (local_audio_data_register.find(user_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, user_token, user_data));

        if (function_pointer)
        {
          function_pointer(static_cast<const void*>(audio_data.data()), 
                           audio_data_length, user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

// Functions to register and deregister callbacks
frost_bool
NEBULA_SetBluetoothSettingsEventCallback(NEBULA_BluetoothSettingsEventCallback* callback, 
                                         void* user_data)
{
    TRACE_INFO(("%s() callback %p, user_data %p \n", __func__, 
                callback, user_data));

    const bool register_callback = callback != nullptr;
    frost_bool success = frost_false;
    if (register_callback)
    {
        success = local_event_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_SetBluetoothSettingsEventCallback),
                            register_callback);
    }
    else
    {    
        // Special case to deregister
        success = local_event_register.deregisterCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_SetBluetoothSettingsEventCallback),
                            register_callback);
    }

    return success;
}

frost_bool
NEBULA_BluetoothOpenMic(NEBULA_BluetoothSettingsAudioDataCallback callback, void* user_data)
{
    TRACE_INFO(("%s() callback %p, user_data %p \n", __func__, 
                callback, user_data));

    return local_audio_data_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_BluetoothOpenMic));
}

frost_bool
NEBULA_BluetoothCloseMic(void)
{
    TRACE_INFO(("%s()\n", __func__));
    
    // Not able to deregister a specific callback without a function pointer or user_data parameter.
    local_audio_data_register.removeAll();

    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_BluetoothCloseMic));
}

#ifdef __cplusplus
}
#endif

void NebulaBluetoothSettingsCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_BluetoothSettingsEventCallback), proxyEventCallback);
    server.bind(IPC_NAME(NEBULA_BluetoothSettingsAudioDataCallback), proxyAudioDataCallback);
}
