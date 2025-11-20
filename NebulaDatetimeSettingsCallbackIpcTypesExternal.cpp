/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaDatetimeSettingsCallbacksIpcExternal structures.
 */

#include "NebulaDatetimeSettingsCallbackIpcTypesExternal.hpp"

bool
OnTimeUpdatedEventCallback::operator==(const OnTimeUpdatedEventCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}
