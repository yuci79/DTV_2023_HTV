/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_av_control.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAVCONTROLIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAVCONTROLIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_av_control.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_SetVideoPosition);
DECLARE_IPC_NAME(NEBULA_CalculateVideoPosition);
DECLARE_IPC_NAME(NEBULA_GetVideoPosition);
DECLARE_IPC_NAME(NEBULA_GetVideoDecodeWidth);
DECLARE_IPC_NAME(NEBULA_GetVideoDecodeHeight);
DECLARE_IPC_NAME(NEBULA_GetVideoAspectRatio);
DECLARE_IPC_NAME(NEBULA_GetVideoExactAspectRatio);
DECLARE_IPC_NAME(NEBULA_SetVolumeLevel);
DECLARE_IPC_NAME(NEBULA_GetVolumeLevel);
DECLARE_IPC_NAME(NEBULA_SetVideoVisible);
DECLARE_IPC_NAME(NEBULA_AllocateAvResources);
DECLARE_IPC_NAME(NEBULA_ReleaseAvResources);
DECLARE_IPC_NAME(NEBULA_SetBroadcastVideoPresenting);
DECLARE_IPC_NAME(NEBULA_IsBroadcastVideoPresenting);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_VideoAspectRatio);

struct MP_NEBULA_DisplayWindow : public NEBULA_DisplayWindow
{
    MP_NEBULA_DisplayWindow() = default;

    MP_NEBULA_DisplayWindow(const NEBULA_DisplayWindow& other)
        : NEBULA_DisplayWindow (other)
    {
    }

    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_DisplayWindow,
                   x_position,
                   y_position,
                   width,
                   height);
};

#endif
