/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaChannelScanControllerCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaChannelScanControllerCallbacksIpcExternal.hpp"
#include "NebulaChannelScanControllerCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelScanControllerIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaChannelScanControllerCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<OnScanEventCallback> local_user_data;

// Proxy external callbacks
void
proxyScanEventCallback(MP_NEBULA_ScanEventData event_data,
                       RegisteredCallbacksConfig::TokenType user_data_token)
{
    OnScanEventCallback callback;
    if (local_user_data.find(user_data_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() user_data_token %u, user_data %p \n",
                   __func__, user_data_token, user_data));

        if (function_pointer)
        {
            event_data.channel_id_array = nullptr;
            if (!event_data.packed_channel_id_array.empty())
            {
                event_data.channel_id_array = &event_data.packed_channel_id_array[0];
            }
            function_pointer(&event_data, user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool NEBULA_RegisterScanEventCallback(NEBULA_ScanEventCallback* callback, void* user_data)
{
    return local_user_data.registerCallback({callback, user_data},
                                            IPC_NAME(NEBULA_RegisterScanEventCallback));
}

frost_bool NEBULA_DeregisterScanEventCallback(NEBULA_ScanEventCallback* callback, void* user_data)
{
    return local_user_data.deregisterCallback({callback, user_data},
                                              IPC_NAME(NEBULA_DeregisterScanEventCallback));
}

#ifdef __cplusplus
}
#endif

void NebulaChannelScanControllerCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_ScanEventCallback), proxyScanEventCallback);
}
