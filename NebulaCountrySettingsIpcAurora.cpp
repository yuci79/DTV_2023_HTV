/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaCountrySettingsIpcAurora class.
 */

#include "NebulaCountrySettingsIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaCountrySettingsIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaCountrySettingsIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_CountrySettingGetCountry), []() {
            NebulaIpcType<frost_bool, MP_NEBULA_CountryString> country;
            country.return_value = NEBULA_CountrySettingGetCountry(&country.output_params);
            return country; });
            
    server.bind(IPC_NAME(NEBULA_CountrySettingSetCountry),
            [](std::string& country, frost_bool fti_selection) {
            NEBULA_CountryString country_string;
            strncpy(country_string.country_code, country.c_str(), NEBULA_COUNTRY_STRING_LENGTH);
            country_string.country_code[NEBULA_COUNTRY_STRING_LENGTH - 1] = '\0';
            return NEBULA_CountrySettingSetCountry(country_string, fti_selection); });
}