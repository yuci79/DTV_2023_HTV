/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_sound_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASOUNDSETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASOUNDSETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_SoundSettingsIsAudioDescriptionEnabled);
DECLARE_IPC_NAME(NEBULA_SoundSettingsEnableAudioDescription);
DECLARE_IPC_NAME(NEBULA_SoundSettingsIsHeadphoneConnected);
DECLARE_IPC_NAME(NEBULA_SoundSettingsGetHeadphoneVolumeLevel);
DECLARE_IPC_NAME(NEBULA_SoundSettingsGetVolumeLevel);
DECLARE_IPC_NAME(NEBULA_SoundSettingsIsAudioMuted);
DECLARE_IPC_NAME(NEBULA_SoundSettingsSetHeadphoneVolumeLevel);
DECLARE_IPC_NAME(NEBULA_SoundSettingsSetVolumeLevel);
DECLARE_IPC_NAME(NEBULA_SoundSettingsToggleMuteStatus);

#endif
