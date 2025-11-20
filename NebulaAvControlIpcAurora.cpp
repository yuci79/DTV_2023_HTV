/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaAvControlIpcAurora class.
 */

#include "NebulaAvControlIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaAvControlIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaAvControlIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_av_control 'C' functions.
    server.bind(IPC_NAME(NEBULA_SetVideoPosition), [](MP_NEBULA_DisplayWindow output_window,
                                                      frost_bool is_browser_video,
                                                      frost_bool is_broadcast)
    {
        return NEBULA_SetVideoPosition(&output_window, is_browser_video, is_broadcast);
    });
    server.bind(IPC_NAME(NEBULA_CalculateVideoPosition), [](MP_NEBULA_DisplayWindow window,
                                                            frost_bool is_browser_video)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_DisplayWindow> result;
        result.return_value = NEBULA_CalculateVideoPosition(&window,
                                                            is_browser_video,
                                                            &result.output_params);
        return result;
    });
    server.bind(IPC_NAME(NEBULA_GetVideoPosition), []() {
        return MP_NEBULA_DisplayWindow (NEBULA_GetVideoPosition());
    });
    server.bind(IPC_NAME(NEBULA_GetVideoDecodeWidth), &NEBULA_GetVideoDecodeWidth);
    server.bind(IPC_NAME(NEBULA_GetVideoDecodeHeight), &NEBULA_GetVideoDecodeHeight);
    server.bind(IPC_NAME(NEBULA_GetVideoAspectRatio), &NEBULA_GetVideoAspectRatio);
    server.bind(IPC_NAME(NEBULA_GetVideoExactAspectRatio), []()
    {
        NebulaIpcType<frost_bool, float> result;
        result.return_value = NEBULA_GetVideoExactAspectRatio(result.output_params);
        return result;
    });
    server.bind(IPC_NAME(NEBULA_SetVolumeLevel), &NEBULA_SetVolumeLevel);
    server.bind(IPC_NAME(NEBULA_GetVolumeLevel), &NEBULA_GetVolumeLevel);
    server.bind(IPC_NAME(NEBULA_SetVideoVisible), &NEBULA_SetVideoVisible);
    server.bind(IPC_NAME(NEBULA_AllocateAvResources), &NEBULA_AllocateAvResources);
    server.bind(IPC_NAME(NEBULA_ReleaseAvResources), &NEBULA_ReleaseAvResources);
    server.bind(IPC_NAME(NEBULA_SetBroadcastVideoPresenting), &NEBULA_SetBroadcastVideoPresenting);
    server.bind(IPC_NAME(NEBULA_IsBroadcastVideoPresenting), &NEBULA_IsBroadcastVideoPresenting);
}
