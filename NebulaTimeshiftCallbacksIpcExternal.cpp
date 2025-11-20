/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for the NebulaTimeshiftCallbacksIpcExternal class 
 *        and some associated functions.
 */
#include "NebulaTimeshiftCallbacksIpcExternal.hpp"
#include "NebulaTimeshiftCallbackIpcTypesExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaTimeshiftIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_timeshift.h"
#include "RegisteredCallbacks.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaTimeshiftCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<OnPlayPositionChangedEvent> local_play_position_changed_event_register;
RegisteredCallbacks<OnPlaySpeedChangedEvent> local_play_speed_changed_event_register;
RegisteredCallbacks<OnRecordingEvent> local_recording_event_register;

// Proxy external callbacks
void
proxyOnRecordingEvent(frost_int32 rec_state,
                      frost_int32 rec_error,
                      const std::string& recording_id,
                      RegisteredCallbacksConfig::TokenType token)
{
    OnRecordingEvent callback;
    if (local_recording_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            (*function_pointer)(rec_state,
                                rec_error,
                                const_cast<char*>(recording_id.c_str()),
                                user_data);
        }
    }
}

void
proxyOnPlaySpeedChangedEvent(frost_int32 speed,
                             RegisteredCallbacksConfig::TokenType token)
{
    OnPlaySpeedChangedEvent callback;
    if (local_play_speed_changed_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            (*function_pointer)(speed, user_data);
        }
    }
}

void
proxyOnPlayPositionChangedEvent(frost_int32 position,
                                RegisteredCallbacksConfig::TokenType token)
{
    OnPlayPositionChangedEvent callback;
    if (local_play_position_changed_event_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            (*function_pointer)(position, user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool NEBULA_RegisterToPlayPositionChangedEvent(NEBULA_OnPlayPositionChangedEvent callback,
                                                     void* user_data)
{
    return local_play_position_changed_event_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_RegisterToPlayPositionChangedEvent));
}

frost_bool NEBULA_DeregisterFromPlayPositionChangedEvent(NEBULA_OnPlayPositionChangedEvent callback,
                                                         void* user_data)
{
    return local_play_position_changed_event_register.deregisterCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_DeregisterFromPlayPositionChangedEvent));
}

frost_bool NEBULA_RegisterToPlaySpeedChangedEvent(NEBULA_OnPlaySpeedChangedEvent callback,
                                                  void* user_data)
{
    return local_play_speed_changed_event_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_RegisterToPlaySpeedChangedEvent));
}

frost_bool NEBULA_DeregisterFromPlaySpeedChangedEvent(NEBULA_OnPlaySpeedChangedEvent callback,
                                                      void* user_data)
{
    return local_play_speed_changed_event_register.deregisterCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_DeregisterFromPlaySpeedChangedEvent));
}

frost_bool NEBULA_RegisterToRecordingEvent(NEBULA_OnRecordingEvent callback,
                                           void* user_data)
{
    return local_recording_event_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_RegisterToRecordingEvent));
}

frost_bool NEBULA_DeregisterFromRecordingEvent(NEBULA_OnRecordingEvent callback,
                                               void* user_data)
{
    return local_recording_event_register.deregisterCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_DeregisterFromRecordingEvent));
}
                                               
#ifdef __cplusplus
}
#endif

void NebulaTimeshiftCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_OnPlayPositionChangedEvent), proxyOnPlayPositionChangedEvent);
    server.bind(IPC_NAME(NEBULA_OnPlaySpeedChangedEvent), proxyOnPlaySpeedChangedEvent);
    server.bind(IPC_NAME(NEBULA_OnRecordingEvent), proxyOnRecordingEvent);
}
