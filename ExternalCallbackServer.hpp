/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the class ExternalCallbackServer.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_EXTERNALCALLBACKSERVER_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_EXTERNALCALLBACKSERVER_HPP_INCLUDED

#include "3rd_party_wrapper/rpclib/include/CommonIpcServer.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "frost/frost_basictypes.h"

/**
 * @brief Acts as an IPC server for callback requests received in an external process.
 */
class ExternalCallbackServer : private CommonIpcServer
{
public:
    /**
     * @brief Constructor.
     * @param server_port is the ipc port number.
     * @param function_to_bind A source of additional functions to be bound to the rpc server
     *                         during construction.
     */
    ExternalCallbackServer(NebulaIpcPorts server_port, 
                           RpcBoundFunction function_to_bind = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~ExternalCallbackServer() = default;

protected:
    // Implementing CommonIpcServer
    void applyBindings(rpc::server&) override;
};
#endif
