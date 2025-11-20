/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_tuner_configuration.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATUNERCONFIGURATIONIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULATUNERCONFIGURATIONIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetNumberOfTuners);
DECLARE_IPC_NAME(NEBULA_GetCurrentChannelDetails);

/*
 * Define packing of data types used in IPC calls.
 */

struct MP_NEBULA_RFChannelDetails : public NEBULA_RFChannelDetails
{
    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_RFChannelDetails, lnb_configuration_id);
};

#endif
