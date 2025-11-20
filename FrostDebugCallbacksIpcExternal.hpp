/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the FrostDebugCallbacksIpcExternal class and a template helper class.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_FROSTDEBUGCALLBACKSIPCEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_FROSTDEBUGCALLBACKSIPCEXTERNAL_HPP_INCLUDED


#include "nebula_src/adaptation_layer/ipc/FrostDebugCallbacksIpcTypes.hpp"
#include <mutex>

namespace rpc
{
    class server;
}

/**
 * @brief Binds functions which receive Frost Debug callbacks to an external RPC server.
 */
class
FrostDebugCallbacksIpcExternal
{
public:
    /**
      * @brief An invitation to bind functors to an rpc::server.
      * @param server An rpc::server ready to accept bindings.
      */
    static void bindToServer(rpc::server& server);

    /**
     * @brief Transmit a FROST_RegisterCommand() request to nebula and block until acknowledged.
     * @param cmd_name See FROST_RegisterCommand().
     * @param cmd_desc See FROST_RegisterCommand().
     * @param callback See FROST_RegisterCommand().
     * @param param_type See FROST_RegisterCommand().
     * @return See FROST_RegisterCommand().
     */
    static frost_bool registerCommand(const char* cmd_name, const char* cmd_desc,
                                      FROST_DebugCommandCallbackFunction callback,
                                      FROST_DebugParamType param_type);

private:
    static void proxyRegisteredCommandCallback(
                            const std::string& command_name,
                            const MP_FROST_RegisterCommandCallbackData& data);

private:
    static std::map<std::string, FROST_DebugCommandCallbackFunction> m_command_map;
    static std::mutex m_command_map_mutex;
};

#endif