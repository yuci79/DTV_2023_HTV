VEWD_SDK_INCLUDE = $(VEWD_SDK_DIST)/include

COMPONENT_CXXFLAGS += -fPIC

$(info [DEBUG] Entering ipc/component.mak)
$(info [DEBUG] OBJS before assignment: $(OBJS))

# Nebula IPC and related source files for the mumin backend library.

# Local sources below this component.mak.
OBJS +=\
    browser/OperaWindowManagerProxyIpcBrowser \
    BrowserIpcTrace \
    BrowserCallbackServer \
    OperaBrowserBackendCore \
    PluginTrace

# Shared and distant sources.
OBJS +=\
    rpclib/CommonIpcServer \
    rpclib/RpcClient \
    nebula_external/ExternalCallbackServer \
    nebula_external/ExternalIpcTrace \
    nebula_external/AsyncMessageCallbacksIpcExternal \
    nebula_external/FrostDebugCallbacksIpcExternal \
    nebula_external/NebulaDrmRightsMessageProcessorIpcExternal \
    nebula_external/NebulaParentalRatingMessageProcessorIpcExternal \
    nebula_external/NebulaBluetoothSettingsCallbacksIpcExternal \
    nebula_external/NebulaBluetoothSettingsCallbackIpcTypesExternal \
    nebula_external/NebulaBroadcastComponentsIpcExternal \
    nebula_external/NebulaChannelChangeCallbacksIpcExternal \
    nebula_external/NebulaChannelChangeCallbackIpcTypesExternal \
    nebula_external/NebulaChannelScanControllerCallbacksIpcExternal \
    nebula_external/NebulaChannelScanControllerCallbackIpcTypesExternal \
    nebula_external/NebulaComponentCallbackCallbacksIpcExternal \
    nebula_external/NebulaComponentCallbackCallbackIpcTypesExternal \
    nebula_external/NebulaDatetimeSettingsCallbacksIpcExternal \
    nebula_external/NebulaDatetimeSettingsCallbackIpcTypesExternal \
    nebula_external/NebulaDsmccCallbackIpcTypesExternal \
    nebula_external/NebulaDsmccCallbacksIpcExternal \
    nebula_external/NebulaEitQueryIpcTypesExternal \
    nebula_external/NebulaEitQueryCallbacksIpcExternal \
    nebula_external/NebulaEitQueryCallbackIpcTypesExternal \
    nebula_external/NebulaExternalAvSourceNavigatorCallbacksIpcExternal \
    nebula_external/NebulaExternalAvSourceNavigatorCallbackIpcTypesExternal \
    nebula_external/NebulaIpcTypesExternal \
    nebula_external/NebulaNetworkSettingsCallbacksIpcExternal \
    nebula_external/NebulaNetworkSettingsCallbackIpcTypesExternal \
    nebula_external/NebulaOipfSasProviderCallbacksIpcExternal \
    nebula_external/NebulaOipfSasProviderCallbackIpcTypesExternal \
    nebula_external/NebulaParentalControlCallbacksIpcExternal \
    nebula_external/NebulaParentalControlCallbackIpcTypesExternal \
    nebula_external/NebulaPresentSiQueryCallbacksIpcExternal \
    nebula_external/NebulaPresentSiQueryCallbackIpcTypesExternal \
    nebula_external/NebulaPvrRecordingSchedulerCallbacksIpcExternal \
    nebula_external/NebulaPvrRecordingSchedulerCallbackIpcTypesExternal \
    nebula_external/NebulaServiceListUpdateControllerCallbacksIpcExternal \
    nebula_external/NebulaServiceListUpdateControllerCallbackIpcTypesExternal \
    nebula_external/NebulaStreamEventsCallbacksIpcExternal \
    nebula_external/NebulaStreamEventsCallbackIpcTypesExternal \
    nebula_external/NebulaSystemInformationProviderCallbacksIpcExternal \
    nebula_external/NebulaSystemInformationProviderCallbackIpcTypesExternal \
    nebula_external/NebulaTimeshiftCallbacksIpcExternal \
    nebula_external/NebulaTimeshiftCallbackIpcTypesExternal \
    nebula_external/nebula_application_capability_ipc_external \
    nebula_external/nebula_av_control_ipc_external \
    nebula_external/nebula_avs_client_ipc_external \
    nebula_external/nebula_bluetooth_settings_ipc_external \
    nebula_external/nebula_ca_controller_ipc_external \
    nebula_external/nebula_channel_change_ipc_external \
    nebula_external/nebula_channel_info_ipc_external \
    nebula_external/nebula_channel_scan_controller_ipc_external \
    nebula_external/nebula_component_callback_ipc_external \
    nebula_external/nebula_configuration_ipc_external \
    nebula_external/nebula_country_settings_ipc_external \
    nebula_external/nebula_datetime_settings_ipc_external \
    nebula_external/nebula_dsmcc_ipc_external \
    nebula_external/nebula_eit_query_ipc_external \
    nebula_external/nebula_external_av_source_navigator_ipc_external \
    nebula_external/nebula_language_settings_ipc_external \
    nebula_external/nebula_mheg_ipc_external \
    nebula_external/nebula_network_settings_ipc_external \
    nebula_external/nebula_removable_device_controller_ipc_external \
    nebula_external/nebula_oipf_sas_provider_ipc_external \
    nebula_external/nebula_parental_control_ipc_external \
    nebula_external/nebula_present_si_query_ipc_external \
    nebula_external/nebula_pvr_configuration_ipc_external \
    nebula_external/nebula_pvr_disk_controller_ipc_external \
    nebula_external/nebula_pvr_recording_scheduler_ipc_external \
    nebula_external/nebula_satellite_helper_ipc_external \
    nebula_external/nebula_service_list_update_controller_ipc_external \
    nebula_external/nebula_signal_info_ipc_external \
    nebula_external/nebula_sound_settings_ipc_external \
    nebula_external/nebula_standby_controller_ipc_external \
    nebula_external/nebula_stream_events_ipc_external \
    nebula_external/nebula_subtitle_settings_ipc_external \
    nebula_external/nebula_system_information_provider_ipc_external \
    nebula_external/nebula_temi_timeline_ipc_external \
    nebula_external/nebula_timeshift_ipc_external \
    nebula_external/nebula_tuner_configuration_ipc_external \
    nebula_external/nebula_web_browser_ipc_external \
    vewd_common_browser/bpi_graphics_ipc_browser \
    vewd_common_browser/bpi_window_manager_ipc_browser \
    vewd_common_browser/bpi_nativeui_controller_ipc_browser \
    vewd_jspp/NativeOperaWindowManagerCallbacksIpcBrowser \

