/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_parental_control.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPARENTALCONTROLIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPARENTALCONTROLIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_parental_control.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetPinCodeLength);
DECLARE_IPC_NAME(NEBULA_SetParentalControlStatus);
DECLARE_IPC_NAME(NEBULA_GetParentalControlStatus);
DECLARE_IPC_NAME(NEBULA_ValidateParentalRatingPin);
DECLARE_IPC_NAME(NEBULA_SetParentalControlPin);
DECLARE_IPC_NAME(NEBULA_AuthoriseChannelSelection);
DECLARE_IPC_NAME(NEBULA_GetNumParentalRatingSchemes);
DECLARE_IPC_NAME(NEBULA_GetParentalRatingSchemeNameByIndex);
DECLARE_IPC_NAME(NEBULA_GetParentalRatingSchemeInfoByName);
DECLARE_IPC_NAME(NEBULA_GetParentalRatingInfo);
DECLARE_IPC_NAME(NEBULA_enableOperatorLock);
DECLARE_IPC_NAME(NEBULA_disableOperatorLock);
DECLARE_IPC_NAME(NEBULA_IsPinRequested);
DECLARE_IPC_NAME(NEBULA_IsUserLockPinRequested);
DECLARE_IPC_NAME(NEBULA_RegisterToParentalRatingChangeEvent);
DECLARE_IPC_NAME(NEBULA_DeregisterFromParentalRatingChangeEvent);
DECLARE_IPC_NAME(NEBULA_OnParentalRatingChange);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NebulaBlockActivation)

struct MP_NEBULA_ParentalRating : public NEBULA_ParentalRating
{
    MP_NEBULA_ParentalRating() = default;

    MP_NEBULA_ParentalRating(const NEBULA_ParentalRating& other)
        : NEBULA_ParentalRating(other)
    {
    }

    MSGPACK_DEFINE(name,
                   scheme,
                   rating_type,
                   value,
                   labels,
                   region);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_ParentalRating,
                   "",
                   "",
                   rating_type,
                   value,
                   labels,
                   "");
};

struct MP_NEBULA_ParentalRatingScheme : public NEBULA_ParentalRatingScheme
{
    MSGPACK_DEFINE(name,
                   rating_type,
                   length,
                   threshold.name,
                   threshold.scheme,
                   threshold.rating_type,
                   threshold.value,
                   threshold.labels,
                   threshold.region);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_ParentalRatingScheme,
                   "",
                   rating_type,
                   length,
                   "",
                   "",
                   threshold.rating_type,
                   threshold.value,
                   threshold.labels,
                   "");
};

#endif
