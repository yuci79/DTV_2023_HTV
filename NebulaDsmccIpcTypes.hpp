/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_dsmcc.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULADSMCCIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULADSMCCIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_dsmcc.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_DsmccMountCarousel);
DECLARE_IPC_NAME(NEBULA_DsmccStreamOpen);
DECLARE_IPC_NAME(NEBULA_DsmccStreamClose);
DECLARE_IPC_NAME(NEBULA_DsmccStreamGetComponentTag);
DECLARE_IPC_NAME(NEBULA_DsmccStreamGetEventCount);
DECLARE_IPC_NAME(NEBULA_DsmccStreamGetEvents);

DECLARE_IPC_NAME(NEBULA_DsmccStreamAvailableCallback);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_DsmccCarouselSelection);

#endif
