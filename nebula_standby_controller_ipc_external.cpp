/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_standby_controller.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaStandbyControllerIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_standby_controller.h"
#include "NebulaIpcHelper.hpp"

frost_bool
NEBULA_StandbyControllerIsEasiStandbyRequested(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StandbyControllerIsEasiStandbyRequested));
}