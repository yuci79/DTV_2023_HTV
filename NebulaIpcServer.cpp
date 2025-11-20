/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Definitions for the Nebula IPC server.
 */

#include "config/include_opapp_support.h"
#include "config/include_avs.h"
#include "NebulaIpcServer.hpp"

#include "AsyncMessageCallbacksIpcAurora.hpp"
#include "FrostDebugCallbacksIpcAurora.hpp"
#include "NebulaApplicationCapabilityIpcAurora.hpp"
#include "NebulaAvControlIpcAurora.hpp"
#include "NebulaAVSClientIpcAurora.hpp"
#include "NebulaBluetoothSettingsIpcAurora.hpp"
#include "NebulaBluetoothSettingsCallbacksIpcAurora.hpp"
#include "NebulaBroadcastComponentsIpcAurora.hpp"
#include "NebulaCaControllerIpcAurora.hpp"
#include "NebulaChannelChangeIpcAurora.hpp"
#include "NebulaChannelChangeCallbacksIpcAurora.hpp"
#include "NebulaChannelScanControllerIpcAurora.hpp"
#include "NebulaChannelScanControllerCallbacksIpcAurora.hpp"
#include "NebulaChannelInfoIpcAurora.hpp"
#include "NebulaComponentCallbackIpcAurora.hpp"
#include "NebulaComponentCallbackCallbacksIpcAurora.hpp"
#include "NebulaConfigurationIpcAurora.hpp"
#include "NebulaCountrySettingsIpcAurora.hpp"
#include "NebulaDatetimeSettingsCallbacksIpcAurora.hpp"
#include "NebulaDatetimeSettingsIpcAurora.hpp"
#include "NebulaDsmccCallbacksIpcAurora.hpp"
#include "NebulaDsmccIpcAurora.hpp"
#include "NebulaEitQueryIpcAurora.hpp"
#include "NebulaEitQueryCallbacksIpcAurora.hpp"
#include "NebulaExternalAvSourceNavigatorIpcAurora.hpp"
#include "NebulaExternalAvSourceNavigatorCallbacksIpcAurora.hpp"
#include "NebulaLanguageSettingsIpcAurora.hpp"
#include "NebulaMhegIpcAurora.hpp"
#include "NebulaNetworkSettingsIpcAurora.hpp"
#include "NebulaNetworkSettingsCallbacksIpcAurora.hpp"
#include "NebulaOipfSasProviderIpcAurora.hpp"
#include "NebulaOipfSasProviderCallbacksIpcAurora.hpp"
#include "NebulaParentalControlIpcAurora.hpp"
#include "NebulaParentalControlCallbacksIpcAurora.hpp"
#include "NebulaPresentSiQueryIpcAurora.hpp"
#include "NebulaPresentSiQueryCallbacksIpcAurora.hpp"
#include "NebulaPvrConfigurationIpcAurora.hpp"
#include "NebulaPvrDiskControllerIpcAurora.hpp"
#include "NebulaPvrRecordingSchedulerIpcAurora.hpp"
#include "NebulaPvrRecordingSchedulerCallbacksIpcAurora.hpp"
#include "NebulaRemovableDeviceControllerIpcAurora.hpp"
#include "NebulaSatelliteHelperIpcAurora.hpp"
#include "NebulaServiceListUpdateControllerIpcAurora.hpp"
#include "NebulaServiceListUpdateControllerCallbacksIpcAurora.hpp"
#include "NebulaSignalInfoIpcAurora.hpp"
#include "NebulaSoundSettingsIpcAurora.hpp"
#include "NebulaStandbyControllerIpcAurora.hpp"
#include "NebulaStreamEventsIpcAurora.hpp"
#include "NebulaStreamEventsCallbacksIpcAurora.hpp"
#include "NebulaSubtitleSettingsIpcAurora.hpp"
#include "NebulaSystemInformationProviderIpcAurora.hpp"
#include "NebulaSystemInformationProviderCallbacksIpcAurora.hpp"
#include "NebulaTemiTimelineIpcAurora.hpp"
#include "NebulaTimeshiftIpcAurora.hpp"
#include "NebulaTimeshiftCallbacksIpcAurora.hpp"
#include "NebulaTunerConfigurationIpcAurora.hpp"
#include "NebulaWebBrowserIpcAurora.hpp"
#ifdef INCLUDE_OPAPP_SUPPORT
#include "nebula_src/opapp/ipc/aurora/NebulaOpAppChannelListModifierIpcAurora.hpp"
#include "nebula_src/opapp/ipc/aurora/NebulaOpAppEventPublisherProviderCallbacksIpcAurora.hpp"
#include "nebula_src/opapp/ipc/aurora/NebulaOpAppEventPublisherProviderIpcAurora.hpp"
#endif
#include "NebulaBrowserBackendIpcAurora.hpp"

#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "3rd_party_wrapper/rpclib/include/FrostIpcTypes.hpp"

#include <rpc/server.h>
#include <stdio.h>

constexpr unsigned int server_thread_count = 2;

NebulaIpcServer::NebulaIpcServer()
    : CommonIpcServer(NebulaIpcPorts::nebula_server_port, __func__, server_thread_count)
{
    CommonIpcServer::applyBindings();
}

