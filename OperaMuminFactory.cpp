/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file implements UVA factory.
 */
#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include "OperaMuminObjectFactory.hpp"
#include "OperaMuminOipfBackendFactory.hpp"
#if defined USING_SDK422 || defined USING_SDK423
#include "vewd_integration/ipc/OperaBrowserBackendCore.hpp"
#endif
#include "utilities_public/UString.hpp"
#include "uva_backend.h"
#include "uva_client.h"
#include <stdio.h>
// @note Do not use Cabot Trace macros (this code is compiled into a .so)

static OperaMuminObjectFactory* g_mumin_object_factory = 0;

/**
 * Mumin factory implementation
 */
UVABackend*
UVA_RequestBackend(UVABackend::UVABackendType type, UVAClient* client)
{
	printf("++++++++++++++++ UVA_RequestBackend: %d +++++++++++++\n", type);
#if defined USING_SDK422 || defined USING_SDK423
    OperaBrowserBackendCore::instance();
#endif
    if (!OperaMuminObjectFactory::supportedType(type))
    {
        // Avoid unnecessary creation of the object factory.
        return nullptr;
    }

    if (!g_mumin_object_factory)
    {
        g_mumin_object_factory = new OperaMuminObjectFactory();
        if (!g_mumin_object_factory || !g_mumin_object_factory->initialise())
        {
            return nullptr;
        }
    }

    return g_mumin_object_factory->createMuminBackend(type, client);
}

// static
UVA_STATUS
UVA_ReleaseBackend(UVABackend* backend)
{
    if (g_mumin_object_factory)
    {
        g_mumin_object_factory->discardMuminBackend(backend);
    }

    return UVA_OK;
}

UVAOOIFBackendFactory* UVA_RequestOOIFBackendFactory() 
{
	printf("++++++++++++++++ UVA_RequestOOIFBackendFactory+++++++++++++\n");

#if defined USING_SDK422 || defined USING_SDK423
    OperaBrowserBackendCore::instance();
#endif
    return new OperaMuminOipfBackendFactory();
}

long
UVA_GetDefaultStartingBitrate()
{
    return 1.75 * 1000 * 1000; // in bytes
}

bool 
UVA_UsePlatformMSSmoothStreaming()
{
    bool use_platform_ms_smooth_streaming  = false;
    char const *val = getenv("OPERA_USE_BACKEND_FOR_SMOOTH_STREAMING");
    UString choose_value(val);
    UString use_platform_value("1");
    if (choose_value == use_platform_value)
    {
        use_platform_ms_smooth_streaming = true;
    }
    return use_platform_ms_smooth_streaming;
}

