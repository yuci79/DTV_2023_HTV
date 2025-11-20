/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the FrostDebugCallbacksIpcExternal class and some associated functions.
 */

#include "FrostDebugCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/FrostDebugCallbacksIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/FrostDebugCallbacksIpcExternal");

std::map<std::string, FROST_DebugCommandCallbackFunction> FrostDebugCallbacksIpcExternal::m_command_map;
std::mutex FrostDebugCallbacksIpcExternal::m_command_map_mutex;

void FrostDebugCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(FROST_RegisteredCommandCallback), proxyRegisteredCommandCallback);
}

frost_bool FROST_RegisterCommand(const char* cmd_name, const char* cmd_desc,
                                 FROST_DebugCommandCallbackFunction callback,
                                 FROST_DebugParamType param_type)
{
    return FrostDebugCallbacksIpcExternal::registerCommand(cmd_name, cmd_desc,
                                                           callback, param_type);
}

frost_bool
FrostDebugCallbacksIpcExternal::registerCommand(const char* cmd_name,
                                      const char* cmd_desc,
                                      FROST_DebugCommandCallbackFunction callback,
                                      FROST_DebugParamType param_type)
{
    {
        std::lock_guard<std::mutex> lock(m_command_map_mutex);
        m_command_map[cmd_name] = callback;
    }

    const auto success = nebulaRpcCall<frost_bool>(IPC_NAME(FROST_RegisterCommand),
                                                   std::string(cmd_name),
                                                   std::string(cmd_desc),
                                                   param_type);
    if (!success)
    {
        std::lock_guard<std::mutex> lock(m_command_map_mutex);
        m_command_map.erase(cmd_name);
    }
    return success ? frost_true : frost_false;
}

void
FrostDebugCallbacksIpcExternal::proxyRegisteredCommandCallback(const std::string& command_name,
                                                 const MP_FROST_RegisterCommandCallbackData& data)
{
    FROST_DebugCommandCallbackFunction callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(m_command_map_mutex);
        const auto iter = m_command_map.find(command_name);
        if (iter != m_command_map.end())
        {   
            callback = iter->second;
        }
    }
    if (callback)
    {
        FROST_DebugParamStringInt param_string_int;
        param_string_int.string_part = nullptr;
        void* param = nullptr;

        if (data.m_param_null == false)
        {
            switch (data.m_param_type)
            {
            default:
                break;
            case FROST_debug_param_int:
                param_string_int.int_part = data.m_int;
                param = &(param_string_int.int_part);
                break;
            case FROST_debug_param_string_int:
                param_string_int.int_part = data.m_int;
                if (!data.m_string_sub_param_null)
                {
                    param_string_int.string_part = new char[data.m_string.length() + 1];
                    strcpy(param_string_int.string_part, data.m_string.c_str());
                }
                param = &param_string_int;
                break;
            case FROST_debug_param_string:
            case FROST_debug_param_string_none:
                param_string_int.string_part = new char[data.m_string.length() + 1];
                strcpy(param_string_int.string_part, data.m_string.c_str());
                param = param_string_int.string_part;
                break;
            }
        }

        callback(param);

        delete[] param_string_int.string_part;
    }
}
