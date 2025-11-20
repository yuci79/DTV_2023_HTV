/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaDatetimeSettingsIpcAurora class.
 */

#include "NebulaDatetimeSettingsIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_datetime_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaDatetimeSettingsIpcTypes.hpp"
#include <rpc/server.h>
#include <string>
#include <tuple>

void
NebulaDatetimeSettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetUTCCTime),
                NEBULA_DatetimeSettingsGetUTCCTime);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetLocalCTime),
                NEBULA_DatetimeSettingsGetLocalCTime);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneCount),
                NEBULA_DatetimeSettingsGetTimeZoneCount);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneEditable),
                NEBULA_DatetimeSettingsGetTimeZoneEditable);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetIndex),
                NEBULA_DatetimeSettingsGetTimeZoneOffsetIndex);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetStringByIndex), 
        [](frost_int zone_offset_index)
        {
            char zone_offset_string[NEBULA_ZONE_OFFSET_STRING_LENGTH] {};

            const auto result = NEBULA_DatetimeSettingsGetTimeZoneOffsetStringByIndex(
                                    zone_offset_index, zone_offset_string);

            return std::make_tuple(result, std::string(zone_offset_string));
        });

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsIsBroadcastTimeKnown),
                NEBULA_DatetimeSettingsIsBroadcastTimeKnown);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsSetCurrentDateTime),
                NEBULA_DatetimeSettingsSetCurrentDateTime);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneEditable),
                NEBULA_DatetimeSettingsSetTimeZoneEditable);

    server.bind(IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneOffset),
                NEBULA_DatetimeSettingsSetTimeZoneOffset);
}
