/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Helper functions for nebula ipc external.
 */

#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcTypesExternal.hpp"
#include <string.h>

template<>
char*
createNebulaFromIpc(const std::string& ipc_item)
{
    return strdup(ipc_item.c_str());
}