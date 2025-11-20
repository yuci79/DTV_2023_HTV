/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the NebulaNetworkSettingsCallbacksIpcAurora class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULANETWORKSETTINGSCALLBACKSIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULANETWORKSETTINGSCALLBACKSIPCAURORA_HPP_INCLUDED

namespace rpc
{
    class server;
}

/**
 * @brief Class to bind all required nebula_network_settings callback functions.
 */
class NebulaNetworkSettingsCallbacksIpcAurora
{
public:
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
    static bool m_registered_callback;
};

#endif