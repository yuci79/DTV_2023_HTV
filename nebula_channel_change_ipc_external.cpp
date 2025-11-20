/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_channel_change.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaChannelChangeIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" 
NEBULA_SelectChannelResult NEBULA_SelectChannel(
                                 NEBULA_ChannelChangeMethod method,
                                 frost_int channel_id,
                                 NEBULA_ChannelChangeQuietValue quiet_value)
{
    return nebulaRpcCall<NEBULA_SelectChannelResult>(IPC_NAME(NEBULA_SelectChannel),
                                      method,
                                      channel_id,
                                      quiet_value);
}

extern "C"
frost_bool NEBULA_StopCurrentChannel()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StopCurrentChannel));
}

extern "C" 
frost_bool NEBULA_SelectIPChannel(NEBULA_ChannelChangeMethod method,
                                  frost_int channel_handle)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SelectIPChannel),
                                     method,
                                     channel_handle);
}
