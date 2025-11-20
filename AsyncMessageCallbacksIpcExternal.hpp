/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the AsyncMessageCallbacksIpcExternal class and a template helper class.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_ASYNCMESSAGECALLBACKSIPCEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_ASYNCMESSAGECALLBACKSIPCEXTERNAL_HPP_INCLUDED

#include <vector>
#include <mutex>

class AnyNebulaAsyncMessageProcessor;

namespace rpc
{
    class server;
}

/**
 * @brief Binds functions which receive Async Message callbacks to an external RPC server.
 */
class
AsyncMessageCallbacksIpcExternal
{
public:
    /**
      * @brief An invitation to bind functors to an rpc::server.
      * @param server An rpc::server ready to accept bindings.
      */
    static void bindToServer(rpc::server& server);

};
#endif
