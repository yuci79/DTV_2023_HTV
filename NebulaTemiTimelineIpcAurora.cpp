/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaTemiTimelineIpcAurora class.
 */

#include "NebulaTemiTimelineIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_temi_timeline.h"
#include "nebula_src/adaptation_layer/ipc/NebulaTemiTimelineIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaTemiTimelineIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SelectTemiTimeline), &NEBULA_SelectTemiTimeline);

    server.bind(IPC_NAME(NEBULA_GetTemiTimestamp), [](std::uint8_t component_tag,
                                                      std::uint8_t timeline_id) {
        NebulaIpcType<frost_bool, MP_NEBULA_TemiTimestamp> timestamp;
        timestamp.return_value = NEBULA_GetTemiTimestamp(component_tag,
                                                         timeline_id,
                                                         &timestamp.output_params);
        return timestamp; });

    server.bind(IPC_NAME(NEBULA_GetPtsTimestamp), []() {
        NebulaIpcType<frost_bool, MP_NEBULA_TemiTimestamp> timestamp;
        timestamp.return_value = NEBULA_GetPtsTimestamp(&timestamp.output_params);
        return timestamp; });

    server.bind(IPC_NAME(NEBULA_extractTimelineType), [](std::string selector) {
        NebulaIpcType<NEBULA_TimelineType, std::pair<std::uint8_t, std::uint8_t>> data;
        data.return_value = NEBULA_extractTimelineType(selector.c_str(),
                                                       &(data.output_params.first),
                                                       &(data.output_params.second));
        return data; });
}