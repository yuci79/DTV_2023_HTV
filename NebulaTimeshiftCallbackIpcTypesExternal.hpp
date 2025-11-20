/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaTimeshiftCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULATIMESHIFTCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULATIMESHIFTCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_timeshift.h"

struct OnPlayPositionChangedEvent
{
    NEBULA_OnPlayPositionChangedEvent m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnPlayPositionChangedEvent& other) const;
};

struct OnPlaySpeedChangedEvent
{
    NEBULA_OnPlaySpeedChangedEvent m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnPlaySpeedChangedEvent& other) const;
};

struct OnRecordingEvent
{
    NEBULA_OnRecordingEvent m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnRecordingEvent& other) const;
};

#endif
