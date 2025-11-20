/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaChannelChangeCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaChannelChangeCallbacksIpcExternal.hpp"
#include "NebulaChannelChangeCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelChangeIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaChannelChangeCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<OnChannelChangeCallback> local_channel_change_register;
RegisteredCallbacks<OnTunerStatusChangeCallback> local_tuner_status_change_register;
RegisteredCallbacks<OnComponentChangeCallback> local_component_change_register;

// Proxy external callbacks
void
proxyOnChannelChange(NEBULA_Channel_ID channel_id,
                     NEBULA_Tuner_ID tuner_id,
                     NEBULA_ChannelChangeResult result,
                     frost_bool keep_app_running,
                     RegisteredCallbacksConfig::TokenType token,
                     frost_bool generated_artificially)
{
    OnChannelChangeCallback callback;
    if (local_channel_change_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(channel_id, tuner_id,result, keep_app_running,
                             user_data, generated_artificially);
        }
    }
}

void proxyOnTunerStatusChange(NEBULA_Tuner_ID const tuner_id,
                              NEBULA_TunerStatus const tuner_status,
                              RegisteredCallbacksConfig::TokenType token)
{
    OnTunerStatusChangeCallback callback;
    if (local_tuner_status_change_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(tuner_id, tuner_status, user_data);
        }
    }
}

void proxyOnComponentChange(RegisteredCallbacksConfig::TokenType token)
{
    OnComponentChangeCallback callback;
    if (local_component_change_register.find(token, callback))
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

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool NEBULA_RegisterToChannelChangeStatus(NEBULA_OnChannelChange function_pointer,
                                                void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
    return local_channel_change_register.registerCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_RegisterToChannelChangeStatus));
}

frost_bool NEBULA_DeregisterFromChannelChangeStatus(NEBULA_OnChannelChange function_pointer,
                                                   void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
    return local_channel_change_register.deregisterCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_DeregisterFromChannelChangeStatus));
}

frost_bool
NEBULA_RegisterToTunerStatus(NEBULA_OnTunerStatusChange function_pointer,
                             void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
   return local_tuner_status_change_register.registerCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_RegisterToTunerStatus));
}                             

frost_bool
NEBULA_DeregisterFromTunerStatus(NEBULA_OnTunerStatusChange function_pointer,
                             void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
   return local_tuner_status_change_register.deregisterCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_DeregisterFromTunerStatus));
}

frost_bool NEBULA_RegisterToComponentChangeEvent(NEBULA_OnComponentChange function_pointer,
                                                 void* user_data)
{
   TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
    return local_component_change_register.registerCallback(
                            {function_pointer, user_data},
                            IPC_NAME(NEBULA_RegisterToComponentChangeEvent));
}

frost_bool NEBULA_DeregisterFromComponentChangeEvent(NEBULA_OnComponentChange function_pointer,
                                                     void* user_data)
{
    TRACE_INFO(("%s() function_pointer %p, user_data %p \n", __func__, 
                function_pointer, user_data));
    return local_component_change_register.deregisterCallback(
                              {function_pointer, user_data},
                              IPC_NAME(NEBULA_DeregisterFromComponentChangeEvent));
}

#ifdef __cplusplus
}
#endif

void NebulaChannelChangeCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_OnChannelChange), proxyOnChannelChange);
    server.bind(IPC_NAME(NEBULA_OnTunerStatusChange), proxyOnTunerStatusChange);
    server.bind(IPC_NAME(NEBULA_OnComponentChange), proxyOnComponentChange);
}
