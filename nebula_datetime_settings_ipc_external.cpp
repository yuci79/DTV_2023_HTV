/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_datetime_settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaDatetimeSettingsIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_datetime_settings.h"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/cabot_assert.h"
#include <string>
#include <tuple>

frost_uint32
NEBULA_DatetimeSettingsGetUTCCTime()
{
    return nebulaRpcCall<frost_uint32>(IPC_NAME(NEBULA_DatetimeSettingsGetUTCCTime));
}

frost_uint32
NEBULA_DatetimeSettingsGetLocalCTime()
{
    return nebulaRpcCall<frost_uint32>(IPC_NAME(NEBULA_DatetimeSettingsGetLocalCTime));
}

frost_int
NEBULA_DatetimeSettingsGetTimeZoneCount()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneCount));
}

frost_bool
NEBULA_DatetimeSettingsGetTimeZoneEditable()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneEditable));
}

frost_int
NEBULA_DatetimeSettingsGetTimeZoneOffsetIndex()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetIndex));
}

frost_bool
NEBULA_DatetimeSettingsGetTimeZoneOffsetStringByIndex(frost_int zone_offset_index,
                                                      char* zone_offset_string)
{
    const auto result_data = 
        nebulaRpcCall<std::tuple<frost_bool, std::string>>(
                IPC_NAME(NEBULA_DatetimeSettingsGetTimeZoneOffsetStringByIndex),
                zone_offset_index);

    const auto result = std::get<0>(result_data);

    if (result == frost_true)
    {
        strcpy(zone_offset_string, std::get<1>(result_data).c_str());
    }

    return result;
}

frost_bool
NEBULA_DatetimeSettingsIsBroadcastTimeKnown()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_DatetimeSettingsIsBroadcastTimeKnown));
}

frost_bool
NEBULA_DatetimeSettingsSetCurrentDateTime(frost_uint32 current_date_time, bool is_set_time_forced)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_DatetimeSettingsSetCurrentDateTime),
                                     current_date_time, is_set_time_forced);
}

frost_bool
NEBULA_DatetimeSettingsSetTimeZoneEditable(frost_bool editable)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneEditable),
                                     editable);
}

frost_bool
NEBULA_DatetimeSettingsSetTimeZoneOffset(frost_int time_zone_offset)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_DatetimeSettingsSetTimeZoneOffset),
                                     time_zone_offset);
}
