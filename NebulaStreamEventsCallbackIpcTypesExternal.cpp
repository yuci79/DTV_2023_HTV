/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaStreamEventsCallbacksIpcExternal structures.
 */


#include "NebulaStreamEventsCallbackIpcTypesExternal.hpp"

bool
DsmccStreamEventCallback::operator==(const DsmccStreamEventCallback& other) const
{
    return (m_user_data == other.m_user_data);
}