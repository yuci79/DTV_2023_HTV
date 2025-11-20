/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_mheg.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAMHEGIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAMHEGIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_MhegEnable);
DECLARE_IPC_NAME(NEBULA_MhegIsEnabled);
DECLARE_IPC_NAME(NEBULA_MhegResume);
DECLARE_IPC_NAME(NEBULA_MhegSuspend);

#endif
