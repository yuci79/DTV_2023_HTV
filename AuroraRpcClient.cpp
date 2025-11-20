/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the AuroraRpcClient class.
*/
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"

AuroraRpcClient::AuroraRpcClient(const NebulaIpcPorts destination_port)
    : RpcClient(destination_port,
                false,
                callback_timeout_ms,
                check_server_during_call)
{

}
