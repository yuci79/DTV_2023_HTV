/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_satellite_helper.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaSatelliteHelperIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_bool
NEBULA_GetSatelliteInfoWithSatelliteId(frost_int satellite_id,
                                       NEBULA_SatelliteInfo* satellite_info)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_SatelliteInfo>(
                                *satellite_info,
                                IPC_NAME(NEBULA_GetSatelliteInfoWithSatelliteId),
                                satellite_id);
}

frost_bool
NEBULA_GetLNBInfoWithId(frost_int lnb_id, NEBULA_LnbInfo* lnb_info)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_LnbInfo>(
                                *lnb_info,
                                IPC_NAME(NEBULA_GetLNBInfoWithId),
                                lnb_id);
}