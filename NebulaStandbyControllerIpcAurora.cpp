/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaStandbyControllerIpcAurora class.
 */

#include "NebulaStandbyControllerIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_standby_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaStandbyControllerIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaStandbyControllerIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_StandbyControllerIsEasiStandbyRequested), 
                NEBULA_StandbyControllerIsEasiStandbyRequested);
}