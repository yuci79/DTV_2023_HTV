/*
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaWindowManagerProxy class.
 */

#include "config/include_opapp_support.h"
#include "config/using_sdk422.h"

#include "OperaWindowManagerProxy.hpp"
#include "OperaWindowManager.hpp"
#include "OperaWindow.hpp"

#ifdef INCLUDE_OPAPP_SUPPORT
#include "vewd_integration/opapp/OperaOpAppWindow.hpp"
#endif

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/unused.h"

namespace
{
const OperaWindow*
getWindow(const uintptr_t& window_id)
{
    if (!CX_USING_SDK422)
    {
        const OperaWindowHandle handle(window_id); 
        const OperaWindowManager* window_manager = OperaWindowManager::getWindowManager();
        return window_manager ? window_manager->getWindowByHandle(handle) : nullptr;
    }
    else
    {
        SHOULD_NOT_BE_HERE();
        return nullptr;
    }
}
}

OperaWindowManagerProxy::OperaWindowManagerProxy()
{
}

bool
OperaWindowManagerProxy::windowName(const uintptr_t& window_id, cabot::String& name)
{
    if (!CX_USING_SDK422)
    {
        const OperaWindow* window = getWindow(window_id);
        if (window)
        {
            name = window->name();
            return true;
        }
    }
    return false;
}

bool
OperaWindowManagerProxy::windowType(const uintptr_t& window_id, 
                                    OperaWindowType& type)
{
    if (!CX_USING_SDK422)
    {
        const OperaWindow* window = getWindow(window_id);
        if (window)
        {
            type = window->type();
            return true;
        }
    }
    return false;
}

#ifdef INCLUDE_OPAPP_SUPPORT

bool
OperaWindowManagerProxy::opAppWindowType(const uintptr_t& window_id,
                                         nebula::OperatorApplicationType& type)
{
    CABOT_UNUSED_PARAM(window_id);

    if (!CX_USING_SDK422)
    {
        const OperaWindowManager* window_manager = OperaWindowManager::getWindowManager();
        if (window_manager)
        {
            type = window_manager->opAppWindow().opAppType();
            return true;
        }
    }
    return false;
}

#endif
