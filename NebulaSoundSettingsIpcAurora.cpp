/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaSoundSettingsIpcAurora class.
 */

#include "NebulaSoundSettingsIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_sound_settings.h"
#include "nebula_src/adaptation_layer/ipc/NebulaSoundSettingsIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaSoundSettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_SoundSettingsIsAudioDescriptionEnabled),
                NEBULA_SoundSettingsIsAudioDescriptionEnabled);

    server.bind(IPC_NAME(NEBULA_SoundSettingsEnableAudioDescription),
                NEBULA_SoundSettingsEnableAudioDescription);   

    server.bind(IPC_NAME(NEBULA_SoundSettingsIsHeadphoneConnected),
                NEBULA_SoundSettingsIsHeadphoneConnected);

    server.bind(IPC_NAME(NEBULA_SoundSettingsGetHeadphoneVolumeLevel),
                NEBULA_SoundSettingsGetHeadphoneVolumeLevel);

    server.bind(IPC_NAME(NEBULA_SoundSettingsGetVolumeLevel),
                NEBULA_SoundSettingsGetVolumeLevel);

    server.bind(IPC_NAME(NEBULA_SoundSettingsIsAudioMuted),
                NEBULA_SoundSettingsIsAudioMuted);

    server.bind(IPC_NAME(NEBULA_SoundSettingsSetHeadphoneVolumeLevel),
                NEBULA_SoundSettingsSetHeadphoneVolumeLevel);

    server.bind(IPC_NAME(NEBULA_SoundSettingsSetVolumeLevel),
                NEBULA_SoundSettingsSetVolumeLevel);

    server.bind(IPC_NAME(NEBULA_SoundSettingsToggleMuteStatus),
                NEBULA_SoundSettingsToggleMuteStatus);
}
