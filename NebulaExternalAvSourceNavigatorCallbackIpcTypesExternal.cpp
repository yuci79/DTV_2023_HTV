/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for
 *        NebulaExternalAvSourceNavigatorCallbacksIpcExternal structures.
 */

#include "NebulaExternalAvSourceNavigatorCallbackIpcTypesExternal.hpp"

bool
ExternalInterfaceStatusChangeCallback::operator==(
        const ExternalInterfaceStatusChangeCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
ExternalSourceListUpdatedCallback::operator==(const ExternalSourceListUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}
