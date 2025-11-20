/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaChannelInfoIpcAurora class.
 */

#include "NebulaChannelInfoIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelInfoIpcTypes.hpp"

#include <string>
#include <vector>

#include <rpc/server.h>

void
NebulaChannelInfoIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for nebula_channel_info 'C' functions.
    server.bind(IPC_NAME(NEBULA_GetChannelIdByCCID), [](std::string ccid) {
            return NEBULA_GetChannelIdByCCID(ccid.c_str()); });
    server.bind(IPC_NAME(NEBULA_GetChannelIDByIndex), &NEBULA_GetChannelIDByIndex);
    server.bind(IPC_NAME(NEBULA_GetChannelIDByTriplet), &NEBULA_GetChannelIDByTriplet);
    server.bind(IPC_NAME(NEBULA_GetChannelInfoByID), [](frost_int channel_id) {
        NebulaIpcType<frost_bool, MP_NEBULA_Channel> channel;
        channel.return_value = NEBULA_GetChannelInfoByID(channel_id, &channel.output_params);
        return channel; });
    server.bind(IPC_NAME(NEBULA_GetChannelInfoByIndex), [](frost_int channel_list_index) {
        NebulaIpcType<frost_bool, MP_NEBULA_Channel> channel;
        channel.return_value = NEBULA_GetChannelInfoByIndex(channel_list_index, &channel.output_params);
        return channel; });
    server.bind(IPC_NAME(NEBULA_GetChannelListSize), &NEBULA_GetChannelListSize);
    server.bind(IPC_NAME(NEBULA_CreateChannelByDescriptor), [](std::vector<unsigned char> dsd,
                                                               frost_int sid) {
        return NEBULA_CreateChannelByDescriptor(dsd.data(), dsd.size(), sid);
    });
    server.bind(IPC_NAME(NEBULA_CreateChannelByTriplet), &NEBULA_CreateChannelByTriplet);
    server.bind(IPC_NAME(NEBULA_CreateIPChannel), [](std::string broadcast_id) {
        NebulaIpcType<frost_bool, frost_int> channel;
        channel.return_value = NEBULA_CreateIPChannel(broadcast_id.c_str(), &channel.output_params);
        return channel;
    });
    server.bind(IPC_NAME(NEBULA_IsValidChannel), &NEBULA_IsValidChannel);
    server.bind(IPC_NAME(NEBULA_GetCurrentChannel), &NEBULA_GetCurrentChannel);
    server.bind(IPC_NAME(NEBULA_GetNextChannel), &NEBULA_GetNextChannel);
    server.bind(IPC_NAME(NEBULA_GetPreviousChannel), &NEBULA_GetPreviousChannel);
    server.bind(IPC_NAME(NEBULA_GetSidByID), &NEBULA_GetSidByID);
    server.bind(IPC_NAME(NEBULA_GetChannelCCID), [](frost_int channel_id, frost_uint buffer_length)
    {
        std::vector<char> ccid_str(buffer_length);

        NebulaIpcType<frost_bool, std::string> ccid;
        ccid.return_value = NEBULA_GetChannelCCID(channel_id, ccid_str.data(), buffer_length);
        ccid.output_params = ccid_str.data();

        return ccid;
    });

    server.bind(IPC_NAME(NEBULA_GetCurrentFavouriteListNumber), &NEBULA_GetCurrentFavouriteListNumber);

    server.bind(IPC_NAME(NEBULA_GetFavouriteChannelIDs), [](frost_int favourite_list_number) {
        frost_int* channel_ids = nullptr;
        frost_int count;
        NEBULA_GetFavouriteChannelIDs(favourite_list_number, &count, &channel_ids);
        std::vector<frost_int> ids(channel_ids, channel_ids + count);
        free(channel_ids);
        return ids;
    });

    server.bind(IPC_NAME(NEBULA_GetFavouriteListID), [](frost_int favourite_list_number, frost_int buffer_length) {
        char* buffer = new char[buffer_length];
        NEBULA_GetFavouriteListID(favourite_list_number, buffer, buffer_length);
        const std::string output = buffer;
        delete[] buffer;
        return output;
    });

    server.bind(IPC_NAME(NEBULA_GetFavouriteListName), [](frost_int favourite_list_number, frost_int buffer_length) {
        char* buffer = new char[buffer_length];
        NEBULA_GetFavouriteListName(favourite_list_number, buffer, buffer_length);
        const std::string output = buffer;
        delete[] buffer;
        return output;
    });

    server.bind(IPC_NAME(NEBULA_ClearFavouriteList), &NEBULA_ClearFavouriteList);

    server.bind(IPC_NAME(NEBULA_SetFavouriteChannel), [](frost_int favourite_list_number, const std::string& ccid, frost_bool is_favourite) {
        NEBULA_SetFavouriteChannel(favourite_list_number, ccid.c_str(), is_favourite);
    });

    server.bind(IPC_NAME(NEBULA_ChannelListSave), &NEBULA_ChannelListSave);

    server.bind(IPC_NAME(NEBULA_GetIpBroadcastId), [](const std::string ccid) {
        NebulaIpcType<frost_bool, std::string> result;

        char ip_broadcast_id[NEBULA_IP_BROADCAST_ID_LENGTH] = "";
        result.return_value = NEBULA_GetIpBroadcastId(ccid.c_str(), ip_broadcast_id);
        result.output_params = ip_broadcast_id;

        return result;
    });

    server.bind(IPC_NAME(NEBULA_IsDvbIService), [](const std::string ccid) {
        return NEBULA_IsDvbIService(ccid.c_str());
    });
}
