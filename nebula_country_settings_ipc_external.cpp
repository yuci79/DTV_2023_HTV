/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_country_settings.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaCountrySettingsIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_bool
NEBULA_CountrySettingSetCountry(NEBULA_CountryString country, frost_bool fti_selection)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_CountrySettingSetCountry),
                                     country.country_code, fti_selection);
}

extern "C" frost_bool
NEBULA_CountrySettingGetCountry(NEBULA_CountryString* country)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_CountryString>(*country,
                                                  IPC_NAME(NEBULA_CountrySettingGetCountry));
}