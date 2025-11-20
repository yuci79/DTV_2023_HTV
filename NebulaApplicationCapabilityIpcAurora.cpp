/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaApplicationCapabilityIpcAurora class.
 */

#include "NebulaApplicationCapabilityIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaApplicationCapabilityIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaApplicationCapabilityIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_GetSharedCapability), []() {
            NebulaIpcType<frost_bool, MP_NEBULA_ApplicationSharedCapability> capability;
            capability.return_value = NEBULA_GetSharedCapability(&capability.output_params);
            return capability; });
}