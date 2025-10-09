/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class BrowserCallbackServer.
 */

#include "BrowserCallbackServer.hpp"
#include "vewd_integration/jspp_plugins/ipc/browser/NativeOperaWindowManagerCallbacksIpcBrowser.hpp"
#include "avcontrolobject/AVControlObjectIpcServer.hpp"

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("opera/browsercallbackserver");

namespace
{

/**
 * @brief Bind to the BrowserCallbackServer.
 *
 * Call bindToServer() for all classes in vewd_integration which need to
 * bind to the BrowserCallbackServer.
 *
 * @param server An rpc::server ready to accept bindings.
 */
void
bindCallbackServer(rpc::server& server)
{
    TRACE_ALWAYS(("Binding callback server methods\n"));

    // Check if NativeOperaWindowManagerCallbacksIpcBrowser is available
    #ifdef INCLUDE_NATIVE_OPERA_WINDOW_MANAGER_CALLBACKS
    NativeOperaWindowManagerCallbacksIpcBrowser::bindToServer(server);
    #endif

    // Bind AVControlObject server methods
    bindAVControlObjectServer(server);

    TRACE_ALWAYS(("Callback server methods bound successfully\n"));
}

} // namespace


void
BrowserCallbackServer::instance(RpcBoundFunction function_to_bind)
{
    TRACE_ALWAYS(("%s() \n", __func__));

    // The ExternalCallbackServer constructor already handles the binding
    // via the function passed to it
    static ExternalCallbackServer the_instance(NebulaIpcPorts::browser_callback_server_port,
                                               bindCallbackServer);

    // If an additional bind function is provided, we need to handle it differently
    // since ExternalCallbackServer doesn't expose getServer()
    if (function_to_bind) {
        TRACE_WARN(("Additional bind function provided but ExternalCallbackServer doesn't expose server interface\n"));
        // Note: You might need to modify ExternalCallbackServer to support this
        // or restructure how additional bindings are handled
    }
}
