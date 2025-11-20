/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_timeshift.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATIMESHIFTIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATIMESHIFTIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_timeshift.h"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_RecordNow)
DECLARE_IPC_NAME(NEBULA_StopRecording)
DECLARE_IPC_NAME(NEBULA_Pause)
DECLARE_IPC_NAME(NEBULA_Resume)
DECLARE_IPC_NAME(NEBULA_SetSpeed)
DECLARE_IPC_NAME(NEBULA_Seek)
DECLARE_IPC_NAME(NEBULA_StopTimeshift)
DECLARE_IPC_NAME(NEBULA_GetPlaybackOffset)
DECLARE_IPC_NAME(NEBULA_GetMaxOffset)
DECLARE_IPC_NAME(NEBULA_GetPlayPosition)
DECLARE_IPC_NAME(NEBULA_GetPlaySpeed)
DECLARE_IPC_NAME(NEBULA_GetRecordingState)

DECLARE_IPC_NAME(NEBULA_RegisterToPlayPositionChangedEvent)
DECLARE_IPC_NAME(NEBULA_DeregisterFromPlayPositionChangedEvent)
DECLARE_IPC_NAME(NEBULA_OnPlayPositionChangedEvent)

DECLARE_IPC_NAME(NEBULA_RegisterToPlaySpeedChangedEvent)
DECLARE_IPC_NAME(NEBULA_DeregisterFromPlaySpeedChangedEvent)
DECLARE_IPC_NAME(NEBULA_OnPlaySpeedChangedEvent)

DECLARE_IPC_NAME(NEBULA_RegisterToRecordingEvent)
DECLARE_IPC_NAME(NEBULA_DeregisterFromRecordingEvent)
DECLARE_IPC_NAME(NEBULA_OnRecordingEvent)

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_PvrStatus);

#endif