/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_channel_change.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELCHANGEIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELCHANGEIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_SelectChannel);
DECLARE_IPC_NAME(NEBULA_StopCurrentChannel);
DECLARE_IPC_NAME(NEBULA_SelectIPChannel);
DECLARE_IPC_NAME(NEBULA_RegisterToChannelChangeStatus);
DECLARE_IPC_NAME(NEBULA_DeregisterFromChannelChangeStatus);
DECLARE_IPC_NAME(NEBULA_OnChannelChange);
DECLARE_IPC_NAME(NEBULA_RegisterToTunerStatus);
DECLARE_IPC_NAME(NEBULA_DeregisterFromTunerStatus);
DECLARE_IPC_NAME(NEBULA_OnTunerStatusChange);
DECLARE_IPC_NAME(NEBULA_RegisterToComponentChangeEvent);
DECLARE_IPC_NAME(NEBULA_DeregisterFromComponentChangeEvent);
DECLARE_IPC_NAME(NEBULA_OnComponentChange);


/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_ChannelChangeMethod);
MSGPACK_ADD_ENUM(NEBULA_ChannelChangeQuietValue);
MSGPACK_ADD_ENUM(NEBULA_SelectChannelResult);
MSGPACK_ADD_ENUM(NEBULA_TunerStatus);
MSGPACK_ADD_ENUM(NEBULA_ChannelChangeResult);

#endif
