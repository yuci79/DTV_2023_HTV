/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_removable_device_controller.
 */

#include "nebula/adaptation_layer/nebula_removable_device_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaRemovableDeviceControllerIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/cabot_assert.h"

bool
NEBULA_CopyDigitalIbToRemovableDevice(void)
{
    return nebulaRpcCall<bool>(IPC_NAME(NEBULA_CopyDigitalIbToRemovableDevice));
}
