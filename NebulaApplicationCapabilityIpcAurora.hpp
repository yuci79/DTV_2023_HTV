/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the NebulaApplicationCapabilityIpcAurora class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAAPPLICATIONCAPABILITYIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAAPPLICATIONCAPABILITYIPCAURORA_HPP_INCLUDED

namespace rpc
{
    class server;
}

/**
 * @brief Class to bind all required nebula_application_capability functions.
 */
class NebulaApplicationCapabilityIpcAurora
{
public:
    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */ 
    static void bindToServer(rpc::server& server);
};

#endif
