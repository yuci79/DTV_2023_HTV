/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_signal_info.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaSignalInfoIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_int
NEBULA_SignalInfoGetStrength()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SignalInfoGetStrength));
}

frost_int
NEBULA_SignalInfoGetQuality()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SignalInfoGetQuality));
}

frost_int
NEBULA_SignalInfoGetBer()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SignalInfoGetBer));
}

frost_int
NEBULA_SignalInfoGetSnr()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_SignalInfoGetSnr));
}

frost_bool
NEBULA_SignalInfoGetLock()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SignalInfoGetLock));
}