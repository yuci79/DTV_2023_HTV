/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaCaControllerIpcAurora class.
 */

#include "NebulaCaControllerIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaCaControllerIpcTypes.hpp"

#include "frost/frost_standards.h"

#include <rpc/server.h>
#include <vector>

void
NebulaCaControllerIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_ca_controller 'C' functions.

    server.bind(IPC_NAME(NEBULA_HasCiPlusSupport), &NEBULA_HasCiPlusSupport);

    server.bind(IPC_NAME(NEBULA_GetCASystemIds), [](NEBULA_CAStackType type)
        {
            NebulaIpcType<frost_bool, std::vector<frost_uint>> result;
            frost_uint count = 0;
            frost_uint* ids = nullptr;

            result.return_value = NEBULA_GetCASystemIds(type, &count, &ids);

            if (result.return_value) {
                result.output_params = std::vector<frost_uint>(ids, ids + count);
                delete[] ids;
            }

            return result;
        });
}
