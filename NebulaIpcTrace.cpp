/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class NebulaIpcTrace.
 */

#include "NebulaIpcTrace.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "3rd_party_wrapper/rpclib/include/FrostIpcTypes.hpp"

#include <rpc/server.h>

#include <string>
#include <sstream>

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("nebula/ipc/aurora/ipctrace");

namespace
{

#ifdef CABOT_ASSERTIONS

std::map<std::string, int>
unlistedModules(
    const std::map<std::string, int>& modules_to_check,
    const std::map<std::string, int>& modules)
{
    std::map<std::string, int> unlisted_modules;
    for (const auto& module : modules)
    {
        auto found = modules_to_check.find(module.first);
        if (found == modules_to_check.end())
        {
            unlisted_modules.insert(module);
        }
    }
    return unlisted_modules;
}

void
traceUnlistedModules(
    const std::string& prefix,
    const std::map<std::string, int>& modules_to_check,
    const std::map<std::string, int>& modules)
{
    const std::map<std::string, int>& unlisted_modules(
                                    unlistedModules(modules_to_check, modules));
    for (auto const& module : unlisted_modules)
    {
        TRACE_ERROR(("%s: <%s>\n", prefix.c_str(), module.first.c_str()));
    }
}

bool
checkExternalAgainstKnownModules(
    const std::map<std::string, int>& external_modules,
    const std::map<std::string, int>& known_modules)
{
    const bool success = (external_modules.size() == known_modules.size());
    if (!success)
    {
        traceUnlistedModules("missing external module", external_modules, known_modules);
        traceUnlistedModules("unknown external module", known_modules, external_modules);
    }
    return success;
}

#endif // #ifdef CABOT_ASSERTIONS

} // namespace

NebulaIpcTrace::NebulaIpcTrace()
{
    trace_register_external_trace_controller(this);
}

NebulaIpcTrace::~NebulaIpcTrace()
{
    trace_register_external_trace_controller(nullptr);
}

#ifdef _LINUX_PC_PLATFORM
extern void ICE_PC_AddTraceModule(const char*, int);
#endif

void
NebulaIpcTrace::bindToServer(rpc::server& server)
{ 
    TRACE_INFO(("%s()\n", __func__));

    server.bind("FROST_DebugOutTrace", [](std::string& trace_message,
                                          frost_ulong length,
                                          frost_bool enable_socket) {
        FROST_DebugOutTrace(trace_message.data(), length, enable_socket); });

    server.bind("FROST_DebugOut", [](std::string& trace_message,
                                     frost_ulong length) {
        FROST_DebugOut(trace_message.data(), length); });

    server.bind("TRACE_GET_INITIAL_TIME", []() {
        unsigned int initial_time = TRACE_GET_INITIAL_TIME();
        TRACE_INFO(("TRACE_GET_INITIAL_TIME %u\n", initial_time));
        return initial_time; });

    server.bind("INITIALISE_EXTERNAL_TRACE_MODULES",
        [this](const std::map<std::string, int>& external_trace_modules){
            initialise_external_modules(external_trace_modules);
        });
}

void 
NebulaIpcTrace::initialise_external_modules(
    const std::map<std::string, int>& external_trace_modules)
{
    bool initial_startup = false;
    {
        std::lock_guard<std::mutex> lock(m_known_modules_mutex);
        initial_startup = m_known_modules.empty();

        TRACE_INFO(("%s() %zu external, %zu known\n", __func__, 
                        external_trace_modules.size(), m_known_modules.size()));

        if (initial_startup)
        {
            m_known_modules = external_trace_modules;
        }
    }

    if (initial_startup)
    {
        // First time from the external process.
        for (auto& item : external_trace_modules)
        {   
#ifdef _LINUX_PC_PLATFORM
            ICE_PC_AddTraceModule(item.first.c_str(), item.second);
#endif
        }
    }
    else
    {
        // Following an external process restart.
        std::lock_guard<std::mutex> lock(m_known_modules_mutex);
        // This assertion can be avoided by adding any missing modules to PluginTrace.cpp.
        CABOT_ASSERT(checkExternalAgainstKnownModules(external_trace_modules, m_known_modules));

        AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
        for (const auto& external : external_trace_modules)
        {
            const auto& module_name = external.first;
            const int module_level = external.second;
            auto iter = m_known_modules.find(module_name);
            if (iter != m_known_modules.end())
            {
                const int level = iter->second;
                if (level != module_level)
                {
                    TRACE_VERBOSE(("%s() Modifying <%s> level from %d to %d\n", __func__, 
                                    module_name.c_str(), module_level, level));

                    client.call("TRACE_SET_LEVEL", module_name, level);
                }
            }
        }
    }
}

int
NebulaIpcTrace::setLevel(const std::string& module_name, int level)
{
    TRACE_VERBOSE(("%s() <%s> level %d\n", __func__, module_name.c_str(), level));
    int number_adjusted = 0;

    std::lock_guard<std::mutex> lock(m_known_modules_mutex);
    if (m_known_modules.find(module_name) != m_known_modules.end())
    {
        m_known_modules[module_name] = level;
        AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
        client.callReturn(number_adjusted, "TRACE_SET_LEVEL", module_name, level);
    }

    return number_adjusted;
}

void
NebulaIpcTrace::setAllLevels(int level)
{
    TRACE_VERBOSE(("%s() level %d\n", __func__, level));

    AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
    client.call("TRACE_SET_ALL_LEVELS", level);
}

void
NebulaIpcTrace::listModules()
{
    TRACE_VERBOSE(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
    client.call("TRACE_LIST_MODULES");
}

void
NebulaIpcTrace::lineLength(int length)
{
    TRACE_VERBOSE(("%s() length %d\n", __func__, length));

    AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
    client.call("TRACE_SET_LINE_LENGTH", length);
}

void
NebulaIpcTrace::enableTimestamp(unsigned int initial_time)
{
    TRACE_VERBOSE(("%s() initial_time %d\n", __func__, initial_time));

    AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
    client.call("TRACE_SET_INITIAL_TIME", initial_time);
}

void
NebulaIpcTrace::setColourMode(int mode)
{
    TRACE_VERBOSE(("%s() mode %d\n", __func__, mode));

    AuroraRpcClient client(NebulaIpcPorts::browser_trace_server_port);
    client.call("TRACE_SET_COLOUR_MODE", mode);
}
