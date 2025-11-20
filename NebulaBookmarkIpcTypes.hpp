/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_bookmark.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULABOOKMARKIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULABOOKMARKIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_bookmark.h"

/*
 * Define packing of data types used in IPC calls.
 */
struct MP_NEBULA_Bookmark : public NEBULA_Bookmark
{
    bool valid;

    MP_NEBULA_Bookmark(const NEBULA_Bookmark* other = nullptr)
        : NEBULA_Bookmark(other ? *other : NEBULA_Bookmark({})),
          valid(other)
    {
        name[BOOKMARK_NAME_LEN-1] = 0;
    }

    operator NEBULA_Bookmark*()
    {
        return valid ? this : nullptr;
    }

    MSGPACK_DEFINE(valid,
                   name,
                   time_in_secs);

    MEMBER_CHECKED_NEBULA_TYPE(NEBULA_Bookmark, "", time_in_secs);
};

#endif