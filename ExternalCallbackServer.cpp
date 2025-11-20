/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class ExternalCallbackServer.
 */

#include "config/include_opapp_support.h"
#include "ExternalCallbackServer.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaBluetoothSettingsCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaChannelChangeCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaChannelScanControllerCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaComponentCallbackCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaDatetimeSettingsCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaDsmccCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaEitQueryCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaExternalAvSourceNavigatorCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaNetworkSettingsCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaOipfSasProviderCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaParentalControlCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaPresentSiQueryCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaPvrRecordingSchedulerCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaServiceListUpdateControllerCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaStreamEventsCallbacksIpcExternal.hpp" 
#include "nebula_src/adaptation_layer/ipc/external/NebulaSystemInformationProviderCallbacksIpcExternal.hpp" 
#include "nebula_src/adaptation_layer/ipc/external/NebulaTimeshiftCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/AsyncMessageCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/FrostDebugCallbacksIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaMediaPlaybackCallbacksIpcExternal.hpp"

#ifdef INCLUDE_OPAPP_SUPPORT
#include "nebula_src/opapp/ipc/external/NebulaOpAppEventPublisherProviderCallbacksIpcExternal.hpp" 
#endif

ExternalCallbackServer::ExternalCallbackServer(NebulaIpcPorts server_port,
                                               RpcBoundFunction function_to_bind)
    : CommonIpcServer(server_port, __func__)
{
    CommonIpcServer::applyBindings(function_to_bind);
}

void
ExternalCallbackServer::applyBindings(rpc::server& server)
{
    NebulaBluetoothSettingsCallbacksIpcExternal::bindToServer(server);
    NebulaChannelChangeCallbacksIpcExternal::bindToServer(server);
    NebulaChannelScanControllerCallbacksIpcExternal::bindToServer(server);
    NebulaComponentCallbackCallbacksIpcExternal::bindToServer(server);
    NebulaDatetimeSettingsCallbacksIpcExternal::bindToServer(server);
    NebulaDsmccCallbacksIpcExternal::bindToServer(server);
    NebulaEitQueryCallbacksIpcExternal::bindToServer(server);
    NebulaExternalAvSourceNavigatorCallbacksIpcExternal::bindToServer(server);
    NebulaNetworkSettingsCallbacksIpcExternal::bindToServer(server);
    NebulaOipfSasProviderCallbacksIpcExternal::bindToServer(server);
    NebulaParentalControlCallbacksIpcExternal::bindToServer(server);
    NebulaPresentSiQueryCallbacksIpcExternal::bindToServer(server);
    NebulaPvrRecordingSchedulerCallbacksIpcExternal::bindToServer(server);
    NebulaServiceListUpdateControllerCallbacksIpcExternal::bindToServer(server);
    NebulaStreamEventsCallbacksIpcExternal::bindToServer(server);
    NebulaSystemInformationProviderCallbacksIpcExternal::bindToServer(server);
    NebulaTimeshiftCallbacksIpcExternal::bindToServer(server);
    AsyncMessageCallbacksIpcExternal::bindToServer(server);
    FrostDebugCallbacksIpcExternal::bindToServer(server);
    NebulaMediaPlaybackCallbacksIpcExternal::bindToServer(server);
#ifdef INCLUDE_OPAPP_SUPPORT
    NebulaOpAppEventPublisherProviderCallbacksIpcExternal::bindToServer(server);
#endif
}
