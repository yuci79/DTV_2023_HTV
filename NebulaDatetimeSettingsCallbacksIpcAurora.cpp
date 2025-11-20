/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaDatetimeSettingsCallbacksIpcAurora class.
 */

#include "NebulaDatetimeSettingsCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_datetime_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaDatetimeSettingsIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"

#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaDatetimeSettingsCallbacksIpcAurora");

bool NebulaDatetimeSettingsCallbacksIpcAurora::m_registered_callback = false;

namespace
{
extern "C"
void proxyOnTimeUpdatedEvent(void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_OnTimeUpdatedEvent), 
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

}

void
NebulaDatetimeSettingsCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DatetimeSettingsRegisterTimeUpdatedEventCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        const auto result = NEBULA_DatetimeSettingsRegisterTimeUpdatedEventCallback(
                    proxyOnTimeUpdatedEvent,
                    RegisteredCallbacksConfig::toPointer<void*>(token));

        if (result == frost_true)
        {
            m_registered_callback = true;
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsDeregisterTimeUpdatedEventCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        const auto result = NEBULA_DatetimeSettingsDeregisterTimeUpdatedEventCallback(
                    proxyOnTimeUpdatedEvent,
                    RegisteredCallbacksConfig::toPointer<void*>(token));

        if (result == frost_true)
        {
            m_registered_callback = false;
        }

        return result;
    });
}

void
NebulaDatetimeSettingsCallbacksIpcAurora::reset()
{
    if (m_registered_callback)
    {
        NEBULA_DatetimeSettingsDeregisterAllFromTimeUpdatedEventCallback(proxyOnTimeUpdatedEvent);
        m_registered_callback = false;
    }
}
