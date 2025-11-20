/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_subtitle_settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaSubtitleSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_SubtitleSettingsGetEnabled()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SubtitleSettingsGetEnabled));
}

extern "C" frost_bool
NEBULA_SubtitleSettingsSetEnabled(frost_bool enable)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SubtitleSettingsSetEnabled), enable);
}