/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_ca_controller.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACACONTROLLERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACACONTROLLERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_ca_controller.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_HasCiPlusSupport);
DECLARE_IPC_NAME(NEBULA_GetCASystemIds);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_CAStackType);

#endif
