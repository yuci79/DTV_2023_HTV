/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_channel_scan_controller.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaChannelScanControllerIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaSatelliteHelperIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"

frost_bool
NEBULA_GetScanConfiguration(NEBULA_ScanConfiguration * configuration)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_ScanConfiguration>(*configuration,
                        IPC_NAME(NEBULA_GetScanConfiguration));
}

frost_bool
NEBULA_StartChannelScan(frost_bool manual_scan, frost_bool network_scan)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StartChannelScan), manual_scan, network_scan);
}

frost_bool
NEBULA_StopChannelScan(void)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_StopChannelScan));
}

frost_bool
NEBULA_SetChannelSearchParameters(NEBULA_NetworkType network_type,
                                  frost_int start_frequency,
                                  frost_int stop_frequency,
                                  frost_int search_step)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetChannelSearchParameters),
                                     network_type,
                                     start_frequency,
                                     stop_frequency,
                                     search_step);
}

frost_bool
NEBULA_SetSatelliteScanParameters(frost_int satellite_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetSatelliteScanParameters), satellite_index);
}

frost_bool
NEBULA_SetSatelliteManualSearchParams(frost_int satellite_index,
                                      frost_int frequency,
                                      frost_int symbol_rate,
                                      NEBULA_PolarisationType polarization)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetSatelliteManualSearchParams),
                                     satellite_index,
                                     frequency,
                                     symbol_rate,
                                     polarization);
}

frost_bool
NEBULA_GetSatelliteManualSearchInitialParams(NEBULA_TransponderInfo* transp_info)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TransponderInfo>(*transp_info,
                        IPC_NAME(NEBULA_GetSatelliteManualSearchInitialParams));
}

frost_bool
NEBULA_SetCableManualSearchParams(frost_int cable_tuner_freq,
                                  frost_int symbol_rate,
                                  NEBULA_ModulationType modulation)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetCableManualSearchParams),
                                     cable_tuner_freq,
                                     symbol_rate,
                                     modulation);
}

frost_bool
NEBULA_GetCableManualSearchInitialParams(NEBULA_CableConfiguration* cable_conf)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_CableConfiguration>(*cable_conf,
                        IPC_NAME(NEBULA_GetCableManualSearchInitialParams));
}

frost_bool
NEBULA_GetTerrestrialManualSearchInitialParams(NEBULA_TerrestrialConfiguration* terrestrial_conf)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TerrestrialConfiguration>(*terrestrial_conf,
                        IPC_NAME(NEBULA_GetTerrestrialManualSearchInitialParams));
}

frost_bool
NEBULA_SetTerrestrialFrequency(NEBULA_TerrestrialConfiguration* terrestrial_conf,
                               frost_int frequency_khz)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TerrestrialConfiguration>(*terrestrial_conf,
                        IPC_NAME(NEBULA_SetTerrestrialFrequency),
                        frequency_khz);
}

frost_bool
NEBULA_SetTerrestrialChannel(NEBULA_TerrestrialConfiguration* terrestrial_conf,
                             frost_int channel,
                             char* channel_str)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TerrestrialConfiguration>(*terrestrial_conf,
                        IPC_NAME(NEBULA_SetTerrestrialChannel),
                        channel,
                        channel_str);
}

frost_bool
NEBULA_SetTerrestrialBand(NEBULA_TerrestrialConfiguration* terrestrial_conf,
                          NEBULA_TerresterialBandType band)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_TerrestrialConfiguration>(*terrestrial_conf,
                        IPC_NAME(NEBULA_SetTerrestrialBand),
                        band);
}

frost_bool
NEBULA_SetTerrestrialManualSearchParams(frost_int frequency_mhz)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetTerrestrialManualSearchParams),
                                     frequency_mhz);
}

frost_bool
NEBULA_GetAnalogManualSearchInitialParams(NEBULA_AnalogConfiguration* analog_conf)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_AnalogConfiguration>(*analog_conf,
                        IPC_NAME(NEBULA_GetAnalogManualSearchInitialParams));
}

