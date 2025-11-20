/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaExternalAvSourceNavigatorCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaExternalAvSourceNavigatorCallbacksIpcExternal.hpp"
#include "NebulaExternalAvSourceNavigatorCallbackIpcTypesExternal.hpp"
#include "NebulaIpcHelper.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaExternalAvSourceNavigatorIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaExternalAvSourceNavigatorCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
    // Local callback registers
    RegisteredCallbacks<ExternalInterfaceStatusChangeCallback>
        local_external_interface_status_change_register;
    RegisteredCallbacks<ExternalSourceListUpdatedCallback>
        local_external_source_list_updated_register;

    // Proxy external callbacks
    void
    proxyExternalInterfaceStatusChangeCallback(frost_int index,
                                               NEBULA_ExternalInterfaceStatus status,
                                               RegisteredCallbacksConfig::TokenType token)
    {
        ExternalInterfaceStatusChangeCallback callback;
        if (local_external_interface_status_change_register.find(token, callback))
        {
            void* user_data = callback.m_user_data;
            auto function_pointer = callback.m_function_pointer;
            TRACE_INFO(("%s() token %u, user_data %p\n", __func__, token, user_data));

            if (function_pointer)
            {
                function_pointer(index, status, user_data);
            }
        }
    }

    void
    proxyExternalSourceListUpdatedCallback(RegisteredCallbacksConfig::TokenType token)
    {
        ExternalSourceListUpdatedCallback callback;
        if (local_external_source_list_updated_register.find(token, callback))
        {
            void* user_data = callback.m_user_data;
            auto function_pointer = callback.m_function_pointer;
            TRACE_INFO(("%s() token %u, user_data %p\n", __func__, token, user_data));

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

void NEBULA_RegisterExternalInterfaceStatusCallback(
                       NEBULA_ExternalInterfaceStatusChangeCallback function_pointer,
                       void* user_data)
{
    local_external_interface_status_change_register.registerCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_RegisterExternalInterfaceStatusCallback));
}

void NEBULA_RegisterExternalSourceListUpdatedCallback(
                       NEBULA_ExternalSourceListUpdatedCallBack function_pointer,
                       void* user_data)
{
    // registerCallback will replace existing callback in aurora so replicate that here as well
    local_external_source_list_updated_register.removeAll();
    local_external_source_list_updated_register.registerCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_RegisterExternalSourceListUpdatedCallback));
}

#ifdef __cplusplus
}
#endif

void NebulaExternalAvSourceNavigatorCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_ExternalInterfaceStatusChangeCallback),
                &proxyExternalInterfaceStatusChangeCallback);

    server.bind(IPC_NAME(NEBULA_ExternalSourceListUpdatedCallback),
                &proxyExternalSourceListUpdatedCallback);
}
