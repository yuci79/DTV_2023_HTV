/**
 * Copyright 2022 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaChannelScanControllerIpcAurora class.
 */

#include "NebulaChannelScanControllerIpcAurora.hpp"
#include "nebula/adaptation_layer/nebula_channel_scan_controller.h"
#include "nebula_src/adaptation_layer/ipc/NebulaChannelScanControllerIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaSatelliteHelperIpcTypes.hpp"
#include <rpc/server.h>

void
NebulaChannelScanControllerIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_GetScanConfiguration), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_ScanConfiguration> result;
        result.return_value = NEBULA_GetScanConfiguration(&result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_StartChannelScan), &NEBULA_StartChannelScan);
    server.bind(IPC_NAME(NEBULA_StopChannelScan), &NEBULA_StopChannelScan);
    server.bind(IPC_NAME(NEBULA_SetChannelSearchParameters), &NEBULA_SetChannelSearchParameters);
    server.bind(IPC_NAME(NEBULA_SetSatelliteScanParameters), &NEBULA_SetSatelliteScanParameters);
    server.bind(IPC_NAME(NEBULA_SetSatelliteManualSearchParams), &NEBULA_SetSatelliteManualSearchParams);

    server.bind(IPC_NAME(NEBULA_GetSatelliteManualSearchInitialParams), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_TransponderInfo> result;
        result.return_value = NEBULA_GetSatelliteManualSearchInitialParams(&result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetCableManualSearchParams), &NEBULA_SetCableManualSearchParams);

    server.bind(IPC_NAME(NEBULA_GetCableManualSearchInitialParams), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_CableConfiguration> result;
        result.return_value = NEBULA_GetCableManualSearchInitialParams(&result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_GetTerrestrialManualSearchInitialParams), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_TerrestrialConfiguration> result;
        result.return_value = NEBULA_GetTerrestrialManualSearchInitialParams(&result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetTerrestrialFrequency), [](frost_int frequency_khz)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_TerrestrialConfiguration> result;
        result.return_value = NEBULA_SetTerrestrialFrequency(&result.output_params, frequency_khz);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetTerrestrialChannel), [](frost_int channel,
                                                           std::string channel_str)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_TerrestrialConfiguration> result;
        result.return_value = NEBULA_SetTerrestrialChannel(&result.output_params,
                                                           channel,
                                                           const_cast<char*>(channel_str.c_str()));
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetTerrestrialBand), [](NEBULA_TerresterialBandType band)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_TerrestrialConfiguration> result;
        result.return_value = NEBULA_SetTerrestrialBand(&result.output_params, band);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetTerrestrialManualSearchParams), &NEBULA_SetTerrestrialManualSearchParams);

    server.bind(IPC_NAME(NEBULA_GetAnalogManualSearchInitialParams), []()
    {
        NebulaIpcType<frost_bool, MP_NEBULA_AnalogConfiguration> result;
        result.return_value = NEBULA_GetAnalogManualSearchInitialParams(&result.output_params);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetAnalogTvSystem), [](NEBULA_AnalogTVSystem tv_system)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_AnalogConfiguration> result;
        result.return_value = NEBULA_SetAnalogTvSystem(&result.output_params, tv_system);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetAnalogChannel), [](frost_int channel)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_AnalogConfiguration> result;
        result.return_value = NEBULA_SetAnalogChannel(&result.output_params, channel);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetAnalogBand), [](NEBULA_AnalogBandType band)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_AnalogConfiguration> result;
        result.return_value = NEBULA_SetAnalogBand(&result.output_params, band);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetAnalogFrequency), [](frost_int frequency)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_AnalogConfiguration> result;
        result.return_value = NEBULA_SetAnalogFrequency(&result.output_params, frequency);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_SetAnalogManualSearchParams), &NEBULA_SetAnalogManualSearchParams);
    server.bind(IPC_NAME(NEBULA_SetLCNNumbering), &NEBULA_SetLCNNumbering);
    server.bind(IPC_NAME(NEBULA_ResolveLCNConflict), &NEBULA_ResolveLCNConflict);
    server.bind(IPC_NAME(NEBULA_TuneToReferenceTransponder), &NEBULA_TuneToReferenceTransponder);

    server.bind(IPC_NAME(NEBULA_GetSignalParameters), [](frost_int tuner_index)
    {
        NebulaIpcType<frost_bool, MP_NEBULA_SignalParameters> result;
        result.return_value = NEBULA_GetSignalParameters(&result.output_params, tuner_index);
        return result;
    });

    server.bind(IPC_NAME(NEBULA_TuneToUpdatedAnalogChannelDetails), &NEBULA_TuneToUpdatedAnalogChannelDetails);
    server.bind(IPC_NAME(NEBULA_UpdateAnalogFineTuneFrequency), &NEBULA_UpdateAnalogFineTuneFrequency);
    server.bind(IPC_NAME(NEBULA_TuneToAnalogManualChannel), &NEBULA_TuneToAnalogManualChannel);
    server.bind(IPC_NAME(NEBULA_TuneToSatelliteManualChannel), &NEBULA_TuneToSatelliteManualChannel);
    server.bind(IPC_NAME(NEBULA_TuneToCableManualChannel), &NEBULA_TuneToCableManualChannel);
    server.bind(IPC_NAME(NEBULA_TuneToTerrestrialManualChannel), &NEBULA_TuneToTerrestrialManualChannel);

    server.bind(IPC_NAME(NEBULA_GetAvailableTerrestrialBands), []()
    {
        std::string result;
        char* channels = nullptr;
        NEBULA_GetAvailableTerrestrialBands(&channels);
        if (channels != nullptr)
        {
            result = channels;
            free(channels);
        }        
        return result;
    });

    server.bind(IPC_NAME(NEBULA_GetAvailableTerrestrialChannels), [](NEBULA_TerresterialBandType band)
    {
        std::string result;
        char* channels = nullptr;
        NEBULA_GetAvailableTerrestrialChannels(&channels, band);
        if (channels != nullptr)
        {
            result = channels;
            free(channels);
        }
        return result;
    });
}