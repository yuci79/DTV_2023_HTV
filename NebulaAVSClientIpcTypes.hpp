
/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_avs_client.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAVSCLIENTIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAVSCLIENTIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_avs_client.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include <utility>


using StringPair = std::pair<std::string, std::string>;
/*
 * Define IPC function call names.
 */
 DECLARE_IPC_NAME(NEBULA_AVSClientGetDeviceID);
 DECLARE_IPC_NAME(NEBULA_AVSClientGetClientProperties);
 DECLARE_IPC_NAME(NEBULA_AVSClientGetProductInfo);
 DECLARE_IPC_NAME(NEBULA_AVSClientGetAuthInfo);
 DECLARE_IPC_NAME(NEBULA_AVSClientIsEnabled);
 DECLARE_IPC_NAME(NEBULA_AVSClientGetLocale);
 DECLARE_IPC_NAME(NEBULA_AVSClientGetAlexaOnStandbyEnabled); 
 DECLARE_IPC_NAME(NEBULA_AVSClientSetAlexaOnStandbyEnabled);
 DECLARE_IPC_NAME(NEBULA_AVSClientSetupCompleted);
 DECLARE_IPC_NAME(NEBULA_AVSClientSetEnabled);
#endif
