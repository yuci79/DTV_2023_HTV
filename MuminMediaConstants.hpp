/**
 * Copyright 2024 Vestel.  All rights reserved.
 *
 * @file MuminMediaConstants.hpp
 * @brief Shared constants for Mumin Media Backend
 */

#ifndef MUMIN_MEDIA_CONSTANTS_HPP
#define MUMIN_MEDIA_CONSTANTS_HPP

#include "nebula/adaptation_layer/nebula_av_control.h"  // For NEBULA_MediaPlayerStatus

namespace MuminConstants {

// Video dimensions
const int browser_video_default_width = 1280;
const int browser_video_default_height = 720;
const float sd_video_width = 720.0f;
const float sd_video_height = 576.0f;

// Media settings
const int default_max_frame_rate = 60;
const int default_max_bit_rate = 10000000;
const int default_audio_channel_num = 2;

const double normal_playback_rate = 1000;
const double normal_playback_volume = 1.0;

const int max_supported_trickmode_count = 16;
const int invalid_trickmode = 0xFFFFFFFF;

// Move the function here too
inline const char* mediaPlayerStateName(const NEBULA_MediaPlayerStatus status)
{
    const char* result = nullptr;

    switch(status)
    {
        case NEBULA_Stopped:   result = "Stopped";     break;
        case NEBULA_Playing:   result = "Playing";     break;
        case NEBULA_Paused:    result = "Paused";      break;
        case NEBULA_Connecting:result = "Connecting";  break;
        case NEBULA_Buffering: result = "Buffering";   break;
        case NEBULA_Finished:  result = "Finished";    break;
        case NEBULA_Error:     result = "Error";       break;
        case NEBULA_Started:   result = "Started";     break;
        default:               result = "<undefined>"; break;
    }

    return result;
}

} // namespace MuminConstants

#endif // MUMIN_MEDIA_CONSTANTS_HPP
