/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_pvr_disk_controller.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRDISKCONTROLLERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRDISKCONTROLLERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetPvrStatus);
DECLARE_IPC_NAME(NEBULA_GetTotalDiskSpace);
DECLARE_IPC_NAME(NEBULA_GetFreeDiskSpace);
DECLARE_IPC_NAME(NEBULA_GetReservedDiskSpaceForRecordings);

#endif
