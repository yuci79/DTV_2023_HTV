/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_language_settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaLanguageSettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_LanguageSettingGetAudioLanguage(NEBULA_LanguageCode* language_code)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_LanguageCode>(*language_code,
                                                 IPC_NAME(NEBULA_LanguageSettingGetAudioLanguage));
}

extern "C" frost_bool
NEBULA_LanguageSettingSetAudioLanguage(const NEBULA_LanguageCode* language_code)
{
    MP_NEBULA_LanguageCode mapped_language_code;
    strncpy(mapped_language_code.iso_str, language_code->iso_str, NEBULA_LANG_CODE_LEN);
    mapped_language_code.iso_str[NEBULA_LANG_CODE_LEN - 1] = '\0';
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_LanguageSettingSetAudioLanguage),
                                     mapped_language_code);
}

extern "C" frost_bool
NEBULA_LanguageSettingGetSubtitleLanguage(NEBULA_LanguageCode* language_code)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_LanguageCode>(*language_code,
                                            IPC_NAME(NEBULA_LanguageSettingGetSubtitleLanguage));
}

extern "C" frost_bool
NEBULA_LanguageSettingSetSubtitleLanguage(const NEBULA_LanguageCode* language_code)
{
    MP_NEBULA_LanguageCode mapped_language_code;
    strncpy(mapped_language_code.iso_str, language_code->iso_str, NEBULA_LANG_CODE_LEN);
    mapped_language_code.iso_str[NEBULA_LANG_CODE_LEN - 1] = '\0';
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_LanguageSettingSetSubtitleLanguage),
                                     mapped_language_code);
}

extern "C" frost_bool
NEBULA_LanguageSettingGetSystemLanguage(NEBULA_LanguageCode* language_code)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_LanguageCode>(*language_code,
                                                 IPC_NAME(NEBULA_LanguageSettingGetSystemLanguage));
}

extern "C" frost_bool
NEBULA_LanguageSettingSetSystemLanguage(const NEBULA_LanguageCode* language_code)
{
    MP_NEBULA_LanguageCode mapped_language_code;
    strncpy(mapped_language_code.iso_str, language_code->iso_str, NEBULA_LANG_CODE_LEN);
    mapped_language_code.iso_str[NEBULA_LANG_CODE_LEN - 1] = '\0';
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_LanguageSettingSetSystemLanguage),
                                     mapped_language_code);
}