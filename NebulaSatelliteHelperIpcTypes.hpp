/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_satellite_helper.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASATELLITEHELPERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULASATELLITEHELPERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_satellite_helper.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetSatelliteInfoWithSatelliteId);
DECLARE_IPC_NAME(NEBULA_GetLNBInfoWithId);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_PolarisationType);

struct MP_NEBULA_TransponderInfo : public NEBULA_TransponderInfo
{
    MEMBER_CHECKED_MSGPACK_DEFINE(
                   NEBULA_TransponderInfo,
                   id,
                   satellite_id,
                   frequency,
                   polarization,
                   symbol_rate);
};

struct MP_NEBULA_SatelliteInfo : public NEBULA_SatelliteInfo
{
    MSGPACK_DEFINE(
        id,
        orbital_position,
        reference_transponder,
        oad_transponder,
        name);

    MEMBER_CHECKED_NEBULA_TYPE(
        NEBULA_SatelliteInfo,
        id,
        orbital_position,
        reference_transponder,
        oad_transponder,
        "");
};

MSGPACK_ADD_ENUM(NEBULA_DISEQCSwitchType);
MSGPACK_ADD_ENUM(NEBULA_DiseqcVersion);
MSGPACK_ADD_ENUM(NEBULA_BandType);
MSGPACK_ADD_ENUM(NEBULA_LnbType);
MSGPACK_ADD_ENUM(NEBULA_SatellitePosition);
MSGPACK_ADD_ENUM(NEBULA_UnicableStandart);

struct MP_NEBULA_LnbInfo : public NEBULA_LnbInfo
{
    MEMBER_CHECKED_MSGPACK_DEFINE(
        NEBULA_LnbInfo,
        id,
        satellite_id,
        diseqc_switch_type,
        diseqc_version,
        band_type,
        lnb_type,
        satellite_position,
        user_band_frequency,
        user_band,
        second_user_band_frequency,
        second_user_band,
        unicable_version);
};

#endif
