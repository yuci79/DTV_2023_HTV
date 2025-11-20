/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_component_callback.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaComponentCallbackIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C"
void NEBULA_NotifyOnUpdatedComponents(void)
{
    return nebulaRpcCall(IPC_NAME(NEBULA_NotifyOnUpdatedComponents));
}