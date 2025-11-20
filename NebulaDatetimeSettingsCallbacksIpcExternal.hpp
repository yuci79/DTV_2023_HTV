/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the NebulaDatetimeSettingsCallbacksIpcExternal class.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULADATETIMESETTINGSCALLBACKSIPCEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULADATETIMESETTINGSCALLBACKSIPCEXTERNAL_HPP_INCLUDED

namespace rpc
{
    class server;
}

/**
 * @brief Binds functions which receive NEBULA_ callbacks to an external RPC server.
 */
class NebulaDatetimeSettingsCallbacksIpcExternal
{
public:
    /**
      * @brief An invitation to bind functors to an rpc::server.
      * @param server An rpc::server ready to accept bindings.
      */
    static void bindToServer(rpc::server& server);
};

#endif
