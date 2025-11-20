/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_application_capability.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaApplicationCapabilityIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_bool
NEBULA_GetSharedCapability(NEBULA_ApplicationSharedCapability* capability)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_ApplicationSharedCapability>(*capability,
                                                  IPC_NAME(NEBULA_GetSharedCapability));
}