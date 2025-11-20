/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the VideoBroadcastObjectProxyIpcAurora class for IPC aurora.
 */

#include "VideoBroadcastObjectProxyIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula_src/adaptation_layer/ipc/AsyncMessageProcessorIpcTypes.hpp"

void
VideoBroadcastObjectProxyIpcAurora::onDrmRightsError(int error_state,
                                        const std::string& content_id,
                                        const std::string& drm_system_id,
                                        const std::string& rights_issuer_url)
{
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);

    MP_OnDrmRightsError error {error_state, content_id, drm_system_id, rights_issuer_url};

    client.call(IPC_NAME(AsyncMessage_onDrmRightsError), error);
}
    
void
VideoBroadcastObjectProxyIpcAurora::onOipfParentalRatingEvent(uint32_t rating_type,
                                        uint32_t rating_value,
                                        const std::string& rating_country,
                                        const std::string& drm_system_id, 
                                        bool blocked)
{
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
   
    MP_OnOipfParentalRatingEvent event {rating_type, rating_value, rating_country, 
                                        drm_system_id, blocked};

    client.call(IPC_NAME(AsyncMessage_onOipfParentalRatingEvent), event);   
}
