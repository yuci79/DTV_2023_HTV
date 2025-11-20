/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_language_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULALANGUAGESETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULALANGUAGESETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_language_setting.h"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_LanguageSettingGetAudioLanguage);
DECLARE_IPC_NAME(NEBULA_LanguageSettingSetAudioLanguage);
DECLARE_IPC_NAME(NEBULA_LanguageSettingGetSubtitleLanguage);
DECLARE_IPC_NAME(NEBULA_LanguageSettingSetSubtitleLanguage);
DECLARE_IPC_NAME(NEBULA_LanguageSettingGetSystemLanguage);
DECLARE_IPC_NAME(NEBULA_LanguageSettingSetSystemLanguage);

/*
 * Define packing of data types used in IPC calls.
 */

struct MP_NEBULA_LanguageCode : public NEBULA_LanguageCode
{
    MSGPACK_DEFINE(iso_str);

    MEMBER_CHECKED_NEBULA_TYPE(NEBULA_LanguageCode, "");
};

#endif
