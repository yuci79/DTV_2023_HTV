/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the FrostDebugCallbacksIpcAurora class.
 */

#include "FrostDebugCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula_src/adaptation_layer/ipc/FrostDebugCallbacksIpcTypes.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/FrostDebugCallbacksIpcAurora");

struct CommandTypeData
{
    bool registered_by_current_browser;
    FROST_DebugParamType param_type;
};

namespace
{
    std::map<std::string, CommandTypeData> command_types;
    std::mutex command_types_mutex;
    std::map<std::string, FROST_DebugCommandCallbackFunction> command_functions;
}

template<const char* cmd_name>
void proxyCommandCallback(void* param)
{
    FrostDebugCallbacksIpcAurora::callCallback(cmd_name, param);
}

void
FrostDebugCallbacksIpcAurora::callCallback(const std::string& cmd_name, void* param)
{
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);

    FROST_DebugParamType param_type;
    {
        std::lock_guard<std::mutex> lock(command_types_mutex);

        const auto iter = command_types.find(cmd_name);
        if (iter == command_types.end())
        {
            TRACE_ERROR(("%s() Unexpected command %s\n", __func__, cmd_name.c_str()));
            SHOULD_NOT_BE_HERE();
            return;
        }
        if (!iter->second.registered_by_current_browser)
        {
            TRACE_INFO(("%s() Restarted browser has not requested %s\n",
                       __func__, cmd_name.c_str()));
            return;
        }
        param_type = iter->second.param_type;
    }
    
    const MP_FROST_RegisterCommandCallbackData data(param_type, param);

    client.call(IPC_NAME(FROST_RegisteredCommandCallback), cmd_name, data);
    TRACE_INFO(("%s() complete\n", __func__));

}

FrostDebugCallbacksIpcAurora::~FrostDebugCallbacksIpcAurora()
{
    for (auto& persisted_string : m_persisted_strings)
    {
        delete persisted_string;
    }

    m_persisted_strings.clear();
}

static constexpr char command_name_temi[] = "TEMI";
static constexpr char command_name_kill_browser[] = "kill_browser";

void
FrostDebugCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    addCommandHandler(command_name_temi, proxyCommandCallback<command_name_temi>);
    addCommandHandler(command_name_kill_browser, proxyCommandCallback<command_name_kill_browser>);

    // Add bindings for frost debug 'C' functions.

    server.bind(IPC_NAME(FROST_RegisterCommand), [this](std::string& cmd_name,
                                            std::string& cmd_desc,
                                            FROST_DebugParamType param_type)
    {
        FROST_DebugCommandCallbackFunction func_ptr = nullptr;
        auto iter = command_functions.find(cmd_name);
        if (iter != command_functions.end())
        {
            func_ptr = iter->second;
        }

        if (func_ptr)
        {
            {
                std::lock_guard<std::mutex> lock(command_types_mutex);
                auto command_type_iter = command_types.find(cmd_name);
                if (command_type_iter != command_types.end())
                {
                    command_type_iter->second.registered_by_current_browser = true;
                    TRACE_INFO(("%s() Already registered %s\n", __func__, cmd_name.c_str()));
                    return frost_true;
                }
                command_types[cmd_name] = { true, param_type };
            }
            auto peristing_cmd_name = new std::string(cmd_name);
            auto peristing_cmd_desc = new std::string(cmd_desc);

            m_persisted_strings.push_back(peristing_cmd_name);
            m_persisted_strings.push_back(peristing_cmd_desc);

            return FROST_RegisterCommand(peristing_cmd_name->c_str(),
                                         peristing_cmd_desc->c_str(),
                                         func_ptr,
                                         param_type);
        }
        else
        {
            TRACE_ERROR(("%s() Unexpected command %s\n", __func__, cmd_name.c_str()));
            SHOULD_NOT_BE_HERE();
            return frost_false;
        }
    });
}

void
FrostDebugCallbacksIpcAurora::reset()
{
    std::lock_guard<std::mutex> lock(command_types_mutex);
    for (auto& command_type : command_types)
    {
        command_type.second.registered_by_current_browser = false;
    }
}

void
FrostDebugCallbacksIpcAurora::addCommandHandler(const std::string& cmd_name,
                                                FROST_DebugCommandCallbackFunction callback)
{
    command_functions[cmd_name] = callback;
}
