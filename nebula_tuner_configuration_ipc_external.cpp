/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_tuner_configuration.
 */

#include "nebula/adaptation_layer/nebula_tuner_configuration.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTunerConfigurationIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_int
NEBULA_GetNumberOfTuners(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetNumberOfTuners));
}

frost_bool
NEBULA_GetCurrentChannelDetails(NEBULA_RFChannelDetails* channel_details)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_RFChannelDetails>(
                                *channel_details, IPC_NAME(NEBULA_GetCurrentChannelDetails));
}