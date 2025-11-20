/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_service_list_update_controller.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaServiceListUpdateControllerIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_IsChannelListUpdated()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsChannelListUpdated));
}

extern "C" frost_bool
NEBULA_ResetChannelListUpdated()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_ResetChannelListUpdated));
}

extern "C" frost_bool
NEBULA_SetChannelList(const char* channel_list_path)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetChannelList), channel_list_path);
}