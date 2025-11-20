/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementations and helpers for Nebula IPC Types for nebula_pvr_recording_scheduler.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaPvrRecordingSchedulerIpcTypes.hpp"
#include <sstream>

namespace
{

std::string
toString(NEBULA_PvrSchedulerRecordingState state)
{
    switch(state)
    {
    case NEBULA_PvrStateRecordingScheduled:
        return "Scheduled";
    case NEBULA_PvrStateRecordingTriggered:
        return "Triggered";
    case NEBULA_PvrStateRecordingFailed:
        return "Failed";
    case NEBULA_PvrStateRecordingStarted:
        return "Started";
    case NEBULA_PvrStateRecordingCompleted:
        return "Completed";
    case NEBULA_PvrStateRecordingPartiallyCompleted:
        return "PartiallyCompleted";
    case NEBULA_PvrStateRecordingDeleted:
        return "Deleted";
    default:
        return "Unknown";
    }
}

}

std::string
toString(const NEBULA_PvrHandle* recording)
{
    std::ostringstream os;
    
    if (recording == nullptr)
    {
        os << "nullptr";
    }
    else
    {
        os << recording->reminder_id
           << ':'
           << recording->recording_id
           << ':'
           << toString(recording->recording_state);
    }
    return os.str();
}

NEBULA_PvrHandle*
MP_NEBULA_PvrHandle::createNebulaHandle() const
{
    NEBULA_PvrHandle* handle = nullptr;
    if (valid)
    {
        handle = static_cast<NEBULA_PvrHandle*>(malloc(sizeof(NEBULA_PvrHandle)));
        if (handle)
        {
            *handle = *this;
        }
    }
    return handle;
}

MP_NEBULA_RecordingMetadata::MP_NEBULA_RecordingMetadata(
                                const NEBULA_RecordingMetadata* recording_metadata)
{
    if (recording_metadata)
    {
        if (recording_metadata->name)
        {
            name = recording_metadata->name;
        }
        if (recording_metadata->description)
        {
            description = recording_metadata->description;
        }
    }
}