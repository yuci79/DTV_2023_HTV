/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional IPC Types for
 *        OperaWindowManagerProxy.
 */

#ifndef CABOT_VEWD_INTEGRATION_IPC_OPERAWINDOWMANAGERPROXYIPCTYPES_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_OPERAWINDOWMANAGERPROXYIPCTYPES_HPP_INCLUDED

#include "3rd_party_wrapper/rpclib/include/IpcTypes.hpp"
#include "vewd_integration/core/OperaWindowTypes.hpp" 
#include "nebula/browser_interface/OperatorApplicationType.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(OperaWindowManagerProxy_WindowName);
DECLARE_IPC_NAME(OperaWindowManagerProxy_WindowType);
DECLARE_IPC_NAME(OperaWindowManagerProxy_OpAppWindowType);


/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(OperaWindowType);
MSGPACK_ADD_ENUM(nebula::OperatorApplicationType);

#endif
