/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaStreamEventsCallbacksIpcAurora class.
 */

#include "NebulaStreamEventsCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_stream_events.h"
#include "nebula_src/adaptation_layer/ipc/NebulaStreamEventsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaStreamEventsCallbacksIpcAurora");

RegisteredStreamHandles<NEBULA_DsmccStreamEventSubscription*>
            NebulaStreamEventsCallbacksIpcAurora::m_local_stream_event_subscription_handles;

namespace
{

extern "C"
void proxyDsmccStreamEventCallback(void * user_data,
                                   const char * name,
                                   const frost_uint8 * data,
                                   frost_uint data_size)
{
    TRACE_INFO(("%s()\n", __func__));

    std::vector<frost_uint8> data_vector;
    std::copy(data, data + data_size, std::back_inserter(data_vector));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    client.call(IPC_NAME(NEBULA_DsmccStreamEventCallback), 
                RegisteredCallbacksConfig::toToken(user_data),
                std::string(name),
                data_vector);
}

}

bool
NebulaStreamEventsCallbacksIpcAurora::find(
    RegisteredStreamHandlesConfig::TokenType stream_event_subscription_token,
    NEBULA_DsmccStreamEventSubscription*& stream_event_subscription)
{
    return m_local_stream_event_subscription_handles.find(stream_event_subscription_token,
                                                          stream_event_subscription);
}

void
NebulaStreamEventsCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DsmccStreamEventSubscribe),
                [](std::string event_name, RegisteredCallbacksConfig::TokenType user_data_token)
    {
        const auto stream_event_subscription = NEBULA_DsmccStreamEventSubscribe(
                                    event_name.c_str(),
                                    proxyDsmccStreamEventCallback,
                                    RegisteredCallbacksConfig::toPointer<void*>(user_data_token));

        if (!stream_event_subscription)
        {
            return RegisteredStreamHandlesConfig::m_invalid_token;
        }

        bool duplicate = false;
        const auto stream_event_subscription_token
            = m_local_stream_event_subscription_handles.add(stream_event_subscription, duplicate);
        
        if (duplicate)
        {
            // NEBULA_DsmccStreamEventSubscribe always generates a new stream handle.
            // So duplicate should always be false.
            SHOULD_NOT_BE_HERE();
        }
        
        return stream_event_subscription_token;
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamEventUnsubscribe),
                [](RegisteredStreamHandlesConfig::TokenType stream_event_subscription_token)
    {
        NEBULA_DsmccStreamEventSubscription* stream_event_subscription;
        if (find(stream_event_subscription_token, stream_event_subscription))
        {
            NEBULA_DsmccStreamEventUnsubscribe(stream_event_subscription);
            
            m_local_stream_event_subscription_handles.remove(stream_event_subscription_token);
        }
    });
}
