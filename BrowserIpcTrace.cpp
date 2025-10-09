/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class BrowserIpcTrace.
 */

#include "BrowserIpcTrace.hpp"

ExternalIpcTrace&
BrowserIpcTrace::instance()
{
    static ExternalIpcTrace the_instance(NebulaIpcPorts::browser_trace_server_port);
    return the_instance;
}