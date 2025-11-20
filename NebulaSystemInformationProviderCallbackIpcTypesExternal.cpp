/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines comparison helpers for NebulaSystemInformationProviderCallbacksIpcExternal structures.
 */

#include "NebulaSystemInformationProviderCallbackIpcTypesExternal.hpp"

bool
OnSystemInformationProviderGeolocationDataCallback::operator==(const OnSystemInformationProviderGeolocationDataCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnSystemInformationProviderConsentsDataCallback::operator==(const OnSystemInformationProviderConsentsDataCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnCurrentChannelDetailsUpdatedCallback::operator==(const OnCurrentChannelDetailsUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}  

bool
OnChannelListChangeDataUpdatedCallback::operator==(const OnChannelListChangeDataUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnFreeviewPlayDataUpdatedCallback::operator==(const OnFreeviewPlayDataUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnDetTokenUpdatedCallback::operator==(const OnDetTokenUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnAITUpdatedCallback::operator==(const OnAITUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
} 

bool
OnAdvertisingDeviceIDResetCallback::operator==(const OnAdvertisingDeviceIDResetCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnIFAInfoChangeCallback::operator==(const OnIFAInfoChangeCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnNTPUpdatedCallback::operator==(const OnNTPUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnSourceTypeChangedCallback::operator==(const OnSourceTypeChangedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnSystemInformationProviderVisibilityChangedCallback::operator==(const OnSystemInformationProviderVisibilityChangedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnAppStateChangedCallback::operator==(const OnAppStateChangedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnSoftwareUpdateDataChangedCallback::operator==(const OnSoftwareUpdateDataChangedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnRCULowBatteryCallback::operator==(const OnRCULowBatteryCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnPowerUpDataCallback::operator==(const OnPowerUpDataCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnInternetSpeedTestResultCallback::operator==(const OnInternetSpeedTestResultCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnHomeScreenSaverTimeoutChangedCallback::operator==(const OnHomeScreenSaverTimeoutChangedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnFVPTCAcceptedCallback::operator==(const OnFVPTCAcceptedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

bool
OnNativeUIAvailabilityDataUpdatedCallback::operator==(const OnNativeUIAvailabilityDataUpdatedCallback& other) const
{
    return (m_function_pointer == other.m_function_pointer) &&
           (m_user_data == other.m_user_data);
}

