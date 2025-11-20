/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaTimeshiftCallbacksIpcExternal structures.
 */

#include "NebulaTimeshiftCallbackIpcTypesExternal.hpp"

bool
OnPlayPositionChangedEvent::operator==(const OnPlayPositionChangedEvent& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnPlaySpeedChangedEvent::operator==(const OnPlaySpeedChangedEvent& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnRecordingEvent::operator==(const OnRecordingEvent& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}