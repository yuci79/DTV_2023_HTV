/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_channel_scan_controller.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELSCANCONTROLLERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACHANNELSCANCONTROLLERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_channel_scan_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaSatelliteHelperIpcTypes.hpp"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_GetScanConfiguration);
DECLARE_IPC_NAME(NEBULA_StartChannelScan);
DECLARE_IPC_NAME(NEBULA_StopChannelScan);
DECLARE_IPC_NAME(NEBULA_SetChannelSearchParameters);
DECLARE_IPC_NAME(NEBULA_SetSatelliteScanParameters);
DECLARE_IPC_NAME(NEBULA_SetSatelliteManualSearchParams);
DECLARE_IPC_NAME(NEBULA_GetSatelliteManualSearchInitialParams);
DECLARE_IPC_NAME(NEBULA_SetCableManualSearchParams);
DECLARE_IPC_NAME(NEBULA_GetCableManualSearchInitialParams);
DECLARE_IPC_NAME(NEBULA_GetTerrestrialManualSearchInitialParams);
DECLARE_IPC_NAME(NEBULA_SetTerrestrialFrequency);
DECLARE_IPC_NAME(NEBULA_SetTerrestrialChannel);
DECLARE_IPC_NAME(NEBULA_SetTerrestrialBand);
DECLARE_IPC_NAME(NEBULA_SetTerrestrialManualSearchParams);
DECLARE_IPC_NAME(NEBULA_GetAnalogManualSearchInitialParams);
DECLARE_IPC_NAME(NEBULA_SetAnalogTvSystem);
DECLARE_IPC_NAME(NEBULA_SetAnalogChannel);
DECLARE_IPC_NAME(NEBULA_SetAnalogBand);
DECLARE_IPC_NAME(NEBULA_SetAnalogFrequency);
DECLARE_IPC_NAME(NEBULA_SetAnalogManualSearchParams);
DECLARE_IPC_NAME(NEBULA_SetLCNNumbering);
DECLARE_IPC_NAME(NEBULA_ResolveLCNConflict);
DECLARE_IPC_NAME(NEBULA_TuneToReferenceTransponder);
DECLARE_IPC_NAME(NEBULA_GetSignalParameters);
DECLARE_IPC_NAME(NEBULA_TuneToUpdatedAnalogChannelDetails);
DECLARE_IPC_NAME(NEBULA_UpdateAnalogFineTuneFrequency);
DECLARE_IPC_NAME(NEBULA_TuneToAnalogManualChannel);
DECLARE_IPC_NAME(NEBULA_TuneToSatelliteManualChannel);
DECLARE_IPC_NAME(NEBULA_TuneToCableManualChannel);
DECLARE_IPC_NAME(NEBULA_TuneToTerrestrialManualChannel);
DECLARE_IPC_NAME(NEBULA_GetAvailableTerrestrialBands);
DECLARE_IPC_NAME(NEBULA_GetAvailableTerrestrialChannels);

DECLARE_IPC_NAME(NEBULA_RegisterScanEventCallback);
DECLARE_IPC_NAME(NEBULA_DeregisterScanEventCallback);
DECLARE_IPC_NAME(NEBULA_ScanEventCallback);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_ModulationType);
MSGPACK_ADD_ENUM(NEBULA_AnalogTVSystem);
MSGPACK_ADD_ENUM(NEBULA_TerresterialBandType);
MSGPACK_ADD_ENUM(NEBULA_AnalogBandType);
MSGPACK_ADD_ENUM(NEBULA_NetworkType);
MSGPACK_ADD_ENUM(NEBULA_ScanEventType);
MSGPACK_ADD_ENUM(NEBULA_DigiturkInstallationStatus);
MSGPACK_ADD_ENUM(NEBULA_DsmartInstallationStatus);
MSGPACK_ADD_ENUM(NEBULA_AstraHdChannelListUpdateStatus);
MSGPACK_ADD_ENUM(NEBULA_AstraChannelListUpdateFailReason);
MSGPACK_ADD_ENUM(NEBULA_SgtQueryEvent);
MSGPACK_ADD_ENUM(NEBULA_TkgsQueryEventType);

struct MP_NEBULA_ScanConfiguration : public NEBULA_ScanConfiguration
{
    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_ScanConfiguration,
                   min_terrestrial_frequency,
                   max_terrestrial_frequency,
                   min_analog_frequency,
                   max_analog_frequency,
                   min_cable_frequency,
                   max_cable_frequency,
                   min_cable_symbol_rate,
                   max_cable_symbol_rate,
                   analog_fine_tune_step_size_range);
};

struct MP_NEBULA_CableConfiguration : public NEBULA_CableConfiguration
{
    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_CableConfiguration,
                   cable_tuner_frequency,
                   symbol_rate,
                   modulation);
};

struct MP_NEBULA_TerrestrialConfiguration : public NEBULA_TerrestrialConfiguration
{
    MP_NEBULA_TerrestrialConfiguration()
    {
        channel_index_str[0] = '\0';
        error_message[0] = '\0';
    }

    MSGPACK_DEFINE(band,
                   frequency,
                   channel_index_str,
                   error_message);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_TerrestrialConfiguration,
                   band,
                   frequency,
                   "",
                   "");
};

struct MP_NEBULA_AnalogConfiguration : public NEBULA_AnalogConfiguration
{
    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_AnalogConfiguration,
                   band,
                   channel,
                   frequency,
                   tv_system,
                   fine_tune_offset,
                   is_valid);
};

struct MP_NEBULA_SignalParameters : public NEBULA_SignalParameters
{
    MEMBER_CHECKED_MSGPACK_DEFINE(NEBULA_SignalParameters,
                   signal_strength,
                   signal_quality,
                   ber_value);
};

struct MP_NEBULA_ScanEventData : public NEBULA_ScanEventData
{
    std::vector<frost_int> packed_channel_id_array;
    
    MP_NEBULA_ScanEventData() = default;

    MP_NEBULA_ScanEventData(const NEBULA_ScanEventData& other)
        : NEBULA_ScanEventData(other),
          packed_channel_id_array(other.channel_id_array,
                                  other.channel_id_array + other.channel_count)
    {}

    MSGPACK_DEFINE(event_type,
                   frequency_name,
                   percentage,
                   channel_count,
                   packed_channel_id_array,
                   satellite_name,
                   polarization,
                   digiturk_installation_status,
                   dsmart_status,
                   dsmart_diseqc_num,
                   astra_hd_channel_list_update_status,
                   astra_hd_channel_list_update_fail_reason,
                   astra_hd_sgt_query,
                   tkgs_query_event,
                   standby_search);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_ScanEventData,
                   event_type,
                   "",
                   percentage,
                   channel_count,
                   nullptr,
                   "",
                   'a',
                   digiturk_installation_status,
                   dsmart_status,
                   dsmart_diseqc_num,
                   astra_hd_channel_list_update_status,
                   astra_hd_channel_list_update_fail_reason,
                   astra_hd_sgt_query,
                   tkgs_query_event,
                   standby_search);
};

#endif
