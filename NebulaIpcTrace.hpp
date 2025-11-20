/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the Nebula IPC trace handler.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAIPCTRACE_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAIPCTRACE_HPP_INCLUDED

#include "utilities_debug/AnyExternalTraceController.hpp"

#include <map>
#include <mutex>
#include <string>

namespace rpc
{ 
class server;
} 

/**
 * @brief Implements an AnyExternalTraceController to relay trace control requests
 *        to the browser process.
 *        Provides functors for an rpc::server to receive:
 *            FROST_Debug* requests containing formatted trace text.
 *            Requests concerned with synchronising timestamps between aurora and browser. 
 */
class NebulaIpcTrace : public AnyExternalTraceController
{
public:
    /**
     * @brief Constructor.
     */
    NebulaIpcTrace();

    /**
     * @brief Destructor.
     */
    virtual ~NebulaIpcTrace();
    
    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */
    void bindToServer(rpc::server& server);

    // Implementing  AnyExternalTraceController
    int setLevel(const std::string& module_name, int level) override;
    void setAllLevels(int level) override;
    void listModules() override;
    void lineLength(int length) override;
    void enableTimestamp(unsigned int initial_time) override;
    void setColourMode(int mode) override;

private:
    void initialise_external_modules(const std::map<std::string, int>& external_trace_modules);

private:
    std::mutex m_known_modules_mutex;
    std::map<std::string, int> m_known_modules;
};
#endif // CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULAIPCTRACE_HPP_INCLUDED
