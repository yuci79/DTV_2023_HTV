/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaMhegIpcAurora class.
 */

#include "NebulaMhegIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_mheg.h"
#include "nebula_src/adaptation_layer/ipc/NebulaMhegIpcTypes.hpp"

#include <rpc/server.h>

void
NebulaMhegIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_mheg 'C' functions.

    server.bind(IPC_NAME(NEBULA_MhegEnable), &NEBULA_MhegEnable);

    server.bind(IPC_NAME(NEBULA_MhegIsEnabled), &NEBULA_MhegIsEnabled);
    
    server.bind(IPC_NAME(NEBULA_MhegResume), &NEBULA_MhegResume);

    server.bind(IPC_NAME(NEBULA_MhegSuspend), &NEBULA_MhegSuspend);
}
