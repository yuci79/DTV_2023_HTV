/*
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the OperaWindowManagerProxyIpcAurora class.
 */

#ifndef CABOT_VEWD_INTEGRATION_IPC_AURORA_OPERAWINDOWMANAGERPROXYIPCAURORA_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AURORA_OPERAWINDOWMANAGERPROXYIPCAURORA_HPP_INCLUDED

namespace rpc
{
    class server;
}

/**
 * @brief Class to bind all required OperaWindowManagerProxy functions.
 */
class OperaWindowManagerProxyIpcAurora
{
public:
    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */ 
    static void bindToServer(rpc::server& server);
};

#endif
