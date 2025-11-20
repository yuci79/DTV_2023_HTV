/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaNetworkSettingsCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaNetworkSettingsCallbacksIpcExternal.hpp"
#include "NebulaNetworkSettingsCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaNetworkSettingsIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaNetworkSettingsCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<NetworkSettingsEventCallback> local_user_data;

// Proxy external callbacks
void
proxyNetworkSettingsEventCallback(MP_NEBULA_NetworkSettingsEventData event_data,
                                  RegisteredCallbacksConfig::TokenType user_data_token)
{
    NetworkSettingsEventCallback callback;
    if (local_user_data.find(user_data_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() user_data_token %u, user_data %p \n",
                   __func__, user_data_token, user_data));

        if (function_pointer)
        {
            function_pointer(&event_data, user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool
NEBULA_RegisterNetworkSettingsEventCallback(NEBULA_NetworkSettingsEventCallback* callback,
                                            void* user_data)
{
    return local_user_data.registerCallback({callback, user_data},
                                            IPC_NAME(NEBULA_RegisterNetworkSettingsEventCallback));
}

frost_bool
NEBULA_DeregisterNetworkSettingsEventCallback(NEBULA_NetworkSettingsEventCallback* callback,
                                             void* user_data)
{
    return local_user_data.deregisterCallback({callback, user_data},
                                            IPC_NAME(NEBULA_DeregisterNetworkSettingsEventCallback));
}

#ifdef __cplusplus
}
#endif

void NebulaNetworkSettingsCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_NetworkSettingsEventCallback), proxyNetworkSettingsEventCallback);
}