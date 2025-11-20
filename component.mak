
OBJS +=\
    FrostDebugCallbacksIpcTypes \
    aurora/AsyncMessageCallbacksIpcAurora \
    aurora/AuroraRpcClient \
    aurora/FrostDebugCallbacksIpcAurora \
    aurora/VideoBroadcastObjectProxyIpcAurora \
    aurora/NebulaIpcServer \
    aurora/NebulaIpcTrace \
    aurora/NebulaApplicationCapabilityIpcAurora \
    aurora/NebulaAvControlIpcAurora \
    aurora/NebulaBluetoothSettingsIpcAurora \
    aurora/NebulaBluetoothSettingsCallbacksIpcAurora \
    aurora/NebulaBroadcastComponentsIpcAurora \
    aurora/NebulaCaControllerIpcAurora \
    aurora/NebulaChannelChangeIpcAurora \
    aurora/NebulaChannelChangeCallbacksIpcAurora \
    aurora/NebulaChannelInfoIpcAurora \
    aurora/NebulaChannelScanControllerIpcAurora \
    aurora/NebulaChannelScanControllerCallbacksIpcAurora \
    aurora/NebulaComponentCallbackIpcAurora \
    aurora/NebulaComponentCallbackCallbacksIpcAurora \
    aurora/NebulaConfigurationIpcAurora \
    aurora/NebulaCountrySettingsIpcAurora \
    aurora/NebulaDatetimeSettingsIpcAurora \
    aurora/NebulaDatetimeSettingsCallbacksIpcAurora \
    aurora/NebulaDsmccIpcAurora \
    aurora/NebulaDsmccCallbacksIpcAurora \
    aurora/NebulaEitQueryIpcAurora \
    aurora/NebulaEitQueryCallbacksIpcAurora \
    aurora/NebulaExternalAvSourceNavigatorIpcAurora \
    aurora/NebulaExternalAvSourceNavigatorCallbacksIpcAurora \
    aurora/NebulaLanguageSettingsIpcAurora \
    aurora/NebulaMhegIpcAurora \
    aurora/NebulaNetworkSettingsIpcAurora \
    aurora/NebulaNetworkSettingsCallbacksIpcAurora \
    aurora/NebulaOipfSasProviderIpcAurora \
    aurora/NebulaOipfSasProviderCallbacksIpcAurora \
    aurora/NebulaParentalControlIpcAurora \
    aurora/NebulaParentalControlCallbacksIpcAurora \
    aurora/NebulaPresentSiQueryIpcAurora \
    aurora/NebulaPresentSiQueryCallbacksIpcAurora \
    aurora/NebulaPvrConfigurationIpcAurora \
    aurora/NebulaPvrDiskControllerIpcAurora \
    aurora/NebulaPvrRecordingSchedulerIpcAurora \
    aurora/NebulaPvrRecordingSchedulerCallbacksIpcAurora \
    aurora/NebulaRemovableDeviceControllerIpcAurora \
    aurora/NebulaSatelliteHelperIpcAurora \
    aurora/NebulaServiceListUpdateControllerIpcAurora \
    aurora/NebulaServiceListUpdateControllerCallbacksIpcAurora \
    aurora/NebulaSignalInfoIpcAurora \
    aurora/NebulaSoundSettingsIpcAurora \
    aurora/NebulaStandbyControllerIpcAurora \
    aurora/NebulaStreamEventsIpcAurora \
    aurora/NebulaStreamEventsCallbacksIpcAurora \
    aurora/NebulaSubtitleSettingsIpcAurora \
    aurora/NebulaSystemInformationProviderCallbacksIpcAurora \
    aurora/NebulaSystemInformationProviderIpcAurora \
    aurora/NebulaTemiTimelineIpcAurora \
    aurora/NebulaTimeshiftCallbacksIpcAurora \
    aurora/NebulaTimeshiftIpcAurora \
    aurora/NebulaTunerConfigurationIpcAurora \
    aurora/NebulaWebBrowserIpcAurora \
    aurora/NebulaBrowserBackendIpcAurora
    
ifdef INCLUDE_AVS
OBJS +=\
    aurora/NebulaAVSClientIpcAurora 
endif
    
WARNLEVEL := high
