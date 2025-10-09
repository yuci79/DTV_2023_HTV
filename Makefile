#
# The output of the make process will be videobackend.so
#
# This makefile contains some defines
#
# CROSS
# Cross compiler sufflix used (if any).
#
# UVA_INCLUDE
# The directory containing UVA headers.
#
# UVA_INCPATH
# Include paths that should be added during compilation
#
# UVA_LIBDIR
# The directory containing libuva.so

UVA_INCLUDE ?= ../../include
UVA_LIBDIR ?= ../..
OUTPUT_DIR ?= ../../work
#USING_SDK423 = YES

PLUGIN ?= libuvabackend-mumin.so
CXX ?= ${CROSS}g++
CC ?= ${CROSS}gcc
INCLUDE_PATH := -I.
INCLUDE_PATH += $(addprefix -I, $(UVA_INCLUDE))
CFLAGS := -Wall

CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUVA_MEMORY_NEED_STL_ALLOCATOR -DUVA_INCLUDE_OOIF_TYPES
CFLAGS += ${PLATFORM_CXXFLAGS}

ifneq ($(UVA_INCPATH),)
  INCLUDE_PATH += $(addprefix -I, $(UVA_INCPATH))
endif

ifneq ($(PLATFORM_DEFINES),)
  INCLUDE_PATH += $(addprefix -D, $(PLATFORM_DEFINES))
endif

ifeq ($(DEBUG), YES)
	CFLAGS	+=-g -DDEBUG
else
	CFLAGS	+= -O2
endif

USE_FACTORY_STUB =
ifeq ($(USING_SDK422), YES)
USE_FACTORY_STUB = 1
endif
ifeq ($(USING_SDK423), YES)
USE_FACTORY_STUB = 1
endif
ifdef USE_FACTORY_STUB
  BACKEND_PLUGIN=OperaMuminFactoryStub.o
else
  BACKEND_PLUGIN=OperaMuminFactory.o
endif

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
  CFLAGS += -fPIC
else
  CFLAGS += -fpic
endif

CFLAGS += -lrt

ifneq ($(UVA_COMPILERFLAGS),)
  CFLAGS += $(UVA_COMPILERFLAGS)
endif

BACKEND_LDFLAGS := -L$(UVA_LIBDIR) -luva

default all: $(OUTPUT_DIR)/$(PLUGIN)

$(OUTPUT_DIR)/$(PLUGIN): $(OUTPUT_DIR)/%.so : $(OUTPUT_DIR)/$(BACKEND_PLUGIN)
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(BACKEND_LDFLAGS) $(INCLUDE_PATH) $^

$(OUTPUT_DIR)/$(BACKEND_PLUGIN): $(OUTPUT_DIR)/%.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) -c $<

clean:
	rm -rf $(PLUGIN) $(BACKEND_PLUGIN)
