/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_external_av_source_navigator.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAEXTERNALAVSOURCENAVIGATORIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAEXTERNALAVSOURCENAVIGATORIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_external_av_source_navigator.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetNumberOfExternalInterfaces);
DECLARE_IPC_NAME(NEBULA_GetExternalInterfaceName);
DECLARE_IPC_NAME(NEBULA_GetExternalInterfaceTypeName);
DECLARE_IPC_NAME(NEBULA_GetCurrentExternalInterfaceIndex);
DECLARE_IPC_NAME(NEBULA_IsExternalInterfaceSupported);
DECLARE_IPC_NAME(NEBULA_IsExternalInterfaceEnabled);
DECLARE_IPC_NAME(NEBULA_IsExternalInterfaceDisplayed);
DECLARE_IPC_NAME(NEBULA_DeselectExternalInterface);
DECLARE_IPC_NAME(NEBULA_SelectExternalInterface);

DECLARE_IPC_NAME(NEBULA_RegisterExternalInterfaceStatusCallback);
DECLARE_IPC_NAME(NEBULA_RegisterExternalSourceListUpdatedCallback);

DECLARE_IPC_NAME(NEBULA_ExternalInterfaceStatusChangeCallback);
DECLARE_IPC_NAME(NEBULA_ExternalSourceListUpdatedCallback);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_DisplayWindowId);
MSGPACK_ADD_ENUM(NEBULA_ExternalInterfaceStatus);


#endif
