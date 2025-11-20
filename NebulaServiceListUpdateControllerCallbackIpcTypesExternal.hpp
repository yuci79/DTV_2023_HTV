/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaServiceListUpdateControllerCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASERVICELISTUPDATECONTROLLERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASERVICELISTUPDATECONTROLLERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_service_list_update_controller.h"

/**
 * @brief Define the paramters of a NEBULA_ServiceListUpdateCallback callback.
 */
struct ServiceListUpdateCallback
{
    NEBULA_ServiceListUpdateCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const ServiceListUpdateCallback& other) const;
};

#endif
