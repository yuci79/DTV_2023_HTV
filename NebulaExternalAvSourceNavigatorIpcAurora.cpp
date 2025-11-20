/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaExternalAvSourceNavigatorIpcAurora class.
 */

#include "NebulaExternalAvSourceNavigatorIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaExternalAvSourceNavigatorIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaExternalAvSourceNavigatorIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_external_av_source_navigator 'C' functions.
    server.bind(IPC_NAME(NEBULA_GetNumberOfExternalInterfaces), &NEBULA_GetNumberOfExternalInterfaces);

    server.bind(IPC_NAME(NEBULA_GetExternalInterfaceName), [](frost_int index)
    {
        NebulaIpcType<frost_bool, std::string> result;
        char* name = nullptr;

        result.return_value = NEBULA_GetExternalInterfaceName(index, &name);
        if (name != nullptr)
        {
            result.output_params = name;
            free(name);
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_GetExternalInterfaceTypeName), [](frost_int index)
    {
        NebulaIpcType<frost_bool, std::string> result;
        char* name = nullptr;

        result.return_value = NEBULA_GetExternalInterfaceTypeName(index, &name);
        if (name != nullptr)
        {
            result.output_params = name;
            free(name);
        }

        return result;
    });

    server.bind(IPC_NAME(NEBULA_GetCurrentExternalInterfaceIndex), &NEBULA_GetCurrentExternalInterfaceIndex);
    server.bind(IPC_NAME(NEBULA_IsExternalInterfaceSupported), &NEBULA_IsExternalInterfaceSupported);
    server.bind(IPC_NAME(NEBULA_IsExternalInterfaceEnabled), &NEBULA_IsExternalInterfaceEnabled);
    server.bind(IPC_NAME(NEBULA_IsExternalInterfaceDisplayed), &NEBULA_IsExternalInterfaceDisplayed);
    server.bind(IPC_NAME(NEBULA_DeselectExternalInterface), &NEBULA_DeselectExternalInterface);
    server.bind(IPC_NAME(NEBULA_SelectExternalInterface), &NEBULA_SelectExternalInterface);
}
