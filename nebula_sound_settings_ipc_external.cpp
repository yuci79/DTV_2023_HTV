/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_sound_settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaSoundSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_SoundSettingsIsAudioDescriptionEnabled(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsIsAudioDescriptionEnabled));
}

extern "C" frost_bool
NEBULA_SoundSettingsEnableAudioDescription(frost_bool enable)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsEnableAudioDescription),
                                     enable);
}

extern "C"
frost_bool NEBULA_SoundSettingsIsHeadphoneConnected()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsIsHeadphoneConnected));
}

extern "C"
frost_int NEBULA_SoundSettingsGetHeadphoneVolumeLevel()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SoundSettingsGetHeadphoneVolumeLevel));
}

extern "C"
frost_int NEBULA_SoundSettingsGetVolumeLevel()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SoundSettingsGetVolumeLevel));
}

extern "C"
frost_bool NEBULA_SoundSettingsIsAudioMuted()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsIsAudioMuted));
}

extern "C"
frost_bool NEBULA_SoundSettingsSetHeadphoneVolumeLevel(frost_int new_headphone_level)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsSetHeadphoneVolumeLevel),
                                     new_headphone_level);
}

extern "C"
frost_bool NEBULA_SoundSettingsSetVolumeLevel(frost_int new_volume_level)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsSetVolumeLevel),
                                     new_volume_level);
}

extern "C"
frost_bool NEBULA_SoundSettingsToggleMuteStatus()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SoundSettingsToggleMuteStatus));
}