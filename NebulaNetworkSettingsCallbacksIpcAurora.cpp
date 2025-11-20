/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaNetworkSettingsCallbacksIpcAurora class.
 */

#include "NebulaNetworkSettingsCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_network_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaNetworkSettingsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaNetworkSettingsCallbacksIpcAurora");

bool NebulaNetworkSettingsCallbacksIpcAurora::m_registered_callback = false;

namespace
{

extern "C"
void proxyNetworkSettingsEventCallback(const NEBULA_NetworkSettingsEventData* event_data,
                                       void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));
                
    if (event_data)
    {
        AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);

        const MP_NEBULA_NetworkSettingsEventData ipc_event_data(*event_data);

        client.call(IPC_NAME(NEBULA_NetworkSettingsEventCallback),
                ipc_event_data,
                RegisteredCallbacksConfig::toToken(user_data));
    }
}

}

void
NebulaNetworkSettingsCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterNetworkSettingsEventCallback),
                [](RegisteredCallbacksConfig::TokenType user_data_token)
    {
        const auto result = NEBULA_RegisterNetworkSettingsEventCallback(
                                    proxyNetworkSettingsEventCallback,
                                    RegisteredCallbacksConfig::toPointer<void*>(user_data_token));

        if (result == frost_true)
        {
            m_registered_callback = true;
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_DeregisterNetworkSettingsEventCallback),
                [](RegisteredCallbacksConfig::TokenType user_data_token)
    {
        const auto result = NEBULA_DeregisterNetworkSettingsEventCallback(
                                    proxyNetworkSettingsEventCallback,
                                    RegisteredCallbacksConfig::toPointer<void*>(user_data_token));

        if (result == frost_true)
        {
            m_registered_callback = false;
        }

        return result;
    });
}

void 
NebulaNetworkSettingsCallbacksIpcAurora::reset()
{
    if (m_registered_callback)
    {
        NEBULA_DeregisterAllFromNetworkSettingsEventCallback(proxyNetworkSettingsEventCallback);
        m_registered_callback = false;
    }
}
