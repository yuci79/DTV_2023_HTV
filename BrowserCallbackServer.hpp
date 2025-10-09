/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the class BrowserCallbackServer.
 */

#ifndef CABOT_VEWD_INTEGRATION_IPC_BROWSERCALLBACKSERVER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_BROWSERCALLBACKSERVER_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/external/ExternalCallbackServer.hpp"

/**
 * @brief Acts as a singleton IPC server for callback requests received in the browser process.
 */
class BrowserCallbackServer
{
public:  
    /**
     * @brief Create the Browser callback server. This is threadsafe.
     * @param function_to_bind A source of additional functions to be bound to the rpc server
     *                         during construction.
     */
    static void instance(RpcBoundFunction function_to_bind = nullptr);
};
#endif
