/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_web_browser.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAWEBBROWSERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAWEBBROWSERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_WebBrowserGetStarturlOpened);
DECLARE_IPC_NAME(NEBULA_WebBrowserSuspendHbbSystem);
DECLARE_IPC_NAME(NEBULA_WebBrowserResumeHbbSystem);
DECLARE_IPC_NAME(NEBULA_WebBrowserSetHbbSystemEnabled);
DECLARE_IPC_NAME(NEBULA_WebBrowserEnableRepeatForDirectionKeys);
DECLARE_IPC_NAME(NEBULA_WebBrowserVirtualKeyboardOpened);
DECLARE_IPC_NAME(NEBULA_WebBrowserVirtualKeyboardClosed);

#endif
