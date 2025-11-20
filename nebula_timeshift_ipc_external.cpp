/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_timeshift.
 */

#include "nebula/adaptation_layer/nebula_timeshift.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTimeshiftIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_bool
NEBULA_RecordNow(int duration, char **retval)
{
    std::string id;
    const auto result = nebulaRpcCall<frost_bool, std::string>(id,
                                                               IPC_NAME(NEBULA_RecordNow),
                                                               duration);
    if (result == frost_true)
    {
        *retval = strdup(id.c_str());
    }
    return result;
}

frost_bool
NEBULA_StopRecording(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StopRecording));
}

frost_bool
NEBULA_Pause(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_Pause));
}

frost_bool
NEBULA_Resume(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_Resume));
}

frost_bool
NEBULA_SetSpeed(int speed)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetSpeed), speed);
}

frost_bool
NEBULA_Seek(int offset, int reference)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_Seek), offset, reference);
}

frost_bool
NEBULA_StopTimeshift(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StopTimeshift));
}

frost_int
NEBULA_GetPlaybackOffset(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetPlaybackOffset));
}

frost_int
NEBULA_GetMaxOffset(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetMaxOffset));
}

frost_int
NEBULA_GetPlayPosition(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetPlayPosition));
}

frost_int
NEBULA_GetPlaySpeed(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetPlaySpeed));
}

NEBULA_PvrStatus
NEBULA_GetRecordingState(void)
{
    return nebulaRpcCall<NEBULA_PvrStatus>(IPC_NAME(NEBULA_GetRecordingState));
}