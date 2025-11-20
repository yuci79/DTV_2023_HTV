/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_components.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOMPONENTIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULACOMPONENTIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_components.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

#include <string>
#include <vector>
/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NebulaComponents_getComponents);
DECLARE_IPC_NAME(NebulaComponents_selectComponent);
DECLARE_IPC_NAME(NebulaComponents_deselectComponent);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(OIPF_ComponentType);
MSGPACK_ADD_ENUM(NEBULA_AVSystemFormat);
MSGPACK_ADD_ENUM(NEBULA_AspectRatio);
MSGPACK_ADD_ENUM(NEBULA_Kind);
MSGPACK_ADD_ENUM(HBBTV_EncodingFormat);

/**
 * @brief Class to encapsulate the base class for msgpack packing.
 */
struct MP_NEBULA_AVComponent : public NEBULA_AVComponent
{
    /**
     * @brief Default constructor.
     */
    MP_NEBULA_AVComponent() = default;

    /**
     * @brief Constructor to copy from the base class. Required for use as a parameter in 
     *        rpc::client::call().
     */
    MP_NEBULA_AVComponent(const NEBULA_AVComponent& base)
        : NEBULA_AVComponent (base)
    {
    }

    MSGPACK_DEFINE(type,
                   index,
                   stream_id,
                   is_active,
                   pid,
                   system_format,
                   encoding,
                   encrypted,
                   aspect_ratio,
                   language_code,
                   audio_description,
                   audio_channels,
                   hearing_impaired,
                   component_tag,
                   kind,
                   id,
                   receiver_mixed_ad,
                   label);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_AVComponent,
                   type,
                   index,
                   stream_id,
                   is_active,
                   pid,
                   system_format,
                   encoding,
                   encrypted,
                   aspect_ratio,
                   "",
                   audio_description,
                   audio_channels,
                   hearing_impaired,
                   component_tag,
                   kind,
                   "",
                   receiver_mixed_ad,
                   "");
};

#endif
