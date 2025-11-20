/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaTimeshiftCallbacksIpcAurora class.
 */

#include "NebulaTimeshiftCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_timeshift.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTimeshiftIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>
#include <stdio.h>
TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaTimeshiftCallbacksIpcAurora");

namespace
{

extern "C"
void proxyOnPlayPositionChangedEvent(frost_int32 position, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnPlayPositionChangedEvent),
                                position,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnPlaySpeedChangedEvent(frost_int32 speed, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnPlaySpeedChangedEvent),
                                speed,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyOnRecordingEvent(frost_int32 rec_state,
                           frost_int32 rec_error,
                           char* recording_id,
                           void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnRecordingEvent),
                                rec_state,
                                rec_error,
                                std::string(recording_id),
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

}

void
NebulaTimeshiftCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterToPlayPositionChangedEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterToPlayPositionChangedEvent(
                  proxyOnPlayPositionChangedEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterFromPlayPositionChangedEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterFromPlayPositionChangedEvent(
                  proxyOnPlayPositionChangedEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterToPlaySpeedChangedEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterToPlaySpeedChangedEvent(
                  proxyOnPlaySpeedChangedEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterFromPlaySpeedChangedEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterFromPlaySpeedChangedEvent(
                  proxyOnPlaySpeedChangedEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterToRecordingEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterToRecordingEvent(
                  proxyOnRecordingEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterFromRecordingEvent),
              [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterFromRecordingEvent(
                  proxyOnRecordingEvent,
                  RegisteredCallbacksConfig::toPointer<void*>(token));
    });
}

void
NebulaTimeshiftCallbacksIpcAurora::reset()
{
    NEBULA_DeregisterAllFromPlayPositionChangedEvent(proxyOnPlayPositionChangedEvent);
    NEBULA_DeregisterAllFromPlaySpeedChangedEvent(proxyOnPlaySpeedChangedEvent);
    NEBULA_DeregisterAllFromRecordingEvent(proxyOnRecordingEvent);
}
