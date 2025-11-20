/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaStreamEventsIpcAurora class.
 */

#include "NebulaStreamEventsIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_stream_events.h"
#include "nebula_src/adaptation_layer/ipc/NebulaStreamEventsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/aurora/NebulaStreamEventsCallbacksIpcAurora.hpp"
#include <rpc/server.h>

void
NebulaStreamEventsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DsmccStreamEventSetSource),[]
        (RegisteredStreamHandlesConfig::TokenType subscription_token,
         frost_uint component_tag,
         NebulaIpcStreamEvents& events)
    {
        const frost_uint event_count = static_cast<frost_uint>(events.size());
        NEBULA_DsmccStreamEvent* event_array = new NEBULA_DsmccStreamEvent[event_count];
        for (frost_uint index = 0; index < event_count; index++)
        {
            auto& event = events.at(index);
            event_array[index].m_id = std::get<0>(event);
            event_array[index].m_name = std::get<1>(event).c_str();
        }
        
        frost_bool result = frost_false;

        NEBULA_DsmccStreamEventSubscription* subscription = nullptr;
        if (NebulaStreamEventsCallbacksIpcAurora::find(subscription_token, subscription))
        {
            result = NEBULA_DsmccStreamEventSetSource(subscription,
                                                      component_tag,
                                                      event_count,
                                                      event_array);
        }

        delete[] event_array;
        return result;
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamEventEnable),[]
        (RegisteredStreamHandlesConfig::TokenType subscription_token,
         frost_uint component_tag,
         std::string event_name,
         frost_uint event_id)
    {
        frost_bool result = frost_false;

        NEBULA_DsmccStreamEventSubscription* subscription = nullptr;
        if (NebulaStreamEventsCallbacksIpcAurora::find(subscription_token, subscription))
        {
            result = NEBULA_DsmccStreamEventEnable(subscription,
                                                   component_tag,
                                                   event_name.c_str(),
                                                   event_id);
        }

        return result;
    });
}
