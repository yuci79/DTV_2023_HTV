/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaWebBrowserIpcAurora class.
 */

#include "NebulaWebBrowserIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_web_browser.h"
#include "nebula_src/adaptation_layer/ipc/NebulaWebBrowserIpcTypes.hpp"

#include <rpc/server.h>

void
NebulaWebBrowserIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_web_browser 'C' functions.

    server.bind(IPC_NAME(NEBULA_WebBrowserGetStarturlOpened), &NEBULA_WebBrowserGetStarturlOpened);

    server.bind(IPC_NAME(NEBULA_WebBrowserSuspendHbbSystem), &NEBULA_WebBrowserSuspendHbbSystem);

    server.bind(IPC_NAME(NEBULA_WebBrowserResumeHbbSystem), &NEBULA_WebBrowserResumeHbbSystem);

    server.bind(IPC_NAME(NEBULA_WebBrowserSetHbbSystemEnabled),
                &NEBULA_WebBrowserSetHbbSystemEnabled);

    server.bind(IPC_NAME(NEBULA_WebBrowserEnableRepeatForDirectionKeys),
                &NEBULA_WebBrowserEnableRepeatForDirectionKeys);

    server.bind(IPC_NAME(NEBULA_WebBrowserVirtualKeyboardOpened),
                &NEBULA_WebBrowserVirtualKeyboardOpened);

    server.bind(IPC_NAME(NEBULA_WebBrowserVirtualKeyboardClosed),
                &NEBULA_WebBrowserVirtualKeyboardClosed);
}
