/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaDsmccCallbacksIpcAurora class.
 */

#include "NebulaDsmccCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_dsmcc.h"
#include "nebula_src/adaptation_layer/ipc/NebulaDsmccIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaStreamEventsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaDsmccCallbacksIpcAurora");

namespace
{
using StreamHandle = NEBULA_DsmccStreamHandle*;
RegisteredStreamHandles<StreamHandle> local_stream_handles;

extern "C"
void
proxyOnDsmccStreamAvailable(void* user_data,
                            StreamHandle stream_handle,
                            frost_bool succeeded)
{
    TRACE_INFO(("%s()\n", __func__));

    bool ok = false;

    RegisteredStreamHandlesConfig::TokenType
         stream_token = RegisteredStreamHandlesConfig::m_invalid_token;
    if (local_stream_handles.find(stream_token, 
        [stream_handle](const StreamHandle& data)
        {
            return stream_handle == data;
        }))
    {
        AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
        ok = client.call(IPC_NAME(NEBULA_DsmccStreamAvailableCallback),
                         RegisteredCallbacksConfig::toToken(user_data),
                         stream_token,
                         succeeded);
    }

    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

}

void
NebulaDsmccCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DsmccStreamOpen),
                [](const std::string& full_path, RegisteredCallbacksConfig::TokenType token)
    {
        const auto stream_handle = 
            NEBULA_DsmccStreamOpen(full_path.c_str(),
                                   proxyOnDsmccStreamAvailable, 
                                   RegisteredCallbacksConfig::toPointer<void*>(token));

        auto stream_token = RegisteredStreamHandlesConfig::m_invalid_token;

        if (stream_handle)
        {
            bool duplicate = false;
            stream_token = local_stream_handles.add(stream_handle, duplicate);

            if (duplicate)
            {
                // NEBULA_DsmccStreamOpen always generates a new stream handle. So duplicate
                // should always be false.
                SHOULD_NOT_BE_HERE();
            }
        }

        return stream_token;
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamClose), 
                [](RegisteredStreamHandlesConfig::TokenType stream_token)
    {
        StreamHandle stream_handle;
        if (local_stream_handles.find(stream_token, stream_handle))
        {
            NEBULA_DsmccStreamClose(stream_handle);

            // Assume that the stream handle is no longer valid.
            if (!local_stream_handles.remove(stream_token))
            {
                SHOULD_NOT_BE_HERE();
            }
        }
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamGetComponentTag),
                [](RegisteredStreamHandlesConfig::TokenType stream_token)
    {
        frost_int result = -1;

        StreamHandle stream_handle;
        if (local_stream_handles.find(stream_token, stream_handle))
        {
            result = NEBULA_DsmccStreamGetComponentTag(stream_handle);
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamGetEventCount),
                [](RegisteredStreamHandlesConfig::TokenType stream_token)
    {
        frost_int result = 0;

        StreamHandle stream_handle;
        if (local_stream_handles.find(stream_token, stream_handle))
        {
            result = NEBULA_DsmccStreamGetEventCount(stream_handle);
        }
        
        return result;
    });

    server.bind(IPC_NAME(NEBULA_DsmccStreamGetEvents),
        [](RegisteredStreamHandlesConfig::TokenType stream_token)
    {
        NebulaIpcStreamEvents result;

        StreamHandle stream_handle;
        if (local_stream_handles.find(stream_token, stream_handle))
        {
            const auto stream_events = NEBULA_DsmccStreamGetEvents(stream_handle);
            const auto events_count = NEBULA_DsmccStreamGetEventCount(stream_handle);

            for (frost_int i = 0; i < events_count; ++i)
            {
                result.emplace_back(stream_events[i].m_id, stream_events[i].m_name);
            }
        }

        return result;
    });
}
