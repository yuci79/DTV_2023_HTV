/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaDatetimeSettingsCallbacksIpcExternal class and some associated
 *        functions.
 */

#include "NebulaDatetimeSettingsCallbacksIpcExternal.hpp"
#include "NebulaDatetimeSettingsCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaDatetimeSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaDatetimeSettingsCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers.
RegisteredCallbacks<OnTimeUpdatedEventCallback> local_time_updated_event_register;


// Proxy external callbacks.
void
proxyOnTimeUpdatedEvent(RegisteredCallbacksConfig::TokenType token)
{

    OnTimeUpdatedEventCallback callback;
    if (local_time_updated_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(user_data);
        }
    }
}
}

frost_bool
NEBULA_DatetimeSettingsRegisterTimeUpdatedEventCallback(
                                        NEBULA_TimeUpdatedEventCallback* callback, void* user_data)
{
    return local_time_updated_event_register.registerCallback({callback, user_data},
                            IPC_NAME(NEBULA_DatetimeSettingsRegisterTimeUpdatedEventCallback));
}

frost_bool
NEBULA_DatetimeSettingsDeregisterTimeUpdatedEventCallback(
                                        NEBULA_TimeUpdatedEventCallback* callback, void* user_data)
{
    return local_time_updated_event_register.deregisterCallback({callback, user_data},
                            IPC_NAME(NEBULA_DatetimeSettingsDeregisterTimeUpdatedEventCallback));
}

void
NebulaDatetimeSettingsCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_OnTimeUpdatedEvent), proxyOnTimeUpdatedEvent);
}
