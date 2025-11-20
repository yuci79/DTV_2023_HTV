/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class ExternalIpcTrace.
 */

#include "ExternalIpcTrace.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "3rd_party_wrapper/rpclib/include/FrostIpcTypes.hpp"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"

#include <rpc/client.h>
#include <rpc/server.h>
#include <rpc/rpc_error.h>

#include <map>
#include <memory>
#include <string>
#ifdef DEBUG 
#include <sstream>
#include <iostream>
#endif

ExternalIpcTrace::ExternalIpcTrace(NebulaIpcPorts server_port,
                                   std::uint64_t timeout_ms)
    : CommonIpcServer(server_port, __func__)
    , m_client(ipc_localhost, NebulaIpcPorts::nebula_server_port)
{
    tryAndCatchRpcErrors([&]
    {
        if (timeout_ms > 0)
        {
            m_client.set_timeout(timeout_ms);
        }

        auto initial_time = m_client.call("TRACE_GET_INITIAL_TIME").as<unsigned int>();
        TRACE_SET_INITIAL_TIME(initial_time);
    });

    CommonIpcServer::applyBindings();
    initialise();
}

ExternalIpcTrace::~ExternalIpcTrace()
{
}

void
ExternalIpcTrace::applyBindings(rpc::server& server)
{
    // For trace level setting.
    server.bind("TRACE_SET_LEVEL", [](std::string& module_name, int level) {
        return TRACE_SET_LEVEL(module_name.data(), level); });

    server.bind("TRACE_SET_ALL_LEVELS", [](int level) {
        TRACE_SET_ALL_LEVELS(level); });

    server.bind("TRACE_LIST_MODULES", &trace_list_modules);

    server.bind("TRACE_SET_LINE_LENGTH", &trace_set_line_length);

    server.bind("TRACE_SET_INITIAL_TIME", [](unsigned int new_initial_time) {
        TRACE_SET_INITIAL_TIME(new_initial_time); });

    server.bind("TRACE_SET_COLOUR_MODE", &trace_set_colour_mode);
}

void
ExternalIpcTrace::reportError(const std::string& buffer) const
{
#ifdef DEBUG 
    std::cout << buffer << std::endl;
#else
    CABOT_UNUSED_PARAM(buffer);
#endif
}

void
ExternalIpcTrace::sendDebugOut(const char* message, frost_ulong length)
{
    tryAndCatchRpcErrors([&]
    {
        m_client.send("FROST_DebugOut", message, length);
    });
}

void
ExternalIpcTrace::sendDebugOutTrace(const char* message, frost_ulong length, 
                                   frost_bool enable_socket)
{
    tryAndCatchRpcErrors([&]
    {
        m_client.send("FROST_DebugOutTrace", message, length, enable_socket);
    });
}

void 
ExternalIpcTrace::callDebugOut(const std::string& message)
{
    tryAndCatchRpcErrors([&]
    {
        m_client.call("FROST_DebugOut", message.c_str(), message.length());
    });
}

void 
ExternalIpcTrace::initialise()
{
    std::map<std::string, int> trace_modules;

    const unsigned short module_count = trace_get_number_of_modules();
    for (unsigned short module_index = 0; module_index < module_count; module_index++)
    {
        char module_name[256] = {};
        trace_get_module_name(module_index, module_name);
        if (module_name)
        {
            const int level = trace_get_level_from_index(module_index);
            trace_modules[module_name] = level;
        }
    }

    if (!trace_modules.empty())
    {
        tryAndCatchRpcErrors([&]
        {
            m_client.call("INITIALISE_EXTERNAL_TRACE_MODULES", trace_modules);
        });
    }
}

template <typename Callable>
void
ExternalIpcTrace::tryAndCatchRpcErrors(Callable&& func)
{
    try
    {
        func();
    }
    catch(rpc::rpc_error& e)
    {
        handleRpcError(e);
    }
    catch(const std::exception& e)
    {
        handleStdException(e);
    }
}
