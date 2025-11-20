/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaChannelChangeCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACHANNELCHANGECALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACHANNELCHANGECALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_channel_change.h"

/**
 * @brief Define the parameters of a NEBULA_OnChannelChange callback.
 */
struct OnChannelChangeCallback 
{
    NEBULA_OnChannelChange m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnChannelChangeCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_OnTunerStatusChange callback.
 */
struct OnTunerStatusChangeCallback 
{
    NEBULA_OnTunerStatusChange m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnTunerStatusChangeCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_OnComponentChange callback.
 */
struct OnComponentChangeCallback
{
    NEBULA_OnComponentChange m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnComponentChangeCallback& other) const;
};

#endif
