/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_service_list_update_controller.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASERVICELISTUPDATECONTROLLERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASERVICELISTUPDATECONTROLLERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_service_list_update_controller.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_IsChannelListUpdated);
DECLARE_IPC_NAME(NEBULA_ResetChannelListUpdated);
DECLARE_IPC_NAME(NEBULA_SetChannelList);

DECLARE_IPC_NAME(NEBULA_RegisterServiceListUpdateCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterServiceListUpdateCallback);
DECLARE_IPC_NAME(NEBULA_ServiceListUpdateCallback);

#endif
