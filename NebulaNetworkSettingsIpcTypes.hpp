/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_network_settings.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULANETWORKSETTINGSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULANETWORKSETTINGSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_network_settings.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_GetSelectedNetworkDevice);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsGetDeviceMacID);

DECLARE_IPC_NAME(NEBULA_RegisterNetworkSettingsEventCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterNetworkSettingsEventCallback);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsEventCallback);

DECLARE_IPC_NAME(NEBULA_NetworkSettingsCheckInternetConnection);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsEnableNetworkInterface);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsGetDeviceIPParams);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsGetNumberOfDevices);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsSetDynamicIPAddress);
DECLARE_IPC_NAME(NEBULA_NetworkSettingsSetStaticIPAddress);
DECLARE_IPC_NAME(NEBULA_AnyWiFiProfileSaved);
DECLARE_IPC_NAME(NEBULA_IsDongleDetected);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_IPDevice);
MSGPACK_ADD_ENUM(NEBULA_NetworkSettingsEventType);
MSGPACK_ADD_ENUM(NEBULA_ConnectionType);
MSGPACK_ADD_ENUM(NEBULA_IPState);
MSGPACK_ADD_ENUM(NEBULA_DNSState);
MSGPACK_ADD_ENUM(NEBULA_LinkStatus);

struct MP_NEBULA_NetworkSettingsEventData : public NEBULA_NetworkSettingsEventData
{
    /**
     * @brief Default constructor.
     */
    MP_NEBULA_NetworkSettingsEventData() = default;

    /**
     * @brief Constructor to copy from the base class. Required for use as a parameter in 
     *        rpc::client::call().
     */
    MP_NEBULA_NetworkSettingsEventData(const NEBULA_NetworkSettingsEventData& base)
        : NEBULA_NetworkSettingsEventData(base)
    {   
    }

    MSGPACK_DEFINE(event_type,
                   connection_type,
                   interface_up,
                   internet_connected,
                   internet_speed,
                   ip_set,
                   ip_params.ip_state,
                   ip_params.dns_state,
                   ip_params.ip_device,
                   ip_params.link_status,
                   ip_params.ip_addr,
                   ip_params.subnet_mask,
                   ip_params.default_gateway,
                   ip_params.broadcast_addr,
                   ip_params.dns_server,
                   ip_params.interface_name);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_NetworkSettingsEventData,
                   event_type,
                   connection_type,
                   interface_up,
                   internet_connected,
                   internet_speed,
                   ip_set,
                   ip_params.ip_state,
                   ip_params.dns_state,
                   ip_params.ip_device,
                   ip_params.link_status,
                   ip_params.ip_addr,
                   ip_params.subnet_mask,
                   ip_params.default_gateway,
                   ip_params.broadcast_addr,
                   {0},
                   "");
};

struct MP_NEBULA_IPParams : public NEBULA_IPParams
{
    /**
     * @brief Default constructor.
     */
    MP_NEBULA_IPParams() = default;

    /**
     * @brief Constructor to copy from the base class. Required for use as a parameter in 
     *        rpc::client::call().
     */
    MP_NEBULA_IPParams(const NEBULA_IPParams& base)
        : NEBULA_IPParams(base)
    {   
    }

    MSGPACK_DEFINE(ip_state,
                   dns_state,
                   ip_device,
                   link_status,
                   ip_addr,
                   subnet_mask,
                   default_gateway,
                   broadcast_addr,
                   dns_server,
                   interface_name);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_IPParams,
                   ip_state,
                   dns_state,
                   ip_device,
                   link_status,
                   ip_addr,
                   subnet_mask,
                   default_gateway,
                   broadcast_addr,
                   {0},
                   "");
};

#endif
