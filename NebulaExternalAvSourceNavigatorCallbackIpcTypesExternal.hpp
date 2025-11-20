/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaExternalAvSourceNavigatorCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAEXTERNALAVSOURCENAVIGATORCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAEXTERNALAVSOURCENAVIGATORCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_external_av_source_navigator.h"

/**
 * @brief Define the parameters of a ExternalInterfaceStatusChange callback.
 */
struct ExternalInterfaceStatusChangeCallback
{
    NEBULA_ExternalInterfaceStatusChangeCallback m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const ExternalInterfaceStatusChangeCallback& other) const;
};

/**
 * @brief Define the parameters of a ExternaSourceListUpdated callback.
 */
struct ExternalSourceListUpdatedCallback
{
    NEBULA_ExternalSourceListUpdatedCallBack m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const ExternalSourceListUpdatedCallback& other) const;
};

#endif
