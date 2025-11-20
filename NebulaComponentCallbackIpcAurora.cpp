/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaComponentCallbackIpcAurora class.
 */

#include "NebulaComponentCallbackIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_component_callback.h"
#include "nebula_src/adaptation_layer/ipc/NebulaComponentCallbackIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaComponentCallbackIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_NotifyOnUpdatedComponents), NEBULA_NotifyOnUpdatedComponents);
}
