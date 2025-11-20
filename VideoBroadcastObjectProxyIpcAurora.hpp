/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the VideoBroadcastObjectProxyIpcAurora class for IPC aurora.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_VIDEOBROADCASTOBJECTPROXYIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_VIDEOBROADCASTOBJECTPROXYIPCAURORA_HPP_INCLUDED

#include "nebula/core/browser_client/AnyVideoBroadcastObjectProxyInterface.hpp"

#include <string>

/**
 * @brief Helper class used by those AsyncMessageProcessors in IPC aurora which require a
 *        AnyVideoBroadcastObjectProxyInterface.
 */
class VideoBroadcastObjectProxyIpcAurora: public AnyVideoBroadcastObjectProxyInterface
{
public:
    virtual ~VideoBroadcastObjectProxyIpcAurora() = default;

public:
    // Implementing AnyVideoBroadcastObjectProxyInterface
    void onDrmRightsError(int error_state,
                          const std::string& content_id,
                          const std::string& drm_system_id,
                          const std::string& rights_issuer_url) override;
                              
    void onOipfParentalRatingEvent(uint32_t rating_type,
                          uint32_t rating_value,
                          const std::string& rating_country,
                          const std::string& drm_system_id, 
                          bool blocked) override;
};

#endif
