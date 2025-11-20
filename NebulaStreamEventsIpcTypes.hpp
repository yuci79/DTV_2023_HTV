/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_stream_events.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASTREAMEVENTSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASTREAMEVENTSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_stream_events.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

#include "frost/frost_basictypes.h"
#include <string>
#include <vector>

using NebulaIpcStreamEvent = std::pair<frost_uint, std::string>;
using NebulaIpcStreamEvents = std::vector<NebulaIpcStreamEvent>;

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_DsmccStreamEventSetSource);
DECLARE_IPC_NAME(NEBULA_DsmccStreamEventEnable);

DECLARE_IPC_NAME(NEBULA_DsmccStreamEventSubscribe);
DECLARE_IPC_NAME(NEBULA_DsmccStreamEventUnsubscribe);
DECLARE_IPC_NAME(NEBULA_DsmccStreamEventCallback);

#endif
