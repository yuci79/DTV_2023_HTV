/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaLanguageSettingsIpcAurora class.
 */

#include "NebulaLanguageSettingsIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaLanguageSettingsIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaLanguageSettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_LanguageSettingGetAudioLanguage), []() {
        NebulaIpcType<frost_bool, MP_NEBULA_LanguageCode> language_code;
        language_code.return_value =
                        NEBULA_LanguageSettingGetAudioLanguage(&language_code.output_params);
        return language_code; });

    server.bind(IPC_NAME(NEBULA_LanguageSettingSetAudioLanguage),
        [](MP_NEBULA_LanguageCode& language_code) {
        return NEBULA_LanguageSettingSetAudioLanguage(&language_code); });

    server.bind(IPC_NAME(NEBULA_LanguageSettingGetSubtitleLanguage), []() {
        NebulaIpcType<frost_bool, MP_NEBULA_LanguageCode> language_code;
        language_code.return_value =
                        NEBULA_LanguageSettingGetSubtitleLanguage(&language_code.output_params);
        return language_code; });

    server.bind(IPC_NAME(NEBULA_LanguageSettingSetSubtitleLanguage),
        [](MP_NEBULA_LanguageCode& language_code) {
        return NEBULA_LanguageSettingSetSubtitleLanguage(&language_code); });

    server.bind(IPC_NAME(NEBULA_LanguageSettingGetSystemLanguage), []() {
        NebulaIpcType<frost_bool, MP_NEBULA_LanguageCode> language_code;
        language_code.return_value =
                        NEBULA_LanguageSettingGetSystemLanguage(&language_code.output_params);
        return language_code; });

    server.bind(IPC_NAME(NEBULA_LanguageSettingSetSystemLanguage),
        [](MP_NEBULA_LanguageCode& language_code) {
        return NEBULA_LanguageSettingSetSystemLanguage(&language_code); });
}
