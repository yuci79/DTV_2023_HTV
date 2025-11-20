/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaSystemInformationProviderCallbacksIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASYSTEMINFORMATIONPROVIDERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULASYSTEMINFORMATIONPROVIDERCALLBACKIPCTYPESEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_system_information_provider.h"

struct OnSystemInformationProviderGeolocationDataCallback
{
    NEBULA_SystemInformationProviderGeolocationDataCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnSystemInformationProviderGeolocationDataCallback& other) const;
};

struct OnSystemInformationProviderConsentsDataCallback
{
    NEBULA_SystemInformationProviderConsentsDataCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnSystemInformationProviderConsentsDataCallback& other) const;
};  

struct OnCurrentChannelDetailsUpdatedCallback
{
    NEBULA_CurrentChannelDetailsUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnCurrentChannelDetailsUpdatedCallback& other) const;
};  

struct OnChannelListChangeDataUpdatedCallback
{
    NEBULA_ChannelListChangeDataUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnChannelListChangeDataUpdatedCallback& other) const;
};

struct OnFreeviewPlayDataUpdatedCallback
{
    NEBULA_FreeviewPlayDataUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnFreeviewPlayDataUpdatedCallback& other) const;
}; 

struct OnDetTokenUpdatedCallback
{
    NEBULA_DetTokenUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnDetTokenUpdatedCallback& other) const;
}; 

struct OnAITUpdatedCallback
{
    NEBULA_AITUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnAITUpdatedCallback& other) const;
}; 

struct OnAdvertisingDeviceIDResetCallback
{
    NEBULA_AdvertisingDeviceIDResetCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnAdvertisingDeviceIDResetCallback& other) const;
};

struct OnIFAInfoChangeCallback
{
    NEBULA_IFAInfoChangeCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnIFAInfoChangeCallback& other) const;
};

struct OnNTPUpdatedCallback
{
    NEBULA_NTPUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnNTPUpdatedCallback& other) const;
};

struct OnSourceTypeChangedCallback
{
    NEBULA_SourceTypeChangedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnSourceTypeChangedCallback& other) const;
};

struct OnSystemInformationProviderVisibilityChangedCallback
{
    NEBULA_HomeLauncherVisibilityCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnSystemInformationProviderVisibilityChangedCallback& other) const;
};

struct OnAppStateChangedCallback
{
    NEBULA_AppStateChangedCallback* m_function_pointer;
    void* m_user_data;

    bool operator==(const OnAppStateChangedCallback& other) const;
};

struct OnSoftwareUpdateDataChangedCallback
{
    NEBULA_SoftwareUpdateDataChangedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnSoftwareUpdateDataChangedCallback& other) const;
};

struct OnRCULowBatteryCallback
{
    NEBULA_RCULowBatteryCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnRCULowBatteryCallback& other) const;
};

struct OnPowerUpDataCallback
{
    NEBULA_PowerUpDataCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnPowerUpDataCallback& other) const;
};

struct OnInternetSpeedTestResultCallback
{
    NEBULA_InternetSpeedTestResultCallback* m_function_pointer;
    void* m_user_data;

    bool operator==(const OnInternetSpeedTestResultCallback& other) const;
};

struct OnHomeScreenSaverTimeoutChangedCallback
{
    NEBULA_HomeScreenSaverTimeoutChangedCallback* m_function_pointer;
    void* m_user_data;

    bool operator==(const OnHomeScreenSaverTimeoutChangedCallback& other) const;
};

struct OnFVPTCAcceptedCallback
{
    NEBULA_FVPTCAcceptedCallback* m_function_pointer;
    void* m_user_data;

    bool operator==(const OnFVPTCAcceptedCallback& other) const;
};

struct OnNativeUIAvailabilityDataUpdatedCallback
{
    NEBULA_NativeUIAvailabilityDataUpdatedCallback* m_function_pointer;
    void* m_user_data;

    // Can be provided by default in C++20.
    bool operator==(const OnNativeUIAvailabilityDataUpdatedCallback& other) const;
};

#endif
