/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_datetime_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULADATETIMESETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULADATETIMESETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetUTCCTime);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetLocalCTime);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneCount);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneEditable);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetIndex);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetStringByIndex);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsIsBroadcastTimeKnown);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsSetCurrentDateTime);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneEditable);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneOffset);
DECLARE_IPC_NAME(NEBULA_OnTimeUpdatedEvent);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsRegisterTimeUpdatedEventCallback);
DECLARE_IPC_NAME(NEBULA_DatetimeSettingsDeregisterTimeUpdatedEventCallback);

#endif
