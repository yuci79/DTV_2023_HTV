/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_ca_controller.
 */

#include "nebula/adaptation_layer/nebula_ca_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaCaControllerIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

#include <vector>

extern "C" frost_bool
NEBULA_HasCiPlusSupport()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_HasCiPlusSupport));
}

extern "C" frost_bool
NEBULA_GetCASystemIds(NEBULA_CAStackType type, frost_uint* count, frost_uint** ids)
{
    using result_type = std::vector<frost_uint>;
    result_type result;

    const auto call_result = nebulaRpcCall<frost_bool, result_type>(
                                result, IPC_NAME(NEBULA_GetCASystemIds), type);

    if (call_result == frost_true)
    {
        *count = static_cast<frost_uint>(result.size());
        *ids = new frost_uint[*count];

        std::copy(result.begin(), result.begin() + *count, *ids);
    }

    return call_result;
}
