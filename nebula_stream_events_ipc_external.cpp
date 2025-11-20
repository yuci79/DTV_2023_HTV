/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_stream_events.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaStreamEventsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"
#include "NebulaIpcHelper.hpp"

extern "C"
frost_bool NEBULA_DsmccStreamEventSetSource(
        NEBULA_DsmccStreamEventSubscription * subscription,
        frost_uint component_tag,
        frost_uint event_count,
        const NEBULA_DsmccStreamEvent * event_array)
{
    const NEBULA_DsmccStreamEvent* event = event_array;
    NebulaIpcStreamEvents events;
    for (frost_uint index = 0; index < event_count; index++)
    {
        events.push_back(std::make_pair(event->m_id, event->m_name));
        event++;
    }

    return nebulaRpcCall<frost_bool>(
                IPC_NAME(NEBULA_DsmccStreamEventSetSource),
                RegisteredStreamHandlesConfig::toToken(subscription),
                component_tag,
                events);
}

extern "C"
frost_bool NEBULA_DsmccStreamEventEnable(
        NEBULA_DsmccStreamEventSubscription * subscription,
        frost_uint component_tag,
        const char * event_name,
        frost_uint event_id)
{
    return nebulaRpcCall<frost_bool>(
                IPC_NAME(NEBULA_DsmccStreamEventEnable),
                RegisteredStreamHandlesConfig::toToken(subscription),
                component_tag,
                std::string(event_name),
                event_id);
}