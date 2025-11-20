/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaChannelChangeIpcAurora class.
 */

#include "NebulaChannelChangeIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_channel_change.h"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelChangeIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaChannelChangeIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SelectChannel), NEBULA_SelectChannel);
    server.bind(IPC_NAME(NEBULA_StopCurrentChannel), NEBULA_StopCurrentChannel);
    server.bind(IPC_NAME(NEBULA_SelectIPChannel), NEBULA_SelectIPChannel);
}
