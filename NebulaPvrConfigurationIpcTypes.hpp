/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_pvr_configuration.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRCONFIGURATIONIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRCONFIGURATIONIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_PvrGetStartPadding);
DECLARE_IPC_NAME(NEBULA_PvrSetStartPadding);
DECLARE_IPC_NAME(NEBULA_PvrGetEndPadding);
DECLARE_IPC_NAME(NEBULA_PvrSetEndPadding);

#endif
