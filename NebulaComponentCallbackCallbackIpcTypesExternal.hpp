/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaComponentCallbackCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACOMPONENTCALLBACKCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACOMPONENTCALLBACKCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_component_callback.h"

/**
 * @brief Define the parameters of a NEBULA_AudioComponentCallback callback.
 */
struct AudioComponentCallback
{
    NEBULA_AudioComponentCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const AudioComponentCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_FillComponentListCallback callback.
 */
struct FillComponentListCallback
{
    NEBULA_FillComponentListCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const FillComponentListCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_ActivateSelectedComponentCallback callback.
 */
struct ActivateSelectedComponentCallback
{
    NEBULA_ActivateSelectedComponentCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const ActivateSelectedComponentCallback& other) const;
};

/**
 * @brief Define the parameters of a NEBULA_ActivateSelectedComponentOnIdCallback callback.
 */
struct ActivateSelectedComponentOnIdCallback
{
    NEBULA_ActivateSelectedComponentOnIdCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const ActivateSelectedComponentOnIdCallback& other) const;
};

#endif
