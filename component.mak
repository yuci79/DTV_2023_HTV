VEWD_SDK_INCLUDE = $(VEWD_SDK_DIST)/include

COMPONENT_CXXFLAGS += -fPIC

$(info [DEBUG] Entering avcontrolobject/component.mak)

OBJS +=\
    avcontrolobject/AVControlObjectIpcTypes \
    avcontrolobject/AVControlObjectIpcServer \
    avcontrolobject/AVControlObjectIpcClient

COMPONENT_SYSTEM_INCLUDES += $(VEWD_SDK_INCLUDE)

COMPONENT_DEFINES += UVA_INCLUDE_OOIF_TYPES

# Create symbolic link in derived
LINK_NAMES = avcontrolobject
TARGET_NAME_avcontrolobject = vewd_integration/ipc/avcontrolobject

DIRS_IN_DERIVED := $(call derived, $(LINK_NAMES))
CBUILD_FIRST: $(DIRS_IN_DERIVED)

$(DIRS_IN_DERIVED): vewd_integration/ipc/avcontrolobject/component.mak
	@echo Creating recursive symlink $@
	$(AT)rm -rf $@
	$(AT)mkdir -p $@
	$(AT)cp -rs $(BUILDROOT)/$(TARGET_NAME_$(@F))/* $@

$(info [DEBUG] Leaving avcontrolobject/component.mak)