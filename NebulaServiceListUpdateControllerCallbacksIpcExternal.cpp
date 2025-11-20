/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaServiceListUpdateControllerCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaServiceListUpdateControllerCallbacksIpcExternal.hpp"
#include "NebulaServiceListUpdateControllerCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaServiceListUpdateControllerIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaServiceListUpdateControllerCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<ServiceListUpdateCallback> local_user_data;

// Proxy external callbacks
void
proxyServiceListUpdateCallback(RegisteredCallbacksConfig::TokenType user_data_token)
{
    ServiceListUpdateCallback callback;
    if (local_user_data.find(user_data_token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() user_data_token %u, user_data %p \n",
                   __func__, user_data_token, user_data));

        if (function_pointer)
        {
            function_pointer(user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool
NEBULA_RegisterServiceListUpdateCallback(NEBULA_ServiceListUpdateCallback callback, void* user_data)
{
    return local_user_data.registerCallback({callback, user_data},
                                            IPC_NAME(NEBULA_RegisterServiceListUpdateCallback));
}

frost_bool
NEBULA_DeregisterServiceListUpdateCallback(NEBULA_ServiceListUpdateCallback callback, void* user_data)
{
    return local_user_data.deregisterCallback({callback, user_data},
                                            IPC_NAME(NEBULA_DeregisterServiceListUpdateCallback));
}

#ifdef __cplusplus
}
#endif

void NebulaServiceListUpdateControllerCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_ServiceListUpdateCallback), proxyServiceListUpdateCallback);
}
