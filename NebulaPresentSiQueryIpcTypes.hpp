/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_present_si_query.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPRESENTSIQUERYIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPRESENTSIQUERYIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_present_si_query.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_MonitorPresentSI);

DECLARE_IPC_NAME(NEBULA_RegisterForPresentSIUpdate);
DECLARE_IPC_NAME(NEBULA_DeregisterFromPresentSIUpdate);
DECLARE_IPC_NAME(NEBULA_PresentSIUpdate);

#endif
