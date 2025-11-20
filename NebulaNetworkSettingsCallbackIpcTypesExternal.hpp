/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaNetworkSettingsCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULANETWORKSETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULANETWORKSETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_network_settings.h"

/**
 * @brief Define the paramters of a NEBULA_NetworkSettingsEventCallback callback.
 */
struct NetworkSettingsEventCallback
{
    NEBULA_NetworkSettingsEventCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const NetworkSettingsEventCallback& other) const;
};

#endif