frost_bool
NEBULA_SetAnalogTvSystem(NEBULA_AnalogConfiguration* analog_conf, NEBULA_AnalogTVSystem tv_system)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_AnalogConfiguration>(*analog_conf,
                        IPC_NAME(NEBULA_SetAnalogTvSystem),
                        tv_system);
}

frost_bool
NEBULA_SetAnalogChannel(NEBULA_AnalogConfiguration* analog_conf, frost_int channel)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_AnalogConfiguration>(*analog_conf,
                        IPC_NAME(NEBULA_SetAnalogChannel),
                        channel);
}

frost_bool
NEBULA_SetAnalogBand(NEBULA_AnalogConfiguration* analog_conf, NEBULA_AnalogBandType band)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_AnalogConfiguration>(*analog_conf,
                        IPC_NAME(NEBULA_SetAnalogBand),
                        band);
}

frost_bool
NEBULA_SetAnalogFrequency(NEBULA_AnalogConfiguration* analog_conf, frost_int frequency)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_AnalogConfiguration>(*analog_conf,
                        IPC_NAME(NEBULA_SetAnalogFrequency),
                        frequency);
}

frost_bool
NEBULA_SetAnalogManualSearchParams(frost_int frequency, NEBULA_AnalogTVSystem tv_system)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetAnalogManualSearchParams),
                                     frequency,
                                     tv_system);
}

frost_bool
NEBULA_SetLCNNumbering(frost_bool enabled)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_SetLCNNumbering), enabled);
}

frost_bool
NEBULA_ResolveLCNConflict(frost_int conflicted_service_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_ResolveLCNConflict), conflicted_service_index);
}

frost_bool
NEBULA_TuneToReferenceTransponder(frost_int lnb_id, frost_int tuner_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToReferenceTransponder),
                                     lnb_id,
                                     tuner_index);
}

frost_bool
NEBULA_GetSignalParameters(NEBULA_SignalParameters* signal_params, frost_int tuner_index)
{
    return nebulaRpcCall<frost_bool, MP_NEBULA_SignalParameters>(*signal_params,
                        IPC_NAME(NEBULA_GetSignalParameters), tuner_index);
}

frost_bool
NEBULA_TuneToUpdatedAnalogChannelDetails(frost_int fine_tune_offset)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToUpdatedAnalogChannelDetails),
                                     fine_tune_offset);
}

frost_bool
NEBULA_UpdateAnalogFineTuneFrequency(frost_int fine_tune_offset)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_UpdateAnalogFineTuneFrequency),
                                     fine_tune_offset);
}

frost_bool
NEBULA_TuneToAnalogManualChannel(frost_int frequency, NEBULA_AnalogTVSystem tv_system)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToAnalogManualChannel),
                                     frequency,
                                     tv_system);
}

frost_bool
NEBULA_TuneToSatelliteManualChannel(frost_int frequency_mhz,
                                    frost_int symbol_rate,
                                    NEBULA_PolarisationType polarisation,
                                    frost_int satellite_index)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToSatelliteManualChannel),
                                     frequency_mhz,
                                     symbol_rate,
                                     polarisation,
                                     satellite_index);
}

frost_bool
NEBULA_TuneToCableManualChannel(frost_int frequency,
                                frost_int symbol_rate,
                                NEBULA_ModulationType modulation)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToCableManualChannel),
                                     frequency,
                                     symbol_rate,
                                     modulation);
}

frost_bool
NEBULA_TuneToTerrestrialManualChannel(frost_int frequency)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_TuneToTerrestrialManualChannel), frequency);
}

void
NEBULA_GetAvailableTerrestrialBands(char** available_channels)
{
    const std::string channels = nebulaRpcCall<std::string>(IPC_NAME(NEBULA_GetAvailableTerrestrialBands));    
    *available_channels = strdup(channels.c_str());
}

void
NEBULA_GetAvailableTerrestrialChannels(char** available_channels, NEBULA_TerresterialBandType band)
{
    const std::string channels = nebulaRpcCall<std::string>(IPC_NAME(NEBULA_GetAvailableTerrestrialChannels),
                                                            band);    
    *available_channels = strdup(channels.c_str());
}