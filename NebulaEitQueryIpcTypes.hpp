/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_eit_query.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAEITQUERYIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAEITQUERYIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_eit_query.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */

DECLARE_IPC_NAME(NEBULA_GetNumProgrammes);
DECLARE_IPC_NAME(NEBULA_GetProgramInfo);
DECLARE_IPC_NAME(NEBULA_GetProgramIDByIndex);
DECLARE_IPC_NAME(NEBULA_GetDescriptors);
DECLARE_IPC_NAME(NEBULA_ProgrammeIs3D);
DECLARE_IPC_NAME(NEBULA_GetGuideDaysAvailable);
DECLARE_IPC_NAME(NEBULA_GetProgrammeCRIDs);

DECLARE_IPC_NAME(NEBULA_RegisterToProgramChangeEvent);
DECLARE_IPC_NAME(NEBULA_DeregisterFromProgramChangeEvent);
DECLARE_IPC_NAME(NEBULA_OnProgramChange);

/*
 * Define packing of data types used in IPC calls.
 */

MSGPACK_ADD_ENUM(NEBULA_ProgramAudioType);
MSGPACK_ADD_ENUM(NEBULA_NetworkType);

struct MP_NEBULA_Program : public NEBULA_Program
{
    MP_NEBULA_Program() = default;

    MP_NEBULA_Program(const NEBULA_Program& other)
        : NEBULA_Program(other)
    {
    }

    MSGPACK_DEFINE(name,
                   long_name,
                   description,
                   long_description,
                   start_time,
                   duration_in_seconds,
                   channel_id,
                   program_index,
                   program_handle,
                   parental_rating_id,
                   episode,
                   network_type,
                   is_crid_actual,
                   crid_url,
                   dvb_url,
                   genre,
                   is_hd,
                   has_subtitles,
                   has_audio_description,
                   blocked,
                   locked,
                   has_sign_language,
                   count_of_audio_languages,
                   audio_languages,
                   count_of_subtitle_languages,
                   subtitle_languages,
                   audio_type,
                   parental_rating.name,
                   parental_rating.scheme,
                   parental_rating.rating_type,
                   parental_rating.value,
                   parental_rating.labels,
                   parental_rating.region);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_Program,
                   "",
                   "",
                   "",
                   "",
                   start_time,
                   duration_in_seconds,
                   channel_id,
                   program_index,
                   program_handle,
                   parental_rating_id,
                   episode,
                   network_type,
                   is_crid_actual,
                   "",
                   "",
                   "",
                   is_hd,
                   has_subtitles,
                   has_audio_description,
                   blocked,
                   locked,
                   has_sign_language,
                   count_of_audio_languages,
                   {""},
                   count_of_subtitle_languages,
                   {""},
                   audio_type,
                   "",
                   "",
                   parental_rating.rating_type,
                   parental_rating.value,
                   parental_rating.labels,
                   "");
};

MSGPACK_ADD_ENUM(NEBULA_CridType);

#endif
