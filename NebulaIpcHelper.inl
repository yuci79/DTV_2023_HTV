/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Template function definitions for NebulaIpcHelper.
 */

#include "3rd_party_wrapper/rpclib/include/RpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

constexpr bool rethrow_exception = true;

template<typename Return, typename... Args> inline
Return
nebulaRpcCall(const std::string& function_name, Args&&... args)
{
    Return return_value;
    RpcClient& client =
        RpcClient::instance<NebulaIpcPorts::nebula_server_port, rethrow_exception>();
    client.callReturn(return_value, function_name, std::forward<Args>(args)...);
    return return_value;
}

template<typename Return, typename Value, typename NebulaOutputParams, typename... Args> inline
Return
nebulaRpcCall(NebulaOutputParams& output_params, const std::string& function_name, Args&&... args)
{
    NebulaIpcType<Return, Value> ipc_result;
    RpcClient& client =
        RpcClient::instance<NebulaIpcPorts::nebula_server_port, rethrow_exception>();
    client.callReturn(ipc_result, function_name, std::forward<Args>(args)...);
    output_params = ipc_result.output_params;
    return ipc_result.return_value;
}

template<typename... Args>
void
nebulaRpcCall(const std::string& function_name, Args&&... args)
{
    RpcClient& client =
        RpcClient::instance<NebulaIpcPorts::nebula_server_port, rethrow_exception>();
    client.call(function_name, std::forward<Args>(args)...);
}