/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaConfigurationIpcAurora class.
 */

#include "NebulaConfigurationIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_configuration.h"
#include "nebula_src/adaptation_layer/ipc/NebulaConfigurationIpcTypes.hpp"
#include "language/LanguageCode.hpp"
#include <rpc/server.h>

void
NebulaConfigurationIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_GetNetworkIds), []() {
                NebulaIpcType<frost_bool, std::vector<frost_int>> ids;
                frost_int count = 0;
                frost_int* network_id_array = nullptr;
                ids.return_value = NEBULA_GetNetworkIds(&count, &network_id_array);
                if (ids.return_value == frost_true)
                {
                    ids.output_params = std::vector<frost_int>(network_id_array,
                                                               network_id_array + count);
                }
                free(network_id_array);
                return ids; });

    server.bind(IPC_NAME(NEBULA_GetDeviceId), []() {
                NebulaIpcType<frost_bool, std::string> id;
                char device_id[NEBULA_DEVICE_ID_LENGTH+1] = { '\0' };
                id.return_value = NEBULA_GetDeviceId(device_id);
                id.output_params = device_id;
                return id; });

    server.bind(IPC_NAME(NEBULA_SetHostingPageUrl),
                [](std::string& url) {
                return NEBULA_SetHostingPageUrl(url.c_str()); });

    server.bind(IPC_NAME(NEBULA_IsAudioDescriptionEnabled), NEBULA_IsAudioDescriptionEnabled);

    server.bind(IPC_NAME(NEBULA_NormaliseLanguageCode),
                [](std::string language) {
                NebulaIpcType<frost_bool, std::string> result;
                char code[max_iso_639_str_length+1] {};
                result.return_value = NEBULA_NormaliseLanguageCode(language.c_str(), code);
                if (result.return_value == frost_true)
                {
                    result.output_params = code;
                }
                return result; });
}
