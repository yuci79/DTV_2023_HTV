/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the AsyncMessageCallbacksIpcAurora class.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_ASYNCMESSAGECALLBACKSIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_ASYNCMESSAGECALLBACKSIPCAURORA_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/AsyncMessageProcessorIpcTypes.hpp"
#include <array>

class AnyNebulaAsyncMessageProcessor;

namespace rpc
{
    class server;
}

/**
 * @brief Class to manage registration and deregistration of AsyncMessageProcessors required 
 *        by the external peer. 
 * @note  Only one processor of each type is expected, multiple instances in external are 
 *        managed by the external peer.
 */
class AsyncMessageCallbacksIpcAurora
{
public:
    /**
     * @brief Constructor.
     */
    AsyncMessageCallbacksIpcAurora();

    /**
     * @brief Creates an AsyncMessageProcessor of the given type, and registers it
     *        with nebula and stores the handle for later deregistration.
     * @param type Indicates the type of AsyncMessageProcessor required.
     * @return true if a new processor was created and registered, otherwise false.
     */
    bool createProcessor(AsyncMesssageProcessorRegistrationIpcType type);

    /**
     * @brief The previously registered AsyncMessageProcessor of the given type is deregistered
     *        with nebula.
     * @param type Indicates the type of AsyncMessageProcessor required.
     * @return true if a processor was found and deregistered, otherwise false.
     */
    bool removeProcessor(AsyncMesssageProcessorRegistrationIpcType type);

    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */ 
    static void bindToServer(rpc::server& server);

    /**
     * @brief Reset all callbacks, typically after the ipc external process has shutdown.
     */ 
    static void reset();

private:
    void deregisterProcessors();
    
    // Non-custodial container of registered processor handles. 
    // The pointers must not be deleted by this class.
    std::array<AnyNebulaAsyncMessageProcessor*, MaxRegistrationIpcTypes> m_processor_handles;
};

#endif
