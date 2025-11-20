/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaStreamEventsCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaStreamEventsCallbacksIpcExternal.hpp"
#include "NebulaStreamEventsCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaStreamEventsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaStreamEventsCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<DsmccStreamEventCallback> local_dsmcc_stream_event_user_data;

std::map<RegisteredStreamHandlesConfig::TokenType,
         RegisteredCallbacksConfig::TokenType> local_stream_to_callback_map;
std::mutex local_stream_to_callback_mutex;

// Proxy external callbacks
void
proxyDsmccStreamEvent(RegisteredCallbacksConfig::TokenType user_data_token,
                      std::string name,
                      std::vector<frost_uint8> data)
{
    DsmccStreamEventCallback callback;
    if (local_dsmcc_stream_event_user_data.find(user_data_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() user_data_token %u, user_data %p \n",
                   __func__, user_data_token, user_data));

        if (function_pointer)
        {
            frost_uint8* raw_data = new frost_uint8[data.size()];
            std::copy(data.begin(), data.end(), raw_data);
            frost_uint data_size = static_cast<frost_uint>(data.size());

            function_pointer(user_data, name.c_str(), raw_data, data_size);

            delete[] raw_data;
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

NEBULA_DsmccStreamEventSubscription *
NEBULA_DsmccStreamEventSubscribe(
        const char * event_name,
        NEBULA_DsmccStreamEventCallback callback,
        void * user_data)
{
    bool duplicate = false;
    const auto user_data_token
                    = local_dsmcc_stream_event_user_data.add({callback, user_data}, duplicate);

    const auto stream_event_subscription_token
                    = nebulaRpcCall<RegisteredStreamHandlesConfig::TokenType>(
                                                    IPC_NAME(NEBULA_DsmccStreamEventSubscribe),
                                                    std::string(event_name),
                                                    user_data_token);

    if ((stream_event_subscription_token == RegisteredStreamHandlesConfig::m_invalid_token) &&
        !duplicate)
    {
        if (!local_dsmcc_stream_event_user_data.remove(user_data_token))
        {
            TRACE_ERROR(("%s() Failed to remove user_data_token %u\n", __func__, user_data_token));
        }
    }
    else
    {
        std::lock_guard<std::mutex> lock(local_stream_to_callback_mutex);
        local_stream_to_callback_map[stream_event_subscription_token] = user_data_token;
    }

    return RegisteredStreamHandlesConfig::toHandle<NEBULA_DsmccStreamEventSubscription*>
                    (stream_event_subscription_token);
}

void
NEBULA_DsmccStreamEventUnsubscribe(NEBULA_DsmccStreamEventSubscription * subscription)
{
    const auto stream_event_subscription_token
                    = RegisteredStreamHandlesConfig::toToken(subscription);

    nebulaRpcCall(IPC_NAME(NEBULA_DsmccStreamEventUnsubscribe), stream_event_subscription_token);

    std::lock_guard<std::mutex> lock(local_stream_to_callback_mutex);
    const auto iter = local_stream_to_callback_map.find(stream_event_subscription_token);

    if (iter != local_stream_to_callback_map.end())
    {
        const auto user_token = iter->second;
        if (!local_dsmcc_stream_event_user_data.remove(user_token))
        {
            TRACE_ERROR(("%s() Failed to remove user_token %u\n", __func__, user_token));
        }

        local_stream_to_callback_map.erase(iter);
    }
}

#ifdef __cplusplus
}
#endif

void NebulaStreamEventsCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DsmccStreamEventCallback), proxyDsmccStreamEvent);
}
