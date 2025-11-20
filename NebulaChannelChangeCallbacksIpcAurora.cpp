/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaChannelChangeCallbacksIpcAurora class.
 */

#include "NebulaChannelChangeCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelChangeIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaChannelChangeCallbacksIpcAurora");

namespace
{
extern "C"
void proxyOnChannelChange(NEBULA_Channel_ID channel_id,
                          NEBULA_Tuner_ID tuner_id,
                          NEBULA_ChannelChangeResult result,
                          frost_bool keep_app_running,
                          void* user_data,
                          frost_bool generated_artificially)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnChannelChange), 
                                channel_id, tuner_id, result, keep_app_running,
                                RegisteredCallbacksConfig::toToken(user_data),
                                generated_artificially);
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

void proxyOnTunerStatusChange(NEBULA_Tuner_ID const tuner_id,
                              NEBULA_TunerStatus const tuner_status,
                              void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnTunerStatusChange),
                                tuner_id, tuner_status,
                                RegisteredCallbacksConfig::toToken(user_data));

    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

void proxyOnComponentChange(void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));
    
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnComponentChange),
                                RegisteredCallbacksConfig::toToken(user_data));

    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}
}

void
NebulaChannelChangeCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterToChannelChangeStatus),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_RegisterToChannelChangeStatus(
                    proxyOnChannelChange,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_DeregisterFromChannelChangeStatus),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_DeregisterFromChannelChangeStatus(
                    proxyOnChannelChange, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_RegisterToTunerStatus),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_RegisterToTunerStatus(
                    proxyOnTunerStatusChange,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_DeregisterFromTunerStatus),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_DeregisterFromTunerStatus(
                    proxyOnTunerStatusChange,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_RegisterToComponentChangeEvent),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_RegisterToComponentChangeEvent(
                    proxyOnComponentChange, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });

    server.bind(IPC_NAME(NEBULA_DeregisterFromComponentChangeEvent),
                [](RegisteredCallbacksConfig::TokenType token)
      {
          return NEBULA_DeregisterFromComponentChangeEvent(
                    proxyOnComponentChange,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
      });
}

void
NebulaChannelChangeCallbacksIpcAurora::reset()
{
    NEBULA_DeregisterAllFromChannelChangeStatus(proxyOnChannelChange);
    NEBULA_DeregisterAllFromTunerStatus(proxyOnTunerStatusChange);
    NEBULA_DeregisterAllFromComponentChangeEvent(proxyOnComponentChange);
}
