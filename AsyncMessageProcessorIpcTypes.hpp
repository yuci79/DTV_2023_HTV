/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        AsyncMessageProcessor.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_ASYNCMESSAGEPROCESSORIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_ASYNCMESSAGEPROCESSORIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(nebula_RegisterAsyncMessageCallback);
DECLARE_IPC_NAME(nebula_DeregisterAsyncMessageCallback);

DECLARE_IPC_NAME(AsyncMessage_onDrmRightsError);
DECLARE_IPC_NAME(AsyncMessage_onOipfParentalRatingEvent);

/*
 * Used in RPC calls from external to aurora to indicate the type
 * of async message processor to register.
 */
enum AsyncMesssageProcessorRegistrationIpcType : unsigned int
{
    NebulaDrmRightsMessageRegistration,
    NebulaParentalRatingMessageRegistration,
    // Always last
    MaxRegistrationIpcTypes
};

MSGPACK_ADD_ENUM(AsyncMesssageProcessorRegistrationIpcType);

/*
 * Structures to pack parameters passed from aurora to external during
 * processor execution.
 */
struct MP_OnDrmRightsError
{
    int error_state;
    std::string content_id;
    std::string drm_system_id;
    std::string rights_issuer_url;

    MSGPACK_DEFINE(error_state, content_id, drm_system_id, rights_issuer_url);

};

struct MP_OnOipfParentalRatingEvent
{
    uint32_t rating_type;
    uint32_t rating_value;
    std::string rating_country;
    std::string drm_system_id;
    bool blocked;

    MSGPACK_DEFINE(rating_type, rating_value, rating_country, drm_system_id, blocked);
};

#endif
