/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_configuration.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACONFIGURATIONIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACONFIGURATIONIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetNetworkIds);
DECLARE_IPC_NAME(NEBULA_GetDeviceId);
DECLARE_IPC_NAME(NEBULA_SetHostingPageUrl);
DECLARE_IPC_NAME(NEBULA_IsAudioDescriptionEnabled);
DECLARE_IPC_NAME(NEBULA_NormaliseLanguageCode);

#endif
