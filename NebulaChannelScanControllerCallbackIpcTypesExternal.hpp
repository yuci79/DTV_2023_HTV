/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaChannelScanControllerCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACHANNELSCANCONTROLLERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULACHANNELSCANCONTROLLERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_channel_scan_controller.h"

/**
 * @brief Define the parameters of a NEBULA_SetScanEventCallback callback.
 */
struct OnScanEventCallback
{
    NEBULA_ScanEventCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnScanEventCallback& other) const;
};

#endif
