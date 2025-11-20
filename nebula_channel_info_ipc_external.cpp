/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_channel_info.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaChannelInfoIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

extern "C" frost_int
NEBULA_GetChannelIdByCCID(const char* ccid)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetChannelIdByCCID), std::string(ccid));
}

extern "C" frost_int
NEBULA_GetChannelIDByIndex(frost_int index_in_channel_list)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetChannelIDByIndex), index_in_channel_list);
}

extern "C" frost_int
NEBULA_GetChannelIDByTriplet(frost_int onid,
                             frost_int tsid,
                             frost_int sid)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetChannelIDByTriplet), onid, tsid, sid);
}

extern "C" frost_bool
NEBULA_GetChannelInfoByID(frost_int channel_id,
                          NEBULA_Channel* channel_info)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_Channel>(*channel_info,
                                            IPC_NAME(NEBULA_GetChannelInfoByID),
                                            channel_id);
}

extern "C" frost_bool
NEBULA_GetChannelInfoByIndex(frost_int channel_list_index,
                          NEBULA_Channel* channel_info)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_Channel>(*channel_info,
                                            IPC_NAME(NEBULA_GetChannelInfoByIndex),
                                            channel_list_index);
}

extern "C" frost_int
NEBULA_GetChannelListSize(void)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetChannelListSize));
}

extern "C" frost_int
NEBULA_CreateChannelByDescriptor(const unsigned char * dsd,
                                 size_t dsd_size,
                                 frost_int sid)
{
    const std::vector<unsigned char> dsd_vec(dsd, dsd + dsd_size);
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_CreateChannelByDescriptor), dsd_vec, sid);
}

extern "C" frost_int
NEBULA_CreateChannelByTriplet(NEBULA_NetworkType channel_type,
                              frost_int onid, frost_int tsid, frost_int sid)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_CreateChannelByTriplet),
                                    channel_type, onid, tsid, sid);
}

extern "C" frost_bool
NEBULA_CreateIPChannel(const char* broadcast_id, frost_int *channel_id)
{
    return nebulaRpcCall<frost_bool, frost_int>(*channel_id,
                                                IPC_NAME(NEBULA_CreateIPChannel),
                                                std::string(broadcast_id));
}

extern "C" frost_bool
NEBULA_IsValidChannel(frost_int channel_id)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsValidChannel), channel_id);
}

extern "C" frost_int
NEBULA_GetCurrentChannel()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetCurrentChannel));
}

extern "C" frost_int
NEBULA_GetNextChannel()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetNextChannel));
}

extern "C" frost_int
NEBULA_GetPreviousChannel()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetPreviousChannel));
}

extern "C" frost_int
NEBULA_GetSidByID(frost_int channel_id)
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetSidByID), channel_id);
}

extern "C" frost_bool
NEBULA_GetChannelCCID(frost_int channel_id, char* ccid, frost_uint buffer_length)
{
    std::string ccid_str;
    const auto result = nebulaRpcCall<frost_bool, std::string>(ccid_str, IPC_NAME(NEBULA_GetChannelCCID),
                                                               channel_id,
                                                               buffer_length);
    std::snprintf(ccid, buffer_length, "%s", ccid_str.c_str());

    return result;

}

extern "C" frost_int
NEBULA_GetCurrentFavouriteListNumber()
{
    return nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_GetCurrentFavouriteListNumber));
}

extern "C" void
NEBULA_GetFavouriteChannelIDs(frost_int favourite_list_number, frost_int* count, frost_int** channel_ids)
{
    const auto ids = nebulaRpcCall<std::vector<frost_int>>(
                                IPC_NAME(NEBULA_GetFavouriteChannelIDs), favourite_list_number);
    *count = static_cast<frost_int>(ids.size());
    *channel_ids = static_cast<frost_int*>(malloc(sizeof(frost_int) * (*count)));
    std::copy(ids.begin(), ids.begin() + *count, *channel_ids);
}

extern "C" void
NEBULA_GetFavouriteListID(frost_int favourite_list_number, char* fav_id, frost_int buffer_length)
{
    const auto result = nebulaRpcCall<std::string>(
                                IPC_NAME(NEBULA_GetFavouriteListID),
                                favourite_list_number, buffer_length);
    strncpy(fav_id, result.c_str(), buffer_length);
    fav_id[buffer_length - 1] = '\0';
}

extern "C" void
NEBULA_GetFavouriteListName(frost_int favourite_list_number, char* name, frost_int buffer_length)
{
    const auto result = nebulaRpcCall<std::string>(
                                IPC_NAME(NEBULA_GetFavouriteListName),
                                favourite_list_number, buffer_length);
    strncpy(name, result.c_str(), buffer_length);
    name[buffer_length - 1] = '\0';
}

extern "C" void
NEBULA_ClearFavouriteList(frost_int favourite_list_number)
{
    nebulaRpcCall(IPC_NAME(NEBULA_ClearFavouriteList), favourite_list_number);
}

extern "C" void
NEBULA_SetFavouriteChannel(frost_int favourite_list_number, const char* ccid, frost_bool is_favourite)
{
    nebulaRpcCall(IPC_NAME(NEBULA_SetFavouriteChannel),
                  favourite_list_number, std::string(ccid), is_favourite);
}

extern "C" frost_bool
NEBULA_ChannelListSave()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_ChannelListSave));
}

extern "C" frost_bool
NEBULA_GetIpBroadcastId(const char* ccid, char* ip_broadcast_id)
{
    std::string ip_broadcast_id_str;
    const auto result = nebulaRpcCall<frost_bool, std::string>(ip_broadcast_id_str,
                                                               IPC_NAME(NEBULA_GetIpBroadcastId),
                                                               std::string(ccid));
    strncpy(ip_broadcast_id, ip_broadcast_id_str.c_str(), NEBULA_IP_BROADCAST_ID_LENGTH);
    ip_broadcast_id[NEBULA_IP_BROADCAST_ID_LENGTH - 1] = '\0';

    return result;
}

extern "C" frost_bool
NEBULA_IsDvbIService(const char* ccid)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_IsDvbIService), std::string(ccid));
}
