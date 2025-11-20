/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_channel_info.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELINFOIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELINFOIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_channel_info.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetChannelIdByCCID);
DECLARE_IPC_NAME(NEBULA_GetChannelIDByIndex);
DECLARE_IPC_NAME(NEBULA_GetChannelIDByTriplet);
DECLARE_IPC_NAME(NEBULA_GetChannelInfoByID);
DECLARE_IPC_NAME(NEBULA_GetChannelInfoByIndex);
DECLARE_IPC_NAME(NEBULA_GetChannelListSize);

DECLARE_IPC_NAME(NEBULA_CreateChannelByDescriptor);
DECLARE_IPC_NAME(NEBULA_CreateChannelByTriplet);
DECLARE_IPC_NAME(NEBULA_CreateIPChannel);
DECLARE_IPC_NAME(NEBULA_IsValidChannel);
DECLARE_IPC_NAME(NEBULA_GetCurrentChannel);
DECLARE_IPC_NAME(NEBULA_GetNextChannel);
DECLARE_IPC_NAME(NEBULA_GetPreviousChannel);
DECLARE_IPC_NAME(NEBULA_GetSidByID);
DECLARE_IPC_NAME(NEBULA_GetChannelCCID);

DECLARE_IPC_NAME(NEBULA_GetCurrentFavouriteListNumber);
DECLARE_IPC_NAME(NEBULA_GetFavouriteChannelIDs);
DECLARE_IPC_NAME(NEBULA_GetFavouriteListID);
DECLARE_IPC_NAME(NEBULA_GetFavouriteListName);
DECLARE_IPC_NAME(NEBULA_ClearFavouriteList);
DECLARE_IPC_NAME(NEBULA_SetFavouriteChannel);
DECLARE_IPC_NAME(NEBULA_ChannelListSave);
DECLARE_IPC_NAME(NEBULA_GetIpBroadcastId);

DECLARE_IPC_NAME(NEBULA_IsDvbIService);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_ChannelType);
MSGPACK_ADD_ENUM(NEBULA_NetworkType);
MSGPACK_ADD_ENUM(NEBULA_RunningStatus);

struct MP_NEBULA_Channel : public NEBULA_Channel
{
    MSGPACK_DEFINE(channel_type, 
                   ccid, 
                   dsd, 
                   dsd_size,
                   is_onid_valid,
                   onid, 
                   tsid, 
                   sid, 
                   networkType, 
                   name, 
                   name_edited,
                   is_encrypted, 
                   is_locked, 
                   is_hidden, 
                   is_hd,
                   major_channel, 
                   minor_channel, 
                   broadcast_dvb_channel,
                   freesat_service_id, 
                   nid, 
                   opal_channel_type,
                   short_name, 
                   has_CUTV, 
                   running_status,
                   numeric_selection, 
                   recordable,
                   is_dvbi);
    
    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_Channel,
                   channel_type, 
                   "", 
                   "", 
                   dsd_size,
                   is_onid_valid,
                   onid, 
                   tsid, 
                   sid, 
                   networkType, 
                   "", 
                   "",
                   is_encrypted, 
                   is_locked, 
                   is_hidden, 
                   is_hd,
                   major_channel, 
                   minor_channel, 
                   broadcast_dvb_channel,
                   freesat_service_id, 
                   nid, 
                   opal_channel_type,
                   "", 
                   has_CUTV, 
                   running_status,
                   numeric_selection, 
                   recordable,
                   is_dvbi);
};

#endif
