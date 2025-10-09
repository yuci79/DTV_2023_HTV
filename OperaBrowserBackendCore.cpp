/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines OperaBrowserBackendCore.
 */

#include "OperaBrowserBackendCore.hpp"
#include "BrowserCallbackServer.hpp"
#include "BrowserIpcTrace.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaBroadcastComponentsIpcExternal.hpp"
#include "nebula_src/adaptation_layer/NebulaBroadcastComponentsInterface.hpp"
#include "vewd_integration/oipf/EventDispatchConfigurer.hpp"
#include "vewd_integration/jspp_plugins/ipc/browser/NativeOperaWindowManagerCallbacksIpcBrowser.hpp"

#include "OperaBrowserBackendCore.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp"  // for nebulaRpcCall
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"   // for IPC_NAME()

#include "frost/frost_basictypes.h"                             // for frost_bool, frost_true, etc.



#include "frost/frost_debug.h"
#include "frost/frost_time.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/browserbackendcore");

namespace
{

void killBrowserProcess(void*)
{
    TRACE_WARN(("%s()\n", __func__));

    // Allow the trace to be transmitted.
    FROST_Sleep(50);

    std::runtime_error exception("Browser killed by testtool");
    throw exception;
}

void registerTesttool()
{
     FROST_RegisterCommand("kill_browser",
                           "Kill the browser by throwing an exception in the external process",
                            killBrowserProcess,
                            FROST_debug_param_none);
}

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
    NativeOperaWindowManagerCallbacksIpcBrowser::bindToServer(server);
    // Add more here...
}

} // namespace


OperaBrowserBackendCore&
OperaBrowserBackendCore::instance()
{
    TRACE_ALWAYS(("%s() \n", __func__));

    static OperaBrowserBackendCore the_instance;
    return the_instance;
}


bool OperaBrowserBackendCore::initialise()
{
    TRACE_ALWAYS(("%s() \n", __func__));

    // Ask Nebula side to start the backend service
    auto success = nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_MuminStartService), 0);
    return success == frost_true;
}

void OperaBrowserBackendCore::discard()
{
    TRACE_ALWAYS(("%s() \n", __func__));

    // Tell Nebula to stop backend service
    nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_MuminStopService));
}

bool OperaBrowserBackendCore::supportedType(int type)
{
    TRACE_ALWAYS(("%s() \n", __func__));

    // Query Nebula backend status
    auto status = nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_MuminGetStatus));
    return status == 1;
}

bool OperaBrowserBackendCore::createMuminBackend(int type, void* client)
{
    TRACE_ALWAYS(("%s() \n", __func__));

    // If backend not running, start it
    auto status = nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_MuminGetStatus));
    if (status == 0) {
        return initialise();
    }
    return true;
}

OperaBrowserBackendCore::OperaBrowserBackendCore()
{
    TRACE_ALWAYS(("%s() \n", __func__));

    BrowserIpcTrace::instance();

    // BrowserCallbackServer instance must be created before any
    // callbacks/listeners are registered with the Aurora process.
    BrowserCallbackServer::instance(bindCallbackServer);
    
    m_event_dispatch_configurer.initialise();
    m_broadcast_components_interface = new NebulaBroadcastComponentsIpcExternal;
    nebula::setBroadcastComponentAccessor(m_broadcast_components_interface);

    registerTesttool();
}

OperaBrowserBackendCore::~OperaBrowserBackendCore()
{
    TRACE_ALWAYS(("%s() \n", __func__));

    delete m_broadcast_components_interface;    
    m_event_dispatch_configurer.terminate();
}
