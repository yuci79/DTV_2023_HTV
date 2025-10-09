/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements trace module initialisation for JS plugins.
 */

#include "utilities_debug/trace.h"

/**
 * @brief Duplicate all the trace module names from jspp_plugins and jsplugins to ensure 
 *        they are all initialised at browser process startup.
 */

namespace PluginTrace_applicationmanager
{
    TRACE_IMPLEMENT("vewd/plugins/applicationmanager");
}

namespace PluginTrace_applicationcontroller
{
    TRACE_IMPLEMENT("vewd/plugins/applicationcontroller");
}

namespace PluginTrace_networkmanager
{
    TRACE_IMPLEMENT("vewd/plugins/networkmanager");
}

namespace PluginTrace_tivoos
{
    TRACE_IMPLEMENT_LEVEL("vewd/plugins/tivoos", TRACE_LEVEL_INFO);
}

namespace PluginTrace_voice
{
    TRACE_IMPLEMENT_LEVEL("vewd/plugins/voice", TRACE_LEVEL_ERROR);
}

// Add more here in unique namespaces
