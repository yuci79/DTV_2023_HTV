/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the class BrowserIpcTrace.
 */

#ifndef CABOT_VEWD_INTEGRATION_IPC_BROWSERIPCTRACE_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_BROWSERIPCTRACE_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/external/ExternalIpcTrace.hpp"

/**
 * @brief Acts as a singleton IPC server for trace control requests received in the browser process.
 */
class BrowserIpcTrace
{
public:  
    static ExternalIpcTrace& instance();
};

#endif
