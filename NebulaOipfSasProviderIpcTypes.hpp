/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_oipf_sas_provider.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAOIPFSASPROVIDERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAOIPFSASPROVIDERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_oipf_sas_provider.h"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_OipfSasRegisterCallback);
DECLARE_IPC_NAME(NEBULA_GlobalDrmClientRegisterCallback);
DECLARE_IPC_NAME(NEBULA_OipfSasCallback);
DECLARE_IPC_NAME(NEBULA_OipfSasDeregisterCallback);
DECLARE_IPC_NAME(NEBULA_OipfSasSendMessage);
DECLARE_IPC_NAME(NEBULA_OipfSasSendCanPlayRequest);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_OipfSasCallbackType);

#endif