void 
NebulaIpcServer::resetCallbacks()
{
    // All callback classes to purge tokens etc
    m_ipc_frost_debug.reset();
    AsyncMessageCallbacksIpcAurora::reset();
    NebulaBluetoothSettingsCallbacksIpcAurora::reset();
    NebulaChannelChangeCallbacksIpcAurora::reset();
    NebulaChannelScanControllerCallbacksIpcAurora::reset();
    NebulaDatetimeSettingsCallbacksIpcAurora::reset();
    NebulaNetworkSettingsCallbacksIpcAurora::reset();
    NebulaOipfSasProviderCallbacksIpcAurora::reset();
    NebulaPvrRecordingSchedulerCallbacksIpcAurora::reset();
    NebulaServiceListUpdateControllerCallbacksIpcAurora::reset();
    NebulaSystemInformationProviderCallbacksIpcAurora::reset();
    NebulaTimeshiftCallbacksIpcAurora::reset();
#ifdef INCLUDE_OPAPP_SUPPORT
    NebulaOpAppEventPublisherProviderCallbacksIpcAurora::reset();
#endif
}

void
NebulaIpcServer::applyBindings(rpc::server& server)
{
    m_ipc_trace.bindToServer(server);
    m_ipc_frost_debug.bindToServer(server);
    AsyncMessageCallbacksIpcAurora::bindToServer(server);
    NebulaApplicationCapabilityIpcAurora::bindToServer(server);
    NebulaAvControlIpcAurora::bindToServer(server);
    NebulaBroadcastComponentsIpcAurora::bindToServer(server);
    NebulaCaControllerIpcAurora::bindToServer(server);
    NebulaChannelChangeIpcAurora::bindToServer(server);
    NebulaBluetoothSettingsIpcAurora::bindToServer(server);
    NebulaBluetoothSettingsCallbacksIpcAurora::bindToServer(server);
    NebulaChannelChangeCallbacksIpcAurora::bindToServer(server);
    NebulaChannelScanControllerIpcAurora::bindToServer(server);
    NebulaChannelScanControllerCallbacksIpcAurora::bindToServer(server);
    NebulaChannelInfoIpcAurora::bindToServer(server);
    NebulaComponentCallbackIpcAurora::bindToServer(server);
    NebulaComponentCallbackCallbacksIpcAurora::bindToServer(server);
    NebulaConfigurationIpcAurora::bindToServer(server);
    NebulaCountrySettingsIpcAurora::bindToServer(server);
    NebulaDatetimeSettingsIpcAurora::bindToServer(server);
    NebulaDatetimeSettingsCallbacksIpcAurora::bindToServer(server);
    NebulaDsmccCallbacksIpcAurora::bindToServer(server);
    NebulaDsmccIpcAurora::bindToServer(server);
    NebulaEitQueryIpcAurora::bindToServer(server);
    NebulaEitQueryCallbacksIpcAurora::bindToServer(server);
    NebulaExternalAvSourceNavigatorIpcAurora::bindToServer(server);
    NebulaExternalAvSourceNavigatorCallbacksIpcAurora::bindToServer(server);
    NebulaLanguageSettingsIpcAurora::bindToServer(server);
    NebulaMhegIpcAurora::bindToServer(server);
    NebulaNetworkSettingsIpcAurora::bindToServer(server);
    NebulaNetworkSettingsCallbacksIpcAurora::bindToServer(server);
    NebulaOipfSasProviderIpcAurora::bindToServer(server);
    NebulaOipfSasProviderCallbacksIpcAurora::bindToServer(server);
    NebulaParentalControlIpcAurora::bindToServer(server);
    NebulaParentalControlCallbacksIpcAurora::bindToServer(server);
    NebulaPresentSiQueryIpcAurora::bindToServer(server);
    NebulaPresentSiQueryCallbacksIpcAurora::bindToServer(server);
    NebulaPvrConfigurationIpcAurora::bindToServer(server);
    NebulaPvrDiskControllerIpcAurora::bindToServer(server);
    NebulaPvrRecordingSchedulerIpcAurora::bindToServer(server);
    NebulaPvrRecordingSchedulerCallbacksIpcAurora::bindToServer(server);
    NebulaRemovableDeviceControllerIpcAurora::bindToServer(server);
    NebulaSatelliteHelperIpcAurora::bindToServer(server);
    NebulaServiceListUpdateControllerIpcAurora::bindToServer(server);
    NebulaServiceListUpdateControllerCallbacksIpcAurora::bindToServer(server);
    NebulaSignalInfoIpcAurora::bindToServer(server);
    NebulaSoundSettingsIpcAurora::bindToServer(server);
    NebulaStandbyControllerIpcAurora::bindToServer(server);
    NebulaStreamEventsIpcAurora::bindToServer(server);
    NebulaStreamEventsCallbacksIpcAurora::bindToServer(server);
    NebulaSubtitleSettingsIpcAurora::bindToServer(server);
    NebulaSystemInformationProviderIpcAurora::bindToServer(server);
    NebulaSystemInformationProviderCallbacksIpcAurora::bindToServer(server);
    NebulaTemiTimelineIpcAurora::bindToServer(server);
    NebulaTimeshiftIpcAurora::bindToServer(server);
    NebulaTimeshiftCallbacksIpcAurora::bindToServer(server);
    NebulaTunerConfigurationIpcAurora::bindToServer(server);
    NebulaWebBrowserIpcAurora::bindToServer(server);
#ifdef INCLUDE_OPAPP_SUPPORT
    NebulaOpAppChannelListModifierIpcAurora::bindToServer(server);
    NebulaOpAppEventPublisherProviderCallbacksIpcAurora::bindToServer(server);
    NebulaOpAppEventPublisherProviderIpcAurora::bindToServer(server);
#endif
#ifdef INCLUDE_AVS
    NebulaAVSClientIpcAurora::bindToServer(server);
#endif
    // In the bindToServer function:
    NebulaBrowserBackendIpcAurora::bindToServer(server);
    printf("  Bound: NebulaBrowserBackendIpcAurora\n");

}
