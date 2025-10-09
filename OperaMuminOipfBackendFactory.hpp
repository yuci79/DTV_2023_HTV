/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief TODO: Brief description of the purpose of this code
 */
#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINOIPFBACKENDFACTORY_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINOIPFBACKENDFACTORY_HPP_INCLUDED

#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include <memory>
#include <string>
#include "uva_ooif_backend_factory.h"
#include "uva_backend.h"
#include "uva_capabilities.h"
#include "uva_master_configuration.h"
#include "uva_download_trigger.h"
#include "uva_download_manager.h"
#include "uva_search_manager.h"
#include "uva_oipf_object_factory.h"
#include "uva_parental_control_manager.h"
#include "uva_recording_scheduler.h"
#include "uva_cs_manager.h"
#include "uva_media_synchroniser.h"
#ifdef USING_SDK423
#include "uva_proprietary_function_provider.h"
#endif

class OperaMuminOipfBackendFactory: public UVAOOIFBackendFactory
{
public:
    OperaMuminOipfBackendFactory();
    virtual ~OperaMuminOipfBackendFactory();
    virtual UVA_STATUS getChannel(const std::string& ccid, std::shared_ptr<UVAChannel>& result);
    virtual UVA_STATUS createCapabilities(
        std::shared_ptr<UVACapabilities>& backend);
    virtual UVA_STATUS createMasterConfiguration(
        std::shared_ptr<UVAMasterConfiguration>& backend);
    
#if !defined USING_SDK422 && !defined USING_SDK423
    virtual UVA_STATUS createDownloadTrigger(
        std::shared_ptr<UVADownloadTrigger>& backend);
    virtual UVA_STATUS createDownloadManager(
        std::shared_ptr<UVADownloadManager>& backend);
#endif
    virtual UVA_STATUS createSearchManager(
        std::shared_ptr<UVASearchManager>& backend);
    virtual UVA_STATUS createOipfObjectFactory(
        std::shared_ptr<UVAOipfObjectFactory>& backend);
    virtual UVA_STATUS createParentalControlManager(
        std::shared_ptr<UVAParentalControlManager>& backend);
    virtual UVA_STATUS createRecordingScheduler(
        std::shared_ptr<UVARecordingScheduler>& backend);
#if !defined USING_SDK422 && !defined USING_SDK423
    virtual UVA_STATUS createGatewayInfo(
            std::shared_ptr<UVAGatewayInfo>& backend);
    virtual UVA_STATUS createCSManager(
            std::shared_ptr<UVACSManager>& backend);
    virtual UVA_STATUS createCorrelationTimestamp(
      std::shared_ptr<UVACorrelationTimestamp>& backend);
    virtual UVA_STATUS createMediaSynchroniser(
            std::shared_ptr<UVAMediaSynchroniser>& backend);
#endif
#ifdef USING_SDK423
    UVA_STATUS createProprietaryFunctionProvider(
        std::shared_ptr<UVAProprietaryFunctionProvider>& backend) override;
#endif
};

#endif
