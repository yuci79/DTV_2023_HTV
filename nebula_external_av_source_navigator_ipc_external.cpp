/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_external_av_source_navigator.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaExternalAvSourceNavigatorIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C"  frost_int
NEBULA_GetNumberOfExternalInterfaces(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetNumberOfExternalInterfaces));
}

extern "C" frost_bool
NEBULA_GetExternalInterfaceName(frost_int index, char** name)
{
    std::string name_str;

    auto result = nebulaRpcCall<frost_bool, std::string>(name_str,
                                                         IPC_NAME(NEBULA_GetExternalInterfaceName),
                                                         index);
    if (result == frost_true)
    {
        *name = static_cast<char*>(malloc(sizeof(char) * name_str.size() + 1));
        snprintf(*name, name_str.size() + 1, "%s", name_str.c_str());
    }

    return result;
}

extern "C" frost_bool
NEBULA_GetExternalInterfaceTypeName(frost_int index, char** name)
{
    std::string name_str;

    auto result = nebulaRpcCall<frost_bool, std::string>(name_str,
                                                     IPC_NAME(NEBULA_GetExternalInterfaceTypeName),
                                                     index);
    if (result == frost_true)
    {
        *name = static_cast<char*>(malloc(sizeof(char) * name_str.size() + 1));
        snprintf(*name, name_str.size() + 1, "%s", name_str.c_str());
    }

    return result;
}

extern "C" frost_int
NEBULA_GetCurrentExternalInterfaceIndex(NEBULA_DisplayWindowId window)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetCurrentExternalInterfaceIndex), window);
}

extern "C" frost_bool
NEBULA_IsExternalInterfaceSupported(frost_int index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsExternalInterfaceSupported), index);
}

extern "C" frost_bool
NEBULA_IsExternalInterfaceEnabled(frost_int index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsExternalInterfaceEnabled), index);
}

extern "C" frost_bool
NEBULA_IsExternalInterfaceDisplayed(NEBULA_DisplayWindowId window)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsExternalInterfaceDisplayed), window);
}

extern "C" void
NEBULA_DeselectExternalInterface(NEBULA_DisplayWindowId window)
{
    nebulaRpcCall(IPC_NAME(NEBULA_DeselectExternalInterface), window);
}

extern "C" void
NEBULA_SelectExternalInterface(frost_int index, NEBULA_DisplayWindowId window)
{
    nebulaRpcCall(IPC_NAME(NEBULA_SelectExternalInterface), index, window);
}
