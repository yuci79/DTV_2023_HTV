/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the Nebula IPC server.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAIPCSERVER_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAIPCSERVER_HPP_INCLUDED

#include "FrostDebugCallbacksIpcAurora.hpp"
#include "NebulaIpcTrace.hpp"
#include "3rd_party_wrapper/rpclib/include/CommonIpcServer.hpp"

/**
 * @brief An IPC server for Nebula calls from a separate browser process.
 */
class NebulaIpcServer : private CommonIpcServer
{
public:
    /**
     * @brief Create the IPC server.
     */
    NebulaIpcServer();

    /**
     * @brief Reset all IPC callback handlers to remove registrations from IPC external.
     * @note Typically called after a browser shutdown event.
     */
    void resetCallbacks();

protected:
    void applyBindings(rpc::server& server) override;

private:
    NebulaIpcTrace m_ipc_trace;
    FrostDebugCallbacksIpcAurora m_ipc_frost_debug;
};

#endif
