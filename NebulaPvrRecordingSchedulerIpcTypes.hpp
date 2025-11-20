/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations for IPC function names and additional Nebula IPC Types for
 *        nebula_pvr_recording_scheduler.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRRECORDINGSCHEDULERIPCTYPES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAPVRRECORDINGSCHEDULERIPCTYPES_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_pvr_recording_scheduler.h"
#include "nebula/adaptation_layer/ipc/MsgPackHelper.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcTypes.hpp"

/*
 * Define IPC function call names.
 */
DECLARE_IPC_NAME(NEBULA_PvrSchedulerRecord);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerRecordAt);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecording);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecordingByTypeAndIndex);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecordingsCountByType);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetScheduledRecording);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerRemove);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerStop);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetStartPadding);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetStartPadding);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerUpdateScheduledRecording);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerUpdateRecordingDuration);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetEndPadding);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetEndPadding);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRepeatDays);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetName);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetName);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetDescription);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetDescription);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetCustomId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetCustomId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetCustomMetadata);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetCustomMetadata);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetState);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetError);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetScheduleId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetProgrammeId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetProgrammeIdType);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecordingId);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetDoNotDelete);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetDoNotDelete);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetSaveDays);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetSaveDays);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetSaveEpisodes);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerSetSaveEpisodes);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetBlocked);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetLocked);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetShowType);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetSubtitlesAvailable);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetSubtitleLanguages);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerIsHD);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerIsMultilingual);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetAudioLanguages);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetGenres);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetBookmarks);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerAddBookmark);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerRemoveBookmark);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetAudioType);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetStartTime);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetDuration);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecordingStartTime);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetRecordingDuration);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerGetChannelID);

DECLARE_IPC_NAME(NEBULA_PvrSchedulerPVREventCallback);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerScheduledRecordingEventCallback);

DECLARE_IPC_NAME(NEBULA_PvrSchedulerRegisterPVREventCallback);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerDeregisterPVREventCallback);
DECLARE_IPC_NAME(NEBULA_PvrSchedulerRegisterScheduledRecordingEventCallback);

/*
 * Define packing of data types used in IPC calls.
 */
MSGPACK_ADD_ENUM(NEBULA_PvrCollectionType);
MSGPACK_ADD_ENUM(NEBULA_PvrSchedulerPvrEvent)
MSGPACK_ADD_ENUM(NEBULA_PvrSchedulerRecordingError);
MSGPACK_ADD_ENUM(NEBULA_PvrSchedulerRecordingProgrammeIDType);
MSGPACK_ADD_ENUM(NEBULA_PvrSchedulerRecordingShowType);
MSGPACK_ADD_ENUM(NEBULA_PvrSchedulerRecordingState);

struct MP_NEBULA_PvrHandle : public NEBULA_PvrHandle
{
    bool valid;

    MP_NEBULA_PvrHandle(const NEBULA_PvrHandle* other = nullptr)
        : NEBULA_PvrHandle(other ? *other : NEBULA_PvrHandle({})),
          valid(other)
    {
        recording_source_id[RECORDING_SOURCE_ID_LEN-1] = 0;
    }

    operator NEBULA_PvrHandle*()
    {
        return valid ? this : nullptr;
    }

    /**
     * @brief Create a NEBULA_PvrHandle from a MP_NEBULA_PvrHandle.
     * 
     * @return malloc'd NEBULA_PvrHandle if MP_NEBULA_PvrHandle valid otherwise nullptr.
     */
    NEBULA_PvrHandle*
    createNebulaHandle() const;

    MSGPACK_DEFINE(valid,
                   reminder_id,
                   recording_id,
                   recording_state,
                   recording_source_id);

    MEMBER_CHECKED_NEBULA_TYPE(NEBULA_PvrHandle,
                   reminder_id,
                   recording_id,
                   recording_state,
                   "");
};

std::string
toString(const NEBULA_PvrHandle* recording);

struct MP_NEBULA_RecordingMetadata
{
    std::string name;
    std::string description;

    MP_NEBULA_RecordingMetadata() = default;

    MP_NEBULA_RecordingMetadata(const NEBULA_RecordingMetadata* recording_metadata);

    MSGPACK_DEFINE(name,
                   description);

    MEMBER_CHECKED_NEBULA_TYPE(
                   NEBULA_RecordingMetadata,
                   nullptr,
                   nullptr);
};

#endif
