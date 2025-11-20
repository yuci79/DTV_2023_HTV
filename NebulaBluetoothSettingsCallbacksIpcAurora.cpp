/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaBluetoothSettingsCallbacksIpcAurora class.
 */

#include "NebulaBluetoothSettingsCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_bluetooth_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaBluetoothSettingsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaBluetoothSettingsCallbacksIpcAurora");

namespace
{
extern "C"
void proxyOnEvent(NEBULA_BluetoothSettingsEventData* event_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    bool ok = false;
    if (event_data)
    {
        AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
        ok = client.call(IPC_NAME(NEBULA_BluetoothSettingsEventCallback), 
                         MP_NEBULA_BluetoothSettingsEventData(*event_data),
                         RegisteredCallbacksConfig::toToken(user_data)
                         );
    }

    if (!ok)
    {
        TRACE_ERROR(("%s() Failed event_data %p\n", __func__, event_data));
    }
}

void proxyOnAudioData(const void* audio_data, const int len, void* user_data)
{
    TRACE_VERBOSE(("%s() %d bytes\n", __func__, len));

    bool ok = false;

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    if (audio_data && (len > 0))
    {
        const std::vector<std::uint8_t> audio_vector(
                static_cast<const std::uint8_t*>(audio_data),
                static_cast<const std::uint8_t*>(audio_data) + len);

        ok = client.call(IPC_NAME(NEBULA_BluetoothSettingsAudioDataCallback),
                         audio_vector,
                         len,
                         RegisteredCallbacksConfig::toToken(user_data));
    }
    else
    {
        std::vector<std::uint8_t> audio_vector;

        ok = client.call(IPC_NAME(NEBULA_BluetoothSettingsAudioDataCallback),
                         audio_vector,
                         len,
                         RegisteredCallbacksConfig::toToken(user_data));
    }

    if (!ok)
    {
        TRACE_ERROR(("%s() Failed audio_data %p, length %d\n", __func__,
                      audio_data, len));
    }
}

}

void
NebulaBluetoothSettingsCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SetBluetoothSettingsEventCallback),
                [](RegisteredCallbacksConfig::TokenType token,
                bool register_callback)
      {
          return NEBULA_SetBluetoothSettingsEventCallback(
                    register_callback ? proxyOnEvent : nullptr,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_BluetoothOpenMic),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_BluetoothOpenMic(
                    proxyOnAudioData, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    // This is effectively the de-register function associated with NEBULA_BluetoothOpenMic()
    server.bind(IPC_NAME(NEBULA_BluetoothCloseMic), &NEBULA_BluetoothCloseMic);

}

void
NebulaBluetoothSettingsCallbacksIpcAurora::reset()
{
    NEBULA_SetBluetoothSettingsEventCallback(nullptr, nullptr);
    NEBULA_BluetoothCloseMic();
}
