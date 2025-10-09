/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Definitions for the Vewd Aurora IPC server.
 */

#include "VewdAuroraIpcServer.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "vewd_common/ipc/aurora/BpiDeviceInfoIpcAurora.hpp"
#include "vewd_common/ipc/aurora/BpiGraphicsIpcAurora.hpp"
#include "vewd_common/ipc/aurora/BpiInternetStatusIpcAurora.hpp"
#include "vewd_common/ipc/aurora/BpiNativeUiControllerIpcAurora.hpp"
#include "vewd_common/ipc/aurora/BpiProfileIpcAurora.hpp"
#include "vewd_common/ipc/aurora/BpiWindowManagerIpcAurora.hpp"

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("vewd_common/ipc/aurora/ipcserver");

VewdAuroraIpcServer::VewdAuroraIpcServer(RpcBoundFunction function_to_bind,
                                         RpcResetFunction reset_function)
    : CommonIpcServer(NebulaIpcPorts::vewd_aurora_server_port, __func__)
    , m_reset_function(reset_function)
{
    CommonIpcServer::applyBindings(function_to_bind);
}

void 
VewdAuroraIpcServer::resetCallbacks()
{
    TRACE_INFO(("%s()\n", __func__));

    // Add more here.
    
    if (m_reset_function)
    {
        m_reset_function();
    }
}

void
VewdAuroraIpcServer::applyBindings(rpc::server& server)
{
    BpiDeviceInfoIpcAurora::bindToServer(server);
    BpiGraphicsIpcAurora::bindToServer(server);
    BpiInternetStatusIpcAurora::bindToServer(server);
    BpiNativeUiControllerIpcAurora::bindToServer(server);
    BpiProfileIpcAurora::bindToServer(server);
    BpiWindowManagerIpcAurora::bindToServer(server);

    // Add more here.
}
