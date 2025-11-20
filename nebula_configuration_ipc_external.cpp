/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_configuration.
 */

#include "nebula/adaptation_layer/nebula_configuration.h"
#include "nebula_src/adaptation_layer/ipc/NebulaConfigurationIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "language/LanguageCode.hpp"
#include "utilities_debug/cabot_assert.h"

frost_bool
NEBULA_GetNetworkIds(frost_int *count, frost_int ** network_id_array)
{
    std::vector<frost_int> ids;
    const auto result = nebulaRpcCall<frost_bool, std::vector<frost_int>>(
                                ids, IPC_NAME(NEBULA_GetNetworkIds));
    if (result == frost_true)
    {
        *count = static_cast<frost_int>(ids.size());
        *network_id_array = static_cast<frost_int*>(malloc(sizeof(frost_int) * (*count)));
        std::copy(ids.begin(), ids.begin() + *count, *network_id_array);
    }
    return result; 
}

void
NEBULA_SetHostingPageUrl(const char* url)
{
    nebulaRpcCall(IPC_NAME(NEBULA_SetHostingPageUrl), std::string(url));
}

frost_bool
NEBULA_GetDeviceId(char* device_id)
{
    memset(device_id, '\0', NEBULA_DEVICE_ID_LENGTH);
    std::string id;
    const auto result = nebulaRpcCall<frost_bool, std::string>(
                                id, IPC_NAME(NEBULA_GetDeviceId));
    if (result == frost_true)
    {
        strncpy(device_id, id.c_str(), NEBULA_DEVICE_ID_LENGTH);
        device_id[NEBULA_DEVICE_ID_LENGTH - 1] = '\0';
    }
    return result;    
}

frost_bool
NEBULA_IsAudioDescriptionEnabled()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsAudioDescriptionEnabled));
}

frost_bool
NEBULA_NormaliseLanguageCode(const char* language, char* alpha_3_code)
{
    std::string code;
    const auto result = nebulaRpcCall<frost_bool, std::string>(
                                code, IPC_NAME(NEBULA_NormaliseLanguageCode), std::string(language));
    if (result == frost_true)
    {
        strncpy(alpha_3_code, code.c_str(), max_iso_639_str_length);
    }
    return result;
}