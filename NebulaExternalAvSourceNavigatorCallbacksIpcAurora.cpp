/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaExternalAvSourceNavigatorCallbacksIpcAurora class.
 */

#include "NebulaExternalAvSourceNavigatorCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_external_av_source_navigator.h"
#include "nebula_src/adaptation_layer/ipc/NebulaExternalAvSourceNavigatorIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaExternalAvSourceNavigatorCallbacksIpcAurora");

namespace
{
    extern "C" void
    proxyExternalInterfaceStatusChange(frost_int index,
                                       NEBULA_ExternalInterfaceStatus status,
                                       void* user_data)
    {
        TRACE_INFO(("%s()\n", __func__));

        AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
        const bool ok = client.call(IPC_NAME(NEBULA_ExternalInterfaceStatusChangeCallback),
                                    index, status,
                                    RegisteredCallbacksConfig::toToken(user_data));
        if (!ok)
        {
            TRACE_ERROR(("%s() Failed\n", __func__));
        }
    }

    extern "C" void
    proxyExternalSourceListUpdated(void* user_data)
    {
        TRACE_INFO(("%s()\n", __func__));

        AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
        const bool ok = client.call(IPC_NAME(NEBULA_ExternalSourceListUpdatedCallback),
                                    RegisteredCallbacksConfig::toToken(user_data));
        if (!ok)
        {
            TRACE_ERROR(("%s() Failed\n", __func__));
        }
    }
}

void
NebulaExternalAvSourceNavigatorCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterExternalInterfaceStatusCallback),
        [](RegisteredCallbacksConfig::TokenType token)
    {
        NEBULA_RegisterExternalInterfaceStatusCallback(
                proxyExternalInterfaceStatusChange,
                RegisteredCallbacksConfig::toPointer<void*>(token));

        return frost_true;
    });

    server.bind(IPC_NAME(NEBULA_RegisterExternalSourceListUpdatedCallback),
        [](RegisteredCallbacksConfig::TokenType token)
    {
        NEBULA_RegisterExternalSourceListUpdatedCallback(
                proxyExternalSourceListUpdated,
                RegisteredCallbacksConfig::toPointer<void*>(token));

        return frost_true;
    });
}
