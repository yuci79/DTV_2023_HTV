/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_temi_timeline.
 */

#include "nebula/adaptation_layer/nebula_temi_timeline.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTemiTimelineIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/cabot_assert.h"

frost_bool
NEBULA_SelectTemiTimeline(std::uint8_t component_tag, std::uint8_t timeline_id,
                          frost_bool enable)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SelectTemiTimeline),
                                     component_tag,
                                     timeline_id,
                                     enable);
}

frost_bool
NEBULA_GetTemiTimestamp(std::uint8_t component_tag,
                        std::uint8_t timeline_id,
                        NEBULA_TemiTimestamp* result)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TemiTimestamp>(*result,
                                            IPC_NAME(NEBULA_GetTemiTimestamp),
                                            component_tag,
                                            timeline_id);
}

frost_bool
NEBULA_GetPtsTimestamp(NEBULA_TemiTimestamp* result)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TemiTimestamp>(*result,
                                            IPC_NAME(NEBULA_GetPtsTimestamp));
}

NEBULA_TimelineType
NEBULA_extractTimelineType(const char* selector, std::uint8_t* component_tag, std::uint8_t* timeline_id)
{
    std::pair<std::uint8_t, std::uint8_t> data;

    const NEBULA_TimelineType type = nebulaRpcCall<NEBULA_TimelineType, std::pair<std::uint8_t, std::uint8_t>>(
                                    data,
                                    IPC_NAME(NEBULA_extractTimelineType),
                                    selector);

    *component_tag = data.first;
    *timeline_id = data.second;
    return type;
}