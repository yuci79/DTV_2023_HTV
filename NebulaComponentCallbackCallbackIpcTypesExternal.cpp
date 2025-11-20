/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaComponentCallbackCallbacksIpcExternal structures.
 */


#include "NebulaComponentCallbackCallbackIpcTypesExternal.hpp"

bool
AudioComponentCallback::operator==(const AudioComponentCallback& other) const
{
    // This operator is only implemented to support RegisteredCallbacks deregistration.
    // Deregistaration for this callback only passes the user data
    // so no other members are checked.
    return (m_user_data == other.m_user_data);
}

bool
FillComponentListCallback::operator==(const FillComponentListCallback& other) const
{
    // This operator is only implemented to support RegisteredCallbacks deregistration.
    // Deregistaration for this callback only passes the user data
    // so no other members are checked.
    return (m_user_data == other.m_user_data);
}

bool
ActivateSelectedComponentCallback::operator==(const ActivateSelectedComponentCallback& other) const
{
    // This operator is only implemented to support RegisteredCallbacks deregistration.
    // Deregistaration for this callback only passes the user data
    // so no other members are checked.
    return (m_user_data == other.m_user_data);
}

bool
ActivateSelectedComponentOnIdCallback::operator==(const ActivateSelectedComponentOnIdCallback& other) const
{
    // This operator is only implemented to support RegisteredCallbacks deregistration.
    // Deregistaration for this callback only passes the user data
    // so no other members are checked.
    return (m_user_data == other.m_user_data);
}