/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_signal_info.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASIGNALINFOIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASIGNALINFOIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_signal_info.h"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_SignalInfoGetStrength);
DECLARE_IPC_NAME(NEBULA_SignalInfoGetQuality);
DECLARE_IPC_NAME(NEBULA_SignalInfoGetBer);
DECLARE_IPC_NAME(NEBULA_SignalInfoGetSnr);
DECLARE_IPC_NAME(NEBULA_SignalInfoGetLock);

#endif
