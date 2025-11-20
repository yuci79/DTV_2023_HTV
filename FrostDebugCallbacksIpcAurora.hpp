/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the FrostDebugCallbacksIpcAurora class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_FROSTDEBUGCALLBACKSIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_FROSTDEBUGCALLBACKSIPCAURORA_HPP_INCLUDED

#include "frost/frost_debug.h"
#include <string>
#include <vector>

namespace rpc
{
    class server;
}

/**
 * @brief Class to bind all required frost debug functions.
 */
class FrostDebugCallbacksIpcAurora
{
public:
    /**
     * @brief Destructor.
     */
    ~FrostDebugCallbacksIpcAurora();

    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */ 
    void bindToServer(rpc::server& server);

    /**
     * @brief Reset all callbacks, typically after the ipc external process has shutdown.
     */ 
    static void reset();

    /**
     * @brief Call command handler.
     * @param cmd_name Name of the command that the user types at the console.
     * @param param The param to be passed to the callback.
     */ 
    static void callCallback(const std::string& cmd_name, void* param);

    /**
     * @brief Add a command handler.
     * @param cmd_name Name of the command that the user types at the console.
     * @param callback The function to be called when the command is invoked.
     */ 
    static void addCommandHandler(const std::string& cmd_name,
                                  FROST_DebugCommandCallbackFunction callback);

private:
    std::vector<std::string*> m_persisted_strings;
};

#endif
