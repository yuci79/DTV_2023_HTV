/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_av_control.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaAvControlIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_SetVideoPosition(const NEBULA_DisplayWindow* output_window,
                           frost_bool is_browser_video,
                           frost_bool is_broadcast)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetVideoPosition),
                                     MP_NEBULA_DisplayWindow(*output_window),
                                     is_browser_video,
                                     is_broadcast);
}

extern "C" frost_bool
NEBULA_CalculateVideoPosition(const NEBULA_DisplayWindow* window,
                                  frost_bool is_browser_video,
                                  NEBULA_DisplayWindow* output_window)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_DisplayWindow>(
            *output_window,
            IPC_NAME(NEBULA_CalculateVideoPosition),
            MP_NEBULA_DisplayWindow(*window),
            is_browser_video);
}

extern "C" NEBULA_DisplayWindow
NEBULA_GetVideoPosition(void)
{
    return nebulaRpcCall<MP_NEBULA_DisplayWindow>(IPC_NAME(NEBULA_GetVideoPosition));
}

extern "C" unsigned int
NEBULA_GetVideoDecodeWidth()
{
    return nebulaRpcCall<unsigned int>(IPC_NAME(NEBULA_GetVideoDecodeWidth));
}

extern "C" unsigned int
NEBULA_GetVideoDecodeHeight()
{
    return nebulaRpcCall<unsigned int>(IPC_NAME(NEBULA_GetVideoDecodeHeight));
}

extern "C" NEBULA_VideoAspectRatio
NEBULA_GetVideoAspectRatio(void)
{
    return nebulaRpcCall<NEBULA_VideoAspectRatio>(IPC_NAME(NEBULA_GetVideoAspectRatio));
}

extern "C" frost_bool
NEBULA_GetVideoExactAspectRatio(float &aspect_ratio)
{
    return nebulaRpcCall<frost_bool, float>(aspect_ratio,
                                            IPC_NAME(NEBULA_GetVideoExactAspectRatio));
}

extern "C" frost_bool
NEBULA_SetVolumeLevel(frost_int level, frost_bool is_slave)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetVolumeLevel), level, is_slave);
}

extern "C" frost_int
NEBULA_GetVolumeLevel(frost_bool is_slave)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetVolumeLevel), is_slave);
}

extern "C" frost_bool
NEBULA_SetVideoVisible(frost_bool visible)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetVideoVisible), visible);
}

extern "C" frost_bool
NEBULA_AllocateAvResources(frost_bool is_broadcast)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AllocateAvResources), is_broadcast);
}

extern "C" frost_bool
NEBULA_ReleaseAvResources(frost_bool is_broadcast)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_ReleaseAvResources), is_broadcast);
}

extern "C" void
NEBULA_SetBroadcastVideoPresenting(frost_bool presenting)
{
    nebulaRpcCall(IPC_NAME(NEBULA_SetBroadcastVideoPresenting), presenting);
}

extern "C" frost_bool
NEBULA_IsBroadcastVideoPresenting(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsBroadcastVideoPresenting));
}
