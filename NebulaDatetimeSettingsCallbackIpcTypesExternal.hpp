/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaDatetimeSettingsCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULADATETIMESETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULADATETIMESETTINGSCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_datetime_settings.h"

struct OnTimeUpdatedEventCallback
{
    NEBULA_TimeUpdatedEventCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnTimeUpdatedEventCallback& other) const;
};

#endif
