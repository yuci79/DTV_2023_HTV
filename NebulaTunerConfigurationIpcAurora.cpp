/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaTunerConfigurationIpcAurora class.
 */

#include "NebulaTunerConfigurationIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_tuner_configuration.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTunerConfigurationIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaTunerConfigurationIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_GetNumberOfTuners), NEBULA_GetNumberOfTuners);

    server.bind(IPC_NAME(NEBULA_GetCurrentChannelDetails), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_RFChannelDetails> result;
        result.return_value = NEBULA_GetCurrentChannelDetails(&result.output_params);
        return result;
    });
}