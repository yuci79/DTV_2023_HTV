/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the Vewd Aurora IPC server.
 */

#ifndef CABOT_VEWD_COMMON_IPC_AURORA_VEWDAURORAIPCSERVER_HPP_INCLUDED
#define CABOT_VEWD_COMMON_IPC_AURORA_VEWDAURORAIPCSERVER_HPP_INCLUDED

#include "3rd_party_wrapper/rpclib/include/CommonIpcServer.hpp"

#include <vector>

using RpcResetFunction = std::function<void(void)>;

/**
 * @brief An IPC server for Vewd Aurora calls from a separate browser process.
 */
class VewdAuroraIpcServer : private CommonIpcServer
{
public:
    /**
     * @brief Create the IPC server.
     * @param function_to_bind A source of additional functions to be bound to the rpc server
     *                         during construction.
     * @param reset_function A function to reset additional IPC callback handlers.
     */
    VewdAuroraIpcServer(RpcBoundFunction function_to_bind = nullptr,
                        RpcResetFunction reset_function = nullptr);

    /**
     * @brief Reset all IPC callback handlers to remove registrations from IPC external.
     * @note Typically called after a browser shutdown event.
     */
    void resetCallbacks();

protected:
    void applyBindings(rpc::server& server) override;

private:
    RpcResetFunction m_reset_function;
};

#endif