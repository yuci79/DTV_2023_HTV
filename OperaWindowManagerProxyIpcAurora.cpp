/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the OperaWindowManagerProxyIpcAurora class.
 */
#include "config/include_opapp_support.h"

#include "OperaWindowManagerProxyIpcAurora.hpp"
#include "vewd_integration/ipc/OperaWindowManagerProxyIpcTypes.hpp"
#include "vewd_integration/core/OperaWindowManagerProxy.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "utilities_public/String.hpp"
#include <string>

#include <rpc/server.h>

void
OperaWindowManagerProxyIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for native Window Manager functions.

    server.bind(IPC_NAME(OperaWindowManagerProxy_WindowName), [](uintptr_t window_id)
    {
        cabot::String name;
        OperaWindowManagerProxy window_manager;

        const bool success = window_manager.windowName(window_id, name);
        NebulaIpcType<bool, std::string> result {success, name.c_str()};
        return result;

    });

    server.bind(IPC_NAME(OperaWindowManagerProxy_WindowType), [](uintptr_t window_id)
    {
        OperaWindowType type;
        OperaWindowManagerProxy window_manager;
        const bool success = window_manager.windowType(window_id, type);

        NebulaIpcType<bool, OperaWindowType> result {success, type};
        return result;
    });

#ifdef INCLUDE_OPAPP_SUPPORT
    server.bind(IPC_NAME(OperaWindowManagerProxy_OpAppWindowType), [](uintptr_t window_id)
    {
        nebula::OperatorApplicationType type;
        OperaWindowManagerProxy window_manager;
        const bool success = window_manager.opAppWindowType(window_id, type);

        NebulaIpcType<bool, nebula::OperatorApplicationType> result {success, type};
        return result;
    });

#endif
}
