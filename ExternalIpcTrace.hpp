/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the class ExternalIpcTrace.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_EXTERNALIPCTRACE_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_EXTERNALIPCTRACE_HPP_INCLUDED

#include "3rd_party_wrapper/rpclib/include/CommonIpcServer.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "frost/frost_basictypes.h"

#include <rpc/client.h>

/**
 * @brief Acts as an IPC server for trace control requests received in an external process.
 *        Also has an IPC client sending FROST_DebugOutTrace and FROST_DebugOut output to 
 *        the aurora process.
 */
class ExternalIpcTrace : private CommonIpcServer
{
public:  
    /**
     * @brief Constructor.
     * @param server_port is the ipc port number.
     * @param timeout_ms The overall time limit on each rpc::client::call().
     * @note A timeout_ms value of greater than zero will enable the timeout functionality.
     */
    ExternalIpcTrace(NebulaIpcPorts server_port, std::uint64_t timeout_ms = 0);
    
    /**
     * @brief Destructor.
     */
    virtual ~ExternalIpcTrace();

    /**
     * @brief Send a FROST_DebugOut() request to nebula.
     * @param message See FROST_DebugOut().
     * @param length See FROST_DebugOut().
     */
    void sendDebugOut(const char* message, frost_ulong length);

    /**
     * @brief Send a FROST_DebugOutTrace() request to nebula.
     * @param message See FROST_DebugOutTrace().
     * @param length See FROST_DebugOutTrace().
     * @param enable_socket See FROST_DebugOutTrace().
     */
    void sendDebugOutTrace(const char* message, frost_ulong length, frost_bool enable_socket);

    /**
     * @brief Transmit a FROST_DebugOut() request to nebula and block until acknowledged.
     * @param message The message text to be transmitted.
     */
    void callDebugOut(const std::string& message);

public:
    ExternalIpcTrace(const ExternalIpcTrace&) = delete;
    ExternalIpcTrace& operator=(const ExternalIpcTrace&) = delete;
    
protected:
    // Implementing CommonIpcServer
    void applyBindings(rpc::server&) override;
    void reportError(const std::string& buffer) const override;

private:
    template <typename Callable> void tryAndCatchRpcErrors(Callable&& func);

private:
    void initialise();
    
private:
    rpc::client m_client;
};

#endif