# ifdef INCLUDE_HOTELTV
# OBJS += \
#    nebula_external/OperaBrowserBackendExternal 
# endif

# Add to OBJS list
OBJS +=\
    avcontrolobject/AVControlObjectIpcServer \
    avcontrolobject/AVControlObjectIpcClient \
    avcontrolobject/AVControlObjectIpcTypes





ifdef INCLUDE_OPAPP_SUPPORT
OBJS += \
    nebula_opapp_external/nebula_opapp_channel_list_modifier_ipc_external \
    nebula_opapp_external/nebula_opapp_event_publisher_provider_ipc_external \
    nebula_opapp_external/NebulaOpAppChannelListModifierIpcExternal \
    nebula_opapp_external/NebulaOpAppEventPublisherProviderCallbackIpcTypesExternal \
    nebula_opapp_external/NebulaOpAppEventPublisherProviderCallbacksIpcExternal \
    nebula_opapp_external/NebulaOpAppEventPublisherProviderIpcExternal
endif

$(info [DEBUG] OBJS after assignment: $(OBJS))
$(info [DEBUG] COMPONENT_DEFINES: $(COMPONENT_DEFINES))
$(info [DEBUG] COMPONENT_INCLUDES: $(COMPONENT_INCLUDES))
VEWD_MUMIN_INCLUDES += \
    $(VEWD_SDK_DIST)/src/uva \
    $(VEWD_SDK_DIST)/src/uva/include \

COMPONENT_SYSTEM_INCLUDES += $(VEWD_SDK_INCLUDE)
COMPONENT_SYSTEM_INCLUDES += $(VEWD_MUMIN_INCLUDES)

COMPONENT_DEFINES += UVA_INCLUDE_OOIF_TYPES

#
# Create symbolic links in derived to selected source tree directories.
#
LINK_NAMES = rpclib nebula_external vewd_common_browser vewd_jspp

# Add to LINK_NAMES
LINK_NAMES += avcontrolobject
TARGET_NAME_avcontrolobject = vewd_integration/ipc/avcontrolobject

TARGET_NAME_rpclib = 3rd_party_wrapper/rpclib/src
TARGET_NAME_nebula_external = nebula_src/adaptation_layer/ipc/external
TARGET_NAME_vewd_common_browser = vewd_common/ipc/browser
TARGET_NAME_vewd_jspp = vewd_integration/jspp_plugins/ipc/browser

# Add Nebula core where AVControlObject and MediaDataSource live
LINK_NAMES += nebula_src
TARGET_NAME_nebula_src = nebula_src

# Ensure nebula_src derived directory is created before the ipc component is linked.
# This forces nebula objects/libraries to exist before the backend link step.
CBUILD_FIRST += $(call derived, nebula_src)

ifdef INCLUDE_OPAPP_SUPPORT
LINK_NAMES += nebula_opapp_external opapp

TARGET_NAME_nebula_opapp_external = nebula_src/opapp/ipc/external
TARGET_NAME_opapp = vewd_integration/opapp
endif

DIRS_IN_DERIVED := $(call derived, $(LINK_NAMES))
CBUILD_FIRST: $(DIRS_IN_DERIVED)

$(DIRS_IN_DERIVED): vewd_integration/ipc/component.mak
	@echo Creating recursive symlink $@
	$(AT)rm -rf $@
	$(AT)mkdir -p $@
	$(AT)cp -rs $(BUILDROOT)/$(TARGET_NAME_$(@F))/* $@
