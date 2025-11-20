/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaTimeshiftIpcAurora class.
 */

#include "NebulaTimeshiftIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaTimeshiftIpcTypes.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_timeshift.h"

#include <rpc/server.h>

void
NebulaTimeshiftIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_system_information_provider 'C' functions.

    server.bind(IPC_NAME(NEBULA_RecordNow), [](int duration){
        NebulaIpcType<frost_bool, std::string> result;
        char* recording_id = nullptr;
        result.return_value = NEBULA_RecordNow(duration, &recording_id);
        if (result.return_value == frost_true)
        {
            result.output_params = recording_id;
            free(recording_id);
        }
        return result;
    });

    server.bind(IPC_NAME(NEBULA_StopRecording), &NEBULA_StopRecording);

    server.bind(IPC_NAME(NEBULA_Pause), &NEBULA_Pause);

    server.bind(IPC_NAME(NEBULA_Resume), &NEBULA_Resume);

    server.bind(IPC_NAME(NEBULA_SetSpeed), &NEBULA_SetSpeed);

    server.bind(IPC_NAME(NEBULA_Seek), &NEBULA_Seek);

    server.bind(IPC_NAME(NEBULA_StopTimeshift), &NEBULA_StopTimeshift);
                
    server.bind(IPC_NAME(NEBULA_GetPlaybackOffset), &NEBULA_GetPlaybackOffset);
                
    server.bind(IPC_NAME(NEBULA_GetMaxOffset), &NEBULA_GetMaxOffset);
                
    server.bind(IPC_NAME(NEBULA_GetPlayPosition), &NEBULA_GetPlayPosition);
                
    server.bind(IPC_NAME(NEBULA_GetPlaySpeed), &NEBULA_GetPlaySpeed);
                
    server.bind(IPC_NAME(NEBULA_GetRecordingState), &NEBULA_GetRecordingState);
}
