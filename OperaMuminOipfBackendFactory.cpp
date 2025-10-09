/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief TODO: Brief description of the purpose of this code
 */
#include "OperaMuminOipfBackendFactory.hpp"

#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include "frost/frost_basictypes.h"
#include "nebula/adaptation_layer/nebula_channel_info.h"
#include "nebula/adaptation_layer/nebula_pvr_disk_controller.h"
#include "vewd_integration/oipf/OperaOipfCapabilities.hpp"
#include "vewd_integration/oipf/OperaOipfChannel.hpp"
#include "vewd_integration/oipf/search_manager/OperaOipfSearchManager.hpp"
#include "vewd_integration/oipf/parental_control_manager/OperaOipfParentalControlManager.hpp"
#include "vewd_integration/oipf/OperaOipfMasterConfiguration.hpp"
#include "vewd_integration/oipf/OperaOipfObjectFactory.hpp"
#include "vewd_integration/oipf/OperaOipfRecordingScheduler.hpp"

#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"

TRACE_IMPLEMENT("mumin/backend_factory");

void init_opera_mumin_oipf_backend_factory()
{
    // Defined to stop unreferenced classes/functions from being removed
    // at link time, that are required at runtime by Vewd shared libraries.
}

OperaMuminOipfBackendFactory::OperaMuminOipfBackendFactory()
{}

OperaMuminOipfBackendFactory::~OperaMuminOipfBackendFactory()
{}

UVA_STATUS OperaMuminOipfBackendFactory::getChannel(
    const std::string& ccid, std::shared_ptr<UVAChannel>& result)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    frost_int channel_id = NEBULA_GetChannelIdByCCID(ccid.c_str());
    if (channel_id == -1)
    {
        return UVA_OK;
    }

    NEBULA_Channel nebula_channel;
    if (NEBULA_GetChannelInfoByID(channel_id, &nebula_channel))
    {
        UVAChannel* uva_channel = new OperaOipfChannel(nebula_channel);
        result.reset(uva_channel);
    }

    return UVA_OK;
}

UVA_STATUS OperaMuminOipfBackendFactory::createCapabilities(
    std::shared_ptr<UVACapabilities>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    backend.reset(new OperaOipfCapabilities());
    return UVA_OK;
}

UVA_STATUS OperaMuminOipfBackendFactory::createMasterConfiguration(
    std::shared_ptr<UVAMasterConfiguration>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    backend.reset(new OperaOipfMasterConfiguration());
    return UVA_OK;
}

#if !defined USING_SDK422 && !defined USING_SDK423
UVA_STATUS OperaMuminOipfBackendFactory::createDownloadTrigger(
    std::shared_ptr<UVADownloadTrigger>& backend)
{
    CABOT_UNUSED_PARAM(backend);
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminOipfBackendFactory::createDownloadManager(
    std::shared_ptr<UVADownloadManager>& backend)
{
    CABOT_UNUSED_PARAM(backend);
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    return UVA_NOT_SUPPORTED;
}
#endif

UVA_STATUS OperaMuminOipfBackendFactory::createSearchManager(
    std::shared_ptr<UVASearchManager>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    backend.reset(new OperaOipfSearchManager());
    return UVA_OK;
}

UVA_STATUS OperaMuminOipfBackendFactory::createOipfObjectFactory(
    std::shared_ptr<UVAOipfObjectFactory>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    backend.reset(new OperaOipfObjectFactory());
    return UVA_OK;
}

UVA_STATUS OperaMuminOipfBackendFactory::createParentalControlManager(
    std::shared_ptr<UVAParentalControlManager>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    backend.reset(new OperaOipfParentalControlManager());
    return UVA_OK;
}

UVA_STATUS OperaMuminOipfBackendFactory::createRecordingScheduler(
    std::shared_ptr<UVARecordingScheduler>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    frost_bool pvr_available = frost_false;
    if(NEBULA_GetPvrStatus(&pvr_available) == frost_true && pvr_available == frost_true)
    {
        backend.reset(new OperaOipfRecordingScheduler());
        return UVA_OK;
    }
    return UVA_NOT_SUPPORTED;
}

#if !defined USING_SDK422 && !defined USING_SDK423
UVA_STATUS OperaMuminOipfBackendFactory::createGatewayInfo(std::shared_ptr<UVAGatewayInfo>&)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminOipfBackendFactory::createCSManager(std::shared_ptr<UVACSManager>& backend)
{
    CABOT_UNUSED_PARAM(backend);
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    //backend.reset(new UVACSManager());
    //return UVA_OK;
    return UVA_NOT_SUPPORTED;
}
UVA_STATUS OperaMuminOipfBackendFactory::createCorrelationTimestamp(
                                            std::shared_ptr<UVACorrelationTimestamp>& backend)
{
    CABOT_UNUSED_PARAM(backend);
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminOipfBackendFactory::createMediaSynchroniser(std::shared_ptr<UVAMediaSynchroniser>& backend)
{
    CABOT_UNUSED_PARAM(backend);
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    //backend.reset(new UVAMediaSynchroniser());
    //return UVA_OK;
    return UVA_NOT_SUPPORTED;
}
#endif

#ifdef USING_SDK423
UVA_STATUS OperaMuminOipfBackendFactory::createProprietaryFunctionProvider(
    std::shared_ptr<UVAProprietaryFunctionProvider>& backend)
{
    TRACE_INFO(("%s(): entry\n", __FUNCTION__));
    // Required for oipfObjectFactory.createProprietaryFunctionProvider() to succeed.
    backend.reset(new UVAProprietaryFunctionProvider);
    return UVA_OK;
}
#endif
