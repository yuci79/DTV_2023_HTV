/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for the NebulaStreamEventsCallbacksIpcAurora class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULASTREAMEVENTSCALLBACKSIPCAURORA_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_NEBULASTREAMEVENTSCALLBACKSIPCAURORA_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.hpp"

namespace rpc
{
    class server;
}

class NEBULA_DsmccStreamEventSubscription;

/**
 * @brief Class to bind all required nebula_stream_events callback functions.
 */
class NebulaStreamEventsCallbacksIpcAurora
{
public:
    /**
     * @brief An invitation to bind functors to an rpc::server.
     * @param server An rpc::server ready to accept bindings.
     */ 
    static void bindToServer(rpc::server& server);

    /**
     * @brief Find a NEBULA_DsmccStreamEventSubscription from a token.
     * @param stream_event_subscription_token is the token.
     * @param stream_event_subscription is the output subscription.
     * @returns true if a subscription is found
     */ 
    static bool find(
            RegisteredStreamHandlesConfig::TokenType stream_event_subscription_token,
            NEBULA_DsmccStreamEventSubscription*& stream_event_subscription);

private:
    static RegisteredStreamHandles<NEBULA_DsmccStreamEventSubscription*>
                                        m_local_stream_event_subscription_handles;
};

#endif

