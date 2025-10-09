/*
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Proxy to OperaWindowManager
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOWMANAGERPROXY_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAWINDOWMANAGERPROXY_HPP_INCLUDED

#include "config/include_opapp_support.h"

#include "nebula/browser_interface/OperatorApplicationType.hpp"
#include "vewd_integration/core/OperaWindowTypes.hpp"
#include "utilities_public/String.hpp"
#include <cstdint>

class OperaWindowManagerProxy
{
public:
    /**
     * @brief OperaWindowManagerProxy constructor.
     */
    OperaWindowManagerProxy();

    /**
     * @brief Get window name.
     * @param handle is the window handle.
     * @param[out] name is the window name.
     * @return true if name returned.
     */
    bool windowName(const uintptr_t& window_id, cabot::String& name);
    
    /**
     * @brief Get window type.
     * @param handle is the window handle.
     * @param[out] type is the window type.
     * @return true if type returned.
     */
    bool windowType(const uintptr_t& window_id, OperaWindowType& type);

#ifdef INCLUDE_OPAPP_SUPPORT
    /**
     * @brief Get opapp window type.
     * @param handle is the window handle.
     * @param[out] type is the opapp window type.
     * @return true if type returned.
     */
    bool opAppWindowType(const uintptr_t& window_id, nebula::OperatorApplicationType& type);
#endif
};

#endif
