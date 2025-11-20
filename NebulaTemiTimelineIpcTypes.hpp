/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_temi_timeline.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATEMITIMELINEIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATEMITIMELINEIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_temi_timeline.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_SelectTemiTimeline);
DECLARE_IPC_NAME(NEBULA_GetTemiTimestamp);
DECLARE_IPC_NAME(NEBULA_GetPtsTimestamp);
DECLARE_IPC_NAME(NEBULA_extractTimelineType);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_TimelineType);

struct MP_NEBULA_TemiTimestamp : public NEBULA_TemiTimestamp
{
    MEMBER_CHECKED_MSGPACK_DEFINE(
                   NEBULA_TemiTimestamp,
                   system_usec,
                   current_pts, 
                   timeline_pts,
                   media_timestamp,
                   timescale);
};

#endif