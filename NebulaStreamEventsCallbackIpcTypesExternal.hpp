/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaStreamEventsCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASTREAMEVENTSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASTREAMEVENTSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_stream_events.h"

/**
 * @brief Define the paramters of a NEBULA_DsmccStreamEventCallback callback.
 */
struct DsmccStreamEventCallback
{
    NEBULA_DsmccStreamEventCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const DsmccStreamEventCallback& other) const;
};

#endif
