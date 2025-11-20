/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_component_callback.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOMPONENTCALLBACKIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOMPONENTCALLBACKIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_component_callback.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "NebulaComponentIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_NotifyOnUpdatedComponents);

DECLARE_IPC_NAME(NEBULA_RegisterAudioComponentCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterAudioComponentCallback);
DECLARE_IPC_NAME(NEBULA_AudioComponentCallback);

DECLARE_IPC_NAME(NEBULA_RegisterFillComponentListCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterFillComponentListCallback);
DECLARE_IPC_NAME(NEBULA_FillComponentListCallback);

DECLARE_IPC_NAME(NEBULA_RegisterActivateSelectedComponentCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterActivateSelectedComponentCallback);
DECLARE_IPC_NAME(NEBULA_ActivateSelectedComponentCallback);

DECLARE_IPC_NAME(NEBULA_RegisterActivateSelectedComponentOnIdCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterActivateSelectedComponentOnIdCallback);
DECLARE_IPC_NAME(NEBULA_ActivateSelectedComponentOnIdCallback);

#endif
