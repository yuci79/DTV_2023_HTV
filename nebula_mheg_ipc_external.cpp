/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_mheg.
 */

#include "nebula/adaptation_layer/nebula_mheg.h"
#include "nebula_src/adaptation_layer/ipc/NebulaMhegIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

void 
NEBULA_MhegEnable(bool enable)
{
    nebulaRpcCall(IPC_NAME(NEBULA_MhegEnable), enable);
}

frost_bool
NEBULA_MhegIsEnabled(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_MhegIsEnabled));
}

void NEBULA_MhegResume(void)
{
    nebulaRpcCall(IPC_NAME(NEBULA_MhegResume));
}

void NEBULA_MhegSuspend(void)
{
    nebulaRpcCall(IPC_NAME(NEBULA_MhegSuspend));
}
