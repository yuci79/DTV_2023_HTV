/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        frost debug callbacks.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_FROSTDEBUGCALLBACKSIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_FROSTDEBUGCALLBACKSIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "frost/frost_debug.h"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(FROST_RegisterCommand);
DECLARE_IPC_NAME(FROST_RegisteredCommandCallback);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(FROST_DebugParamType);

struct MP_FROST_RegisterCommandCallbackData
{
    /**
     * @brief Default constructor.
     */
    MP_FROST_RegisterCommandCallbackData() = default;

    /**
     * @brief Constructor to copy from the base class. Required for use as a parameter in 
     *        rpc::client::call().
     */
    MP_FROST_RegisterCommandCallbackData(const MP_FROST_RegisterCommandCallbackData&) = default;

    /**
      * @brief Constructor to pack FROST_DebugParamType for IPC.
      * @param param_type Is the parameter type.
      * @param param Is the parameter.
      */
    MP_FROST_RegisterCommandCallbackData(FROST_DebugParamType param_type, const void* param);

    MSGPACK_DEFINE(m_param_type,
                   m_param_null,
                   m_int,
                   m_string_sub_param_null,
                   m_string);

    FROST_DebugParamType m_param_type;
    bool m_param_null;
    int m_int;
    bool m_string_sub_param_null;
    std::string m_string;
};

#endif
