/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the NEBULA_OipfCallbackHandle callback registration types.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_REGISTEREDOIPFCALLBACKHANDLES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_REGISTEREDOIPFCALLBACKHANDLES_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredOipfCallbackHandlesConfig.hpp"
#include "nebula/adaptation_layer/nebula_oipf_sas_provider.h"

using RegisteredOipfCallbackHandles = RegisteredStreamHandles<NEBULA_OipfCallbackHandle>;

#endif
