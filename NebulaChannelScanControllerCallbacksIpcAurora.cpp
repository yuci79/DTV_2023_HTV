/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaChannelScanControllerCallbacksIpcAurora class.
 */

#include "NebulaChannelScanControllerCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_channel_scan_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelScanControllerIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaChannelScanControllerCallbacksIpcAurora");

bool NebulaChannelScanControllerCallbacksIpcAurora::m_registered_callback = false;

namespace
{
    
extern "C"
void proxyOnScanEventCallback(NEBULA_ScanEventData* event_data, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_ScanEventCallback), 
                                MP_NEBULA_ScanEventData(*event_data),
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

}

void
NebulaChannelScanControllerCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterScanEventCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        const auto result = NEBULA_RegisterScanEventCallback(
                            proxyOnScanEventCallback,
                            RegisteredCallbacksConfig::toPointer<void*>(token));

        if (result == frost_true)
        {
            m_registered_callback = true;
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_DeregisterScanEventCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        const auto result = NEBULA_DeregisterScanEventCallback(
                            proxyOnScanEventCallback,
                            RegisteredCallbacksConfig::toPointer<void*>(token));

        if (result == frost_true)
        {
            m_registered_callback = false;
        }

        return result;
    });
}

void
NebulaChannelScanControllerCallbacksIpcAurora::reset()
{
    if (m_registered_callback)
    {
        NEBULA_DeregisterAllFromScanEventCallback(proxyOnScanEventCallback);
        m_registered_callback = false;
    }
}