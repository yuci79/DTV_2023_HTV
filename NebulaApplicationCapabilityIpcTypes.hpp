/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_application_capability.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAPPLICATIONCAPABILITYIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAAPPLICATIONCAPABILITYIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_application_capability.h"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetSharedCapability);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_EPGAvailability);

struct MP_NEBULA_ApplicationSharedCapability : public NEBULA_ApplicationSharedCapability
{
    MEMBER_CHECKED_MSGPACK_DEFINE(
        NEBULA_ApplicationSharedCapability,
        update_astra_service_list_availability,
        cable_support_enabled,
        satellite_support_enabled,
        pvr_support_enabled,
        pvr_ready_support_enabled,
        dvd_support_enabled,
        media_browser_support_enabled,
        epg_availability,
        satcodx_enabled,
        touchpad_osd_enabled,
        standby_service_scan_item_enabled,
        illuminated_logo_item_enabled,
        standby_led_item_enabled,
        power_up_always_on_mode_selection_enabled,
        hybrid_ui_availability,
        manual_oad_availability);
};

#endif
