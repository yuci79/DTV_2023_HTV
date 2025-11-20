/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaSignalInfoIpcAurora class.
 */

#include "NebulaSignalInfoIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaSignalInfoIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaSignalInfoIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SignalInfoGetStrength), NEBULA_SignalInfoGetStrength);
    server.bind(IPC_NAME(NEBULA_SignalInfoGetQuality), NEBULA_SignalInfoGetQuality);
    server.bind(IPC_NAME(NEBULA_SignalInfoGetBer), NEBULA_SignalInfoGetBer);
    server.bind(IPC_NAME(NEBULA_SignalInfoGetSnr), NEBULA_SignalInfoGetSnr);
    server.bind(IPC_NAME(NEBULA_SignalInfoGetLock), NEBULA_SignalInfoGetLock);
}