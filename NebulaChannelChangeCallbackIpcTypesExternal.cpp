/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaChannelChangeCallbacksIpcExternal structures.
 */


#include "NebulaChannelChangeCallbackIpcTypesExternal.hpp"

bool
OnChannelChangeCallback::operator==(const OnChannelChangeCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnTunerStatusChangeCallback::operator==(const OnTunerStatusChangeCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnComponentChangeCallback::operator==(const OnComponentChangeCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}
