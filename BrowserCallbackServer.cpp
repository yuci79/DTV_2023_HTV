// ============================================================================
// CRITICAL ISSUE #1: BrowserCallbackServer.cpp is NOT calling bindAVControlObjectServer
// FILE: BrowserCallbackServer.cpp (FIXED VERSION)
// ============================================================================

#include "BrowserCallbackServer.hpp"
#include "vewd_integration/jspp_plugins/ipc/browser/NativeOperaWindowManagerCallbacksIpcBrowser.hpp"
#include "avcontrolobject/AVControlObjectIpcServer.hpp"  // CRITICAL: Must include this

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("opera/browsercallbackserver");

namespace
{

void bindCallbackServer(rpc::server& server)
{
    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("bindCallbackServer - START\n"));
    TRACE_ALWAYS(("=================================================\n"));

    #ifdef INCLUDE_NATIVE_OPERA_WINDOW_MANAGER_CALLBACKS
    TRACE_ALWAYS(("Binding NativeOperaWindowManagerCallbacks...\n"));
    NativeOperaWindowManagerCallbacksIpcBrowser::bindToServer(server);
    TRACE_ALWAYS(("NativeOperaWindowManagerCallbacks bound\n"));
    #endif

    // CRITICAL FIX: This was missing in your BrowserCallbackServer.cpp
    TRACE_ALWAYS(("Binding AVControlObject server methods...\n"));
    //AVControlObjectIpcServer::bindToServer(server);  // THIS MUST BE CALLED!
    TRACE_ALWAYS(("AVControlObject server methods bound\n"));

    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("bindCallbackServer - COMPLETE\n"));
    TRACE_ALWAYS(("=================================================\n"));
}

} // namespace

void BrowserCallbackServer::instance(RpcBoundFunction function_to_bind)
{
    TRACE_ALWAYS(("BrowserCallbackServer::instance() START\n"));

    static ExternalCallbackServer the_instance(
        NebulaIpcPorts::browser_callback_server_port,
        bindCallbackServer);

    if (function_to_bind) {
        TRACE_WARN(("Additional bind function provided but not used\n"));
    }

    TRACE_ALWAYS(("BrowserCallbackServer::instance() COMPLETE\n"));
}
