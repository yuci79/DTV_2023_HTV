#pragma once

#include <rpc/server.h>

class NebulaBrowserBackendIpcAurora
{
public:
    // Register all Opera backend IPC bindings into the RPC server
    static void bindToServer(rpc::server& server);
    void reset();
};
