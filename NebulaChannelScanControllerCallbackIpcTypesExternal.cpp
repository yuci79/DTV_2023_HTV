/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaChannelScanControllerCallbacksIpcExternal structures.
 */


#include "NebulaChannelScanControllerCallbackIpcTypesExternal.hpp"

bool
OnScanEventCallback::operator==(const OnScanEventCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}