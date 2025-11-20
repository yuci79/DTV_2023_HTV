/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaSubtitleSettingsIpcAurora class.
 */

#include "NebulaSubtitleSettingsIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_subtitle_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaSubtitleSettingsIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaSubtitleSettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SubtitleSettingsGetEnabled),
                NEBULA_SubtitleSettingsGetEnabled);

    server.bind(IPC_NAME(NEBULA_SubtitleSettingsSetEnabled),
                NEBULA_SubtitleSettingsSetEnabled);
}
