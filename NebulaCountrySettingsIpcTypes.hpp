/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_country_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOUNTRYSETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOUNTRYSETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_country_setting.h"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_CountrySettingGetCountry);
DECLARE_IPC_NAME(NEBULA_CountrySettingSetCountry);

/*
 * Define packing of data types used in IPC calls.
 */

struct MP_NEBULA_CountryString : public NEBULA_CountryString
{
    MSGPACK_DEFINE(country_code);

    MEMBER_CHECKED_NEBULA_TYPE(NEBULA_CountryString, "");
};

#endif
