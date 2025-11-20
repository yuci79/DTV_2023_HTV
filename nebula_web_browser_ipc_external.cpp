/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_web_browser.
 */

#include "nebula/adaptation_layer/nebula_web_browser.h"
#include "nebula_src/adaptation_layer/ipc/NebulaWebBrowserIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/cabot_assert.h"

frost_bool
NEBULA_WebBrowserGetStarturlOpened(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_WebBrowserGetStarturlOpened));
}

frost_bool 
NEBULA_WebBrowserSuspendHbbSystem(frost_bool bind_to_current_channel)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_WebBrowserSuspendHbbSystem),
                                              bind_to_current_channel);
}

frost_bool NEBULA_WebBrowserResumeHbbSystem()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_WebBrowserResumeHbbSystem));
}

void
NEBULA_WebBrowserSetHbbSystemEnabled(frost_bool enable)
{
    nebulaRpcCall(IPC_NAME(NEBULA_WebBrowserSetHbbSystemEnabled), enable);
}

void
NEBULA_WebBrowserEnableRepeatForDirectionKeys(frost_bool enable)
{
    nebulaRpcCall(IPC_NAME(NEBULA_WebBrowserEnableRepeatForDirectionKeys), enable);
}

void
NEBULA_WebBrowserVirtualKeyboardOpened(void)
{
    nebulaRpcCall(IPC_NAME(NEBULA_WebBrowserVirtualKeyboardOpened));
}

void
NEBULA_WebBrowserVirtualKeyboardClosed(void)
{
    nebulaRpcCall(IPC_NAME(NEBULA_WebBrowserVirtualKeyboardClosed));
}
