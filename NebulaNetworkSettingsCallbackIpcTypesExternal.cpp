/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaNetworkSettingsCallbacksIpcExternal structures.
 */


#include "NebulaNetworkSettingsCallbackIpcTypesExternal.hpp"

bool
NetworkSettingsEventCallback::operator==(const NetworkSettingsEventCallback& other) const
{
    return (m_user_data == other.m_user_data &&
            m_function_pointer == other.m_function_pointer);
}