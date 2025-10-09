/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMuminMediaBackend
 */
#include "config/vewd_integration/discretix_release_dir.h"
#include "config/novatek_platform.h"

#include "OperaMuminMediaBackend.hpp"

#include "uva_client.h"
#include "uva_event.h"
#include "config/include_ip_tuner.h" //[d]:added to support IP tuner
#include "config/enable_vod_over_ip_tuner.h"
#include "OperaMuminAVComponentHelper.hpp"
#include "OperaMuminObjectFactory.hpp"
#include "OperaMseStreamData.hpp"
#include "OperaMuminHelper.hpp"

#include "vewd_common/bpi/bpi_graphics.hpp"
#include "vewd_common/bpi/bpi_window_manager.hpp"
#include "utilities_public/MseStreamData.hpp"
#include "utilities_public/TunerStreamData.hpp"
#include "utilities_public/MediaPlayerStreamData.hpp"
#include "utilities_public/StreamDataFactory.hpp"
#include "frost/frost_time.h"

#include "nebula/adaptation_layer/nebula_system_information_provider.h"
#include "nebula/adaptation_layer/nebula_picture_settings.h"
#include "nebula/adaptation_layer/nebula_av_control.h"
#include "nebula_src/adaptation_layer/NebulaMediaPlayer.hpp"
#include "nebula_src/adaptation_layer/NebulaMediaPlayerFactory.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"

#include "profile_helper/ProfileHelper.h"
#include "utilities_debug/cabot_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>

#include "utilities_debug/trace.h"

#include "eclipse/core/DrmTypes.hpp"

TRACE_IMPLEMENT("mumin/media_backend");

//#define WORKSHOP_LOGS
#ifdef WORKSHOP_LOGS
FILE * pFile = NULL;
#endif
#define WAITED_MAX 2000

namespace
{
const float sd_video_width = 720.0;
const float sd_video_height = 576.0;

const int browser_video_default_width = 1280;
const int browser_video_default_height = 720;

const int default_max_frame_rate = 60;
const int default_max_bit_rate = 10000000;
const int default_audio_channel_num = 2;

const double normal_playback_rate = 1000;
const double normal_playback_volume = 1.0;

const int max_supported_trickmode_count = 16;
const int invalid_trickmode = 0xFFFFFFFF;

char const* mediaPlayerStateName(const NEBULA_MediaPlayerStatus status)
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
}   // namespace

namespace mumin
{ 
void WriteDataCommand::execute()
{
    m_media_backend.processWriteDataEvent(m_need_data_event, m_bytes_written, m_write_status);
}

void NeedDataCommand::execute()
{
    m_media_backend.needDataInternal(m_source_id, m_req_len, m_buffer);
}

void SendMetaDataCommand::execute()
{
    m_media_backend.sendMetadata();
}

void CancelDataRequestsCommand::execute()
{
    m_media_backend.cancelRequestsForSeekInternal(m_position_msecs);
}

void CancelDataRequestsCompleteCommand::execute()
{
    m_media_backend.cancelDataRequestsComplete();
}
// placed in cpp to reduce header file inclusion
class ExternalComponentsHelperCommand : public AnyCommand
{
public:
    ExternalComponentsHelperCommand(OperaMuminAVComponentHelper* component_helper,
                                    bool register_components,
                                    const std::vector<AVComponent*>& components)
        : m_component_helper(component_helper)
        , m_register_components(register_components)
        , m_components(components)
    {
    }
    virtual void execute();

private:
    OperaMuminAVComponentHelper* m_component_helper;
    bool m_register_components;
    const std::vector<AVComponent*> m_components;
};
void ExternalComponentsHelperCommand::execute()
{
    if (m_register_components)
    {
        (void)m_component_helper->registerExternalComponents(m_components);
    }
    else
    {
        (void)m_component_helper->unregisterExternalComponents(m_components);
    }
}

DecrypterDescription::DecrypterDescription(UVADRMBackend::UVADRMDecrypter* decrypter,
                    const unsigned char* kid, size_t kid_size)
                    : m_decrypter(decrypter)
                    , m_kid(NULL)
                    , m_kid_size(0)
{
    m_kid = new unsigned char[kid_size];
    if (m_kid)
    {
        m_kid_size = kid_size;
        memcpy(m_kid, kid, kid_size);
    }
}

DecrypterDescription::~DecrypterDescription()
{
    delete [] m_kid;
    m_kid = NULL;
    m_kid_size = 0;
}
}

int OperaMuminMediaBackend::m_backend_id_counter = 0;
OperaMuminMediaBackend::OperaMuminMediaBackend(
                               UVABackendType backend_type,
                               UVAClient* client,
                               OperaMuminObjectFactory& object_factory,
                               CommandThread& queue)
    : UVAMediaBackendBase(backend_type)
    , m_client(client)
    , m_av_object(0)
    , m_component_helper(0)
    , m_loop_count(0)
    , m_current_loop_count(0)
    , m_playback_url("")
    , m_total_playback_duration_in_secs(0)
    , m_stop_request_in_nebula_connecting_state(false)
    , m_setposition_request(false)
    , m_setposition_time(0.0)
    , m_file_details_parsed(false)
    , m_streaming_type(mse)
    , m_stream_fp(0)
    , m_stream_offset(0)
    , m_playback_rate(normal_playback_rate)
    , m_play_requested(false)
    , m_load_requested(false)
    , m_drm_backend(NULL)
    , m_playready_drm_backend(NULL)
    , m_object_factory(object_factory)
    , m_playback_volume(normal_playback_volume)
    , m_seek_ok(false)
    , m_previous_play_state(NEBULA_Stopped)
    , m_res_video_width(browser_video_default_width)
    , m_res_video_height(browser_video_default_height)
    , m_backend_video_res_width(browser_video_default_width)
    , m_backend_video_res_height(browser_video_default_height)
    , m_init_ok(false)
    , m_drm_type(drm_type_none)
    , m_suspended(false)
    , m_pixel_aspect_ratio(1.0)
    , m_sent_buffer_underrun(false)
    , m_bbc_video(false)
    , m_fvp_video(false)
    , m_itv_video(false)
    , m_magine_tv_video(false)
    , m_high_bitrate_video(false)
    , m_youtube_video(false)
    , m_playback_started(false)
    , m_ligada_is_active(false)
    , m_ligada_is_ad_insertion_active(false)
    , m_queue(queue)
    , m_async_cancel_in_progress(false)
    , m_position(0.0)
    , m_is_rtsp(false)
    , m_is_udp(false)
    , m_seek_is_active(false)
    , m_youtube_ad(false)
    , m_last_requested_video_window(0,0,0,0)
    , m_last_exact_aspect_ratio(0)
    , m_browser_max_video_width(1920)
    , m_browser_max_video_height(1080)
    , m_backend_id(m_backend_id_counter++)
{
    TRACE_INFO(("Creating backend: type: %d, client: %p, id: %d\n", backend_type, client, m_backend_id));
    m_last_video_window.x_position = 0;
    m_last_video_window.y_position = 0;
    m_last_video_window.width = sd_video_width;
    m_last_video_window.height = sd_video_height;

    frost_int volume_level = NEBULA_GetVolumeLevel(frost_true);
    m_playback_volume = volume_level / 100.0;

    m_init_ok = m_source_mutex.initialise();
    m_init_ok = (m_init_ok && m_seek_mutex.initialise());
}

OperaMuminMediaBackend::~OperaMuminMediaBackend()
{
    TRACE_INFO(("(%d): Destructing backend, type '%d' (mse=%d)\n", m_backend_id, m_streaming_type, StreamingType::mse));

    if (m_component_helper)
    {
        delete m_component_helper;
        m_component_helper = 0;
    }

    if (m_av_object)
    {
        m_object_factory.discardOipfVideoOnDemandObject(m_av_object);
        m_media_data_source.unregisterDataProvider(this);
        m_av_object = 0;
    }

    if (m_stream_fp)
    {
        fclose(m_stream_fp);
        m_stream_fp = 0;
    }

    removeDecrypters();
#ifdef WORKSHOP_LOGS
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
#endif
#ifndef NOVATEK_PLATFORM
#ifdef DISCRETIX_RELEASE_DIR
    if (m_playready_drm_backend)
    {
        TRACE_INFO(("Delete playready drm backend \n"));
        delete m_playready_drm_backend;
        m_playready_drm_backend = NULL;
    }
#endif
#endif
#ifndef NOVATEK_PLATFORM
    if (!m_youtube_ad)
    {
        NebulaMediaPlayer::changeYoutubeThreadPriorities(false);
    }
#endif
}

UVA_STATUS
OperaMuminMediaBackend::initBackend(const char *origin,
            UVABackend::SourceDescription **sources,
            bool automatic_key_system_selection)

{
    TRACE_INFO(("(%d): Size of m_source_array: %d automatic_key_system_selection: %d\n",
            m_backend_id, m_source_list.size(), automatic_key_system_selection));

    if (!m_init_ok)
    {
        return UVA_NOT_INITIALIZED;
    }

    BPI_GraphicsGetNativeSize(&m_browser_max_video_width, &m_browser_max_video_height);

    setStreamingType(sources);

    UVA_STATUS result = UVA_ERROR;
    if (!m_av_object)
    {
        if (strstr(origin,"ad.serve-sys")!=0 || (strstr(origin,"atv.com.tr")!=0 && strstr(origin,"ihbbtv.atv.com.tr")==0))
        {
            ////block advertisement videos
            return UVA_ERROR;
        }

        if( strstr(origin,"hbbtv.test")!=0 ||
            strstr(origin,"hbbtv1.test")!=0 ||
            strstr(origin,"hbbtv2.test")!=0)
        {
            m_ligada_is_active = true;

            if (strstr(origin,"ADINS") !=0)
            {
                m_ligada_is_ad_insertion_active = true;
            }
        }    
        if (createVideoOnDemandObject(av_control_object_type_net_stream, (char*)origin))
        {
            m_media_data_source.registerDataProvider(this);
            m_file_details_parsed = false;
            m_play_requested = false;
            result = UVA_OK;
            m_av_object->setFullScreen(true);
            for (int i = 0; sources[i]; ++i)
            {
                result = addSource(sources[i]);
            }

            if (result != UVA_ERROR)
            {
                //m_av_object->setFullScreen(true);
            }
        }
        else
        {
            m_av_object = 0;
        }
    }
#ifdef WORKSHOP_LOGS
    if (pFile == NULL)
    {
        pFile = fopen("/mnt/hd0a/OperaWSLog", "w");
    }
#endif

    return result;
}

void
OperaMuminMediaBackend::refreshComponentHelper()
{
    if (m_component_helper)
    {
        delete m_component_helper;
        m_component_helper = 0;
    }

    m_component_helper = new OperaMuminAVComponentHelper(*m_av_object, m_client, m_streaming_type == mse, &m_queue);
}

bool
OperaMuminMediaBackend::createVideoOnDemandObject(AVControlObjectType type, char* origin)
{
    TRACE_INFO(("%s(), LINE:(%d)  AVControlObjectType= %d  \n", __FUNCTION__, __LINE__,  type));
    bool success = m_object_factory.createOipfVideoOnDemandObject(&m_av_object, this, m_queue, type,
            m_streaming_type, m_media_data_source, m_media_data_sink, m_is_rtsp, m_is_udp, origin);
    if (success)
    {
        if (m_bbc_video)
        {
            ((AVControlObject*)m_av_object)->setBbcStreamingOnForMstar();
            ((AVControlObject*)m_av_object)->setFvpStreamingOn();
            //added above line in purpose of decreasing the buffering limitation for FVP videos
        }
        if (m_ligada_is_active)
        {
            ((AVControlObject*)m_av_object)->setLigadaStreamingOn(m_ligada_is_ad_insertion_active);
        }
        if (m_fvp_video)
        {
            ((AVControlObject*)m_av_object)->setFvpStreamingOn();
        }
        if (m_magine_tv_video)
        {
            ((AVControlObject*)m_av_object)->setMagineTvStreamingOn();
        }
        if (m_high_bitrate_video)
        {
            ((AVControlObject*)m_av_object)->setHighBitrateStreamingOn();
        }
        refreshComponentHelper();
        success = true;
    }
    return success;
}

UVA_STATUS
OperaMuminMediaBackend::setVideoWindowAndClip(const Rect& window_rect, const Rect& clip_rect)
{
    int x = window_rect.x;
    int y = window_rect.y;
    int h = window_rect.h;
    int w = window_rect.w;
    double aspect_ratio = (double)w / h;
    TRACE_INFO(("(%d): Shall scale video at position : (x = %d , y = %d) with dimensions (w = %d , h= %d) and aspect ratio [ %f ]\n",
            m_backend_id, x, y, w, h, aspect_ratio));
    TRACE_INFO(("(%d): Clipping window : (x = %d , y = %d) with dimensions (w = %d , h= %d) and\n",
            m_backend_id, clip_rect.x,clip_rect.y,clip_rect.w,clip_rect.h));

    if((x + w <= 0) || (y + h <= 0))
    {
        TRACE_INFO(("(%d): setVideoWindow(): IGNORING REQUEST\n", m_backend_id));
        return UVA_ERROR;
    }

    NEBULA_DisplayWindow nebula_window;

    if (m_streaming_type == mse ) // calculate dimensions to preserve aspect ratio.
    {
        nebula_window.x_position = static_cast<float>(x * (double) sd_video_width / m_browser_max_video_width);
        nebula_window.y_position = static_cast<float>(y * (double) sd_video_height / m_browser_max_video_height);

        if (m_youtube_video)
        {
            if ((x < 5) && (w == (int)m_browser_max_video_width))
            {
                nebula_window.width = sd_video_width;
                nebula_window.height = sd_video_height - 2 * nebula_window.y_position;
            }
            else if ((y < 5) && (h == (int)m_browser_max_video_height))
            {
                nebula_window.height = sd_video_height;
                nebula_window.width = sd_video_width - 2 * nebula_window.x_position;
            }
            else
            {
                nebula_window.width = static_cast<float>(w * (double) sd_video_width / m_browser_max_video_width);
                nebula_window.height = static_cast<float>(h * (double) sd_video_height / m_browser_max_video_height);
            }
        }
        else
        {
            nebula_window.width      = static_cast<float>(w * (double) sd_video_width / m_browser_max_video_width);
            nebula_window.height     = static_cast<float>(h * (double) sd_video_height / m_browser_max_video_height);
        }
        TRACE_INFO(("(%d): Manually calculate dimensions, NWindow : ( x=%f, y=%f, w=%f, h=%f )\n",
                m_backend_id, nebula_window.x_position, nebula_window.y_position, nebula_window.width, nebula_window.height));
    }
    else
    {
#if defined(NOVATEK_PLATFORM) || defined(_LINUX_PC_PLATFORM)
        m_last_requested_video_window.x = x;
        m_last_requested_video_window.y = y;
        m_last_requested_video_window.w = w;
        m_last_requested_video_window.h = h;
        adjustVideoAspectRatio(x, y, w, h);
#endif

        nebula_window.x_position = static_cast<float>(x * sd_video_width / (double) m_browser_max_video_width);
        nebula_window.y_position = static_cast<float>(y * sd_video_height / (double) m_browser_max_video_height);
        nebula_window.width      = static_cast<float>(w * sd_video_width / (double) m_browser_max_video_width);
        nebula_window.height     = static_cast<float>(h * sd_video_height / (double) m_browser_max_video_height);
    }

    bool ok = applyVideoPosition(nebula_window);
    if (!ok)
    {
        TRACE_ERROR(("setVideoWindow(): (%d, %d, %d, %d) FAILED\n", x, y, w, h));
    }
    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS 
OperaMuminMediaBackend::load()
{
    bool ok = false;
    
    if (m_source_list.size() == 0)
    {
        TRACE_INFO(("No source available yet.\n"));
        return UVA_NOT_INITIALIZED;
    }

    m_suspended = false;
    m_load_requested = true;

    NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
    TRACE_INFO(("(%d): Current status %s\n", m_backend_id, ::mediaPlayerStateName(playback_status)));
    
    if (playback_status == NEBULA_Stopped)
    {
        ok = prepareForPlay(false);
    }
    else if (playback_status == NEBULA_Connecting)
    {
        ok = true;
        m_play_requested = false;
        if (m_streaming_type == backend)
        {
            sendEvent(new BufferingIdleEvent);
        }
    }
    
    return (ok) ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::play()
{
    TRACE_INFO(("(%d): play()\n", m_backend_id));
    if (m_source_list.size() == 0)
    {
        TRACE_ERROR(("(%d): Cannot play, no source available\n", m_backend_id));
        return UVA_NOT_INITIALIZED;
    }

    m_suspended = false;
    bool playing = false;

    if (m_av_object)
    {
        NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();

        bool delayed_playback = false;
        if (playback_status == NEBULA_Stopped || playback_status == NEBULA_Finished)
        {
            if (m_streaming_type == backend)
            {
                playing = prepareForPlay(true);
                delayed_playback = true;
            }
        }

        if (!delayed_playback)
        {
            NEBULA_DisplayWindow current_window = m_av_object->getVideoOutputWindow();
            if (current_window != m_last_video_window)
            {
                TRACE_INFO(("(%d): Output window changed: current: '%s', expected: '%s'\n",
                        m_backend_id,
                        toStringNebulaWindow(current_window).c_str(),
                        toStringNebulaWindow(m_last_video_window).c_str()));

                m_av_object->setVideoOutputWindow(&m_last_video_window, true);
            }

            playing = m_av_object->play();
            if (playing)
            {
                m_playback_rate = normal_playback_rate;
                m_play_requested = false;
            }
        }

        if (isLoopEnabled())
        {
            m_current_loop_count++;
        }
    }

    TRACE_INFO(("(%d): play() result: %d\n", m_backend_id, playing));
    return playing ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::stop()
{
    TRACE_INFO(("(%d): stop\n", m_backend_id));
    if (m_source_list.size() == 0)
    {
        TRACE_WARN(("No source available - return UVA_NOT_INITIALIZED!.\n"));
        return UVA_NOT_INITIALIZED;
    }

    if (m_av_object)
    {
        NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
        /**
        * For cases when media is not playing and stop is requested.
        * Topfun sends OIPF stop in connecting state
        * Stop request is stored and applied when nebula playing state is received
        */

        if (playback_status == NEBULA_Connecting)
        {
            TRACE_INFO(("(%d): Set m_stop_request_in_nebula_connecting_state = true in NEBULA_connecting state\n", m_backend_id));
            m_stop_request_in_nebula_connecting_state = true;
        }
        m_loop_count = 0;
        m_av_object->stop();
    }
    m_play_requested = false;
    TRACE_INFO(("stop() result: OK\n"));
    return UVA_OK;
}

UVA_STATUS
OperaMuminMediaBackend::pause()
{
    TRACE_INFO(("(%d): pause\n", m_backend_id));
    if (m_source_list.size() == 0)
    {
        TRACE_INFO(("No source available yet.\n"));
        return UVA_OK;
    }

    m_suspended = false;
    double speed_is_0 = 0.0;
    bool ok = false;
    bool set_speed = false;

    if (m_av_object)
    {
        NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
        TRACE_INFO(("(%d): Current status %s\n", m_backend_id, ::mediaPlayerStateName(playback_status)));
        
        if (m_load_requested &&
            (playback_status == NEBULA_Stopped) &&
            (playback_status == NEBULA_Connecting))
        {
            TRACE_INFO(("status is already handled in OperaMuminMediaBackend::load()...\n"));
            return UVA_OK;
        }
        
        if (playback_status == NEBULA_Stopped)
        {
            ok = prepareForPlay(false);
        }
        else if (playback_status == NEBULA_Connecting)
        {
            ok = true;
            m_play_requested = false;
            if (m_streaming_type == backend)
            {
                sendEvent(new BufferingIdleEvent);
            }
        }
        else if (playback_status == NEBULA_Finished)
        {
            m_play_requested = false;
            set_speed = false;
            if (m_streaming_type == backend)
            {
                sendEvent(new BufferingIdleEvent);
            }
            ok = true;
        }
        else
        {
            set_speed = true;
        }

        if (set_speed)
        {
            ok = m_av_object->setSpeed(speed_is_0);
            m_playback_rate = speed_is_0;
        }

        m_load_requested = false;
    }

    TRACE_INFO(("pause(): %d\n", ok));
    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::suspend()
{
    TRACE_INFO(("(%d): Suspending backend\n", m_backend_id));
    if (!m_av_object)
    {
        return UVA_NOT_INITIALIZED;
    }
    m_av_object->suspend(true);

    UVA_STATUS ret_val = UVA_OK;
    NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();

    if (playback_status != NEBULA_Connecting && 
        playback_status != NEBULA_Stopped &&
        playback_status != NEBULA_Error)
    {
        ret_val = stop();
    }
    
    if(ret_val == UVA_OK)
    {
        m_suspended = true;
    }
    return ret_val;
}

UVA_STATUS
OperaMuminMediaBackend::setPlaybackRate(double rate)
{
    if (m_source_list.size() == 0)
    {
        TRACE_WARN(("[%s @ %d] No source available - return UVA_NOT_SUPPORTED!.\n", __FUNCTION__, __LINE__));
        return UVA_NOT_SUPPORTED;
    }

    bool result = true;
    bool rate_supported = true;
    double speed_in_per_thousandths = rate * 1000;
    TRACE_INFO(("%s(): speed:(%f)\n", __FUNCTION__, speed_in_per_thousandths));

    if (m_streaming_type == mse || m_streaming_type == adaptive)
    {
        if (rate != 0.0 && rate != 1.0)
        {
            // We do not support many trick modes for mse/adaptive streaming cases
            //rate_supported = false;
            //result = false;
            speed_in_per_thousandths = m_playback_rate;
        }
    }

    if (m_av_object && rate_supported)
    {
        NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
        if ( /* m_file_details_parsed && */
            m_playback_rate != speed_in_per_thousandths &&
            (playback_status == NEBULA_Playing || playback_status == NEBULA_Paused || playback_status == NEBULA_Buffering)
            && !m_seek_is_active)
        {
            result = m_av_object && m_av_object->setSpeed(static_cast<int>(speed_in_per_thousandths));
        }
    }

    if (result)
    {
        m_playback_rate = speed_in_per_thousandths;
    }

    UVA_STATUS retval = UVA_OK;
    if (!rate_supported)
    {
        retval = UVA_NOT_SUPPORTED;
    }
    else if (!result)
    {
        retval = UVA_ERROR;
    }

    TRACE_INFO(("%s(): returning %d\n", __FUNCTION__, retval));
    return retval;
}

UVA_STATUS
OperaMuminMediaBackend::setVolume(double volume)
{
    if (volume < 0.0 || volume > 1.0)
    {
        return UVA_ERROR;
    }

    int volume_perc = volume * 100;

    if(m_av_object)
    {
        NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
        if(playback_status == NEBULA_Stopped)
        {
            return UVA_OK;
        }
    }

    bool volume_set = setVolumeInternal(volume_perc);
    
    if (volume_set)
    {
        m_playback_volume = volume;
    }

    return volume_set ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::getDuration(double &duration)
{
    //TRACE_VERBOSE(("(%d): duration\n", m_backend_id));
    if (m_source_list.size() == 0)
    {
        TRACE_WARN(("[%s @ %d] No source available - return UVA_NOT_INITIALIZED!.\n", __FUNCTION__, __LINE__));
        return UVA_NOT_INITIALIZED;
    }

    if (m_total_playback_duration_in_secs == 0 && m_streaming_type == mse)
    {
        duration = INFINITY;
    }
    else
    {
        duration = m_total_playback_duration_in_secs;
    }

    //TRACE_VERBOSE(("(%d): Total playback duration in secs: (%.3f)\n", m_backend_id, m_total_playback_duration_in_secs));
    return UVA_OK;
}

UVA_STATUS
OperaMuminMediaBackend::getPosition(double &position)
{
    if (m_suspended)
    {
        if (m_setposition_request)
        {
            position = m_setposition_time;
            m_position = position;
            return UVA_OK;
        }
    }

    if (m_streaming_type == mse)
    {
        int waitedmsec = 0;
        while(!m_file_details_parsed && waitedmsec < WAITED_MAX)
        {
            FROST_Sleep(5);
            waitedmsec += 5;
        }
        if (!m_file_details_parsed)
        {
            TRACE_INFO(("(%d): File details is not parsed after waiting 2s\n", m_backend_id));
        }
    }

    UVA_STATUS result = UVA_ERROR;
    if (m_av_object)
    {
        double current_position_in_secs = 0.0;
        if (m_file_details_parsed)
        {
            std::int64_t pos_msecs = 0;
            if (m_av_object->getCurrentPosition(pos_msecs))
            {
                current_position_in_secs = pos_msecs / 1000.0;
            }
            else
            {
                current_position_in_secs = m_position;
                //TRACE_INFO(("(%d): Returning the last known position(%f)\n", m_backend_id, current_position_in_secs));
            }
        }
        if( m_ligada_is_active == true &&
            m_streaming_type == backend && 
            m_setposition_request)
        {
            NEBULA_MediaPlayerStatus playback_status = m_av_object->getPlaybackStatus();
            if( playback_status == NEBULA_Paused )
            {
                current_position_in_secs = m_setposition_time;
            }
        }

        position = current_position_in_secs;
        m_position = position;

        result = UVA_OK;
    }
    //TRACE_VERBOSE(("(%d): Returning position(%f), result: %s\n", m_backend_id, position, result == UVA_OK ? "OK" : "NOT OK"));
    return result;
}

UVA_STATUS
OperaMuminMediaBackend::setPosition(double position)
{
    FrostMutexLock lock(m_seek_mutex);
    TRACE_INFO(("(%d): Setting position to (%f)\n", m_backend_id, position));
    m_position = position;
    if (m_source_list.size() == 0)
    {
        TRACE_INFO(("Delayed set position\n"));
        m_setposition_time = position;

        if (m_seek_is_active)
        {
            sendEvent(new SeekCompletedExEvent(SeekOutcome::Aborted));
        }

        m_queue.enqueueCommand(new mumin::CancelDataRequestsCommand(*this, position * 1000));
        return UVA_OK;
    }

    if (m_suspended ||
        m_seek_is_active)
    {
        TRACE_INFO(("Delayed set position due to suspend or active seek\n"));
        cancelRequestsForSeekInternal(position * 1000);
        sendEvent(new SeekCompletedExEvent(SeekOutcome::Aborted));
       
        m_setposition_request = true;
        if (m_setposition_time == position)
        {
            return UVA_OK;
        }
        else
        {
            m_setposition_time = position;
            if (m_streaming_type == backend)
            {
                return UVA_OK;    
            }
        }
    }

    bool ok = false;
    if (m_av_object)
    {
        ok = seekActual(position);

        if (!m_playback_started && m_streaming_type != mse)
        {
            sendEvent(new SeekCompletedExEvent(SeekOutcome::Succeeded));
        }
    }
    TRACE_INFO(("(%d): Set position to (%f): returning %d\n", m_backend_id, position, ok));
    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::setVisibility(bool visible)
{
    TRACE_INFO(("setVisibility(), visible = %d\n", visible));

    bool ok = m_av_object && m_av_object->setVisibility(visible);
    return ok ? UVA_OK : UVA_ERROR;
}

bool
OperaMuminMediaBackend::writeDataInternalLocked(uintptr_t source_id,
                                                AnyStreamData* stream_data,
                                                bool end_of_data)
{
    CABOT_ASSERT(stream_data);

    bool write_ok = false;
    bool first_data_after_jump = false;

    mumin::MuminSourceDescription* source_desc = findSourceLocked(source_id);
    CABOT_ASSERT(source_desc);

    if (source_desc)
    {
        source_desc->m_pending_need_data = false;

        first_data_after_jump = source_desc->m_first_data_after_jump;
        if (first_data_after_jump)
        {
            TRACE_INFO(("First data after jump for source(%zu)\n", source_id));
            source_desc->m_first_data_after_jump = false;
        }

        ++source_desc->m_received_packet_count;
    }

    if (m_media_data_sink.sendData(source_id, stream_data, end_of_data, first_data_after_jump))
    {
        write_ok = true;
    }

    if (!write_ok)
    {
        deleteStreamData(stream_data);
    }

    TRACE_VERBOSE(("Send data to sink for source(%zu), last PTS(%f), success(%d)\n", source_id, source_desc->m_last_received_pts, write_ok));
    return write_ok;
}

bool
OperaMuminMediaBackend::processWriteDataEvent(UVAEvent* need_data_event, size_t bytes_written, UVABackend::WriteDataStatus status)
{
    CABOT_ASSERT(need_data_event);

    if (m_source_list.size() == 0)
    {
        TRACE_ERROR(("[%s @ %d] Not initialized!.\n", __FUNCTION__, __LINE__));
        delete need_data_event;
        return UVA_NOT_INITIALIZED;
    }

    bool eos = false;
    if (status != WRITE_DATA_OK)
    {
        TRACE_INFO(("In %s(): END OF DATA stream or an error: Status(%d)\n", __FUNCTION__, status));
        eos = true;
    }

    bool sent_metadata = false;
    bool success = false;
    if (need_data_event->GetType() == UVAEvent::NeedMSEData)
    {
        NeedMSEDataEvent* mse_ev = static_cast<NeedMSEDataEvent *>(need_data_event);
        UVADecoderBuffer** samples = mse_ev->samples;
        mse_ev->samples = 0;    // Assume ownership of samples

        MseStreamData* mse_stream_data_list = 0;
        MseStreamData* last_in_list = 0;

        FrostMutexLock lock(m_source_mutex);
        mumin::MuminSourceDescription* source_desc = findSourceLocked(mse_ev->source_id);

        if (!source_desc)
        {
            delete need_data_event;
            return UVA_ERROR;
        }

        int i = 0;
        for (i = 0; samples[i] != 0; ++i)
        {
            TRACE_VERBOSE(("Source(%zu), PTS(%.3f), DTS(%.3f)\n", mse_ev->source_id, samples[i]->pts_sec, samples[i]->dts_sec));
            if (source_desc->m_first_data_after_jump && i == 0)
            {
                source_desc->m_last_received_pts = samples[i]->pts_sec + samples[i]->duration_sec;
            }
            else
            {
                if ((samples[i]->pts_sec + samples[i]->duration_sec) > source_desc->m_last_received_pts)
                {
                    source_desc->m_last_received_pts  = samples[i]->pts_sec + samples[i]->duration_sec;
                }
            }

            //Opera can not recognise HEVC codec videos properly, this is a hack to mark HEVC video data correctly.
            if (samples[i]->config.codec == 0 &&
               samples[i]->config.stream_id == 0 &&
               samples[i]->config.details.video.width >= 1920 &&
               samples[i]->config.details.video.height >= 1080)
            {
                samples[i]->config.codec=UVAStreamConfig::H265;
            }

            if (samples[i]->config.av(samples[i]->config.codec) == UVAStreamConfig::VIDEO_STREAM)
            {
                int previous_width = m_res_video_width;
                int previous_height = m_res_video_height;

                if (samples[i]->config.details.video.width != 0 &&
                    samples[i]->config.details.video.height != 0)
                {
                    m_res_video_width = samples[i]->config.details.video.width;
                    m_res_video_height = samples[i]->config.details.video.height;
                }
                if (previous_width != m_res_video_width || previous_height != m_res_video_height)
                {
                    TRACE_INFO(("Video resolution change: width '%u' height '%u'. Max width '%u' and height '%u'\n",
                            m_res_video_width, m_res_video_height,
                            samples[i]->config.details.video.max_width, samples[i]->config.details.video.max_height));
                    m_queue.enqueueCommand(new mumin::SendMetaDataCommand(*this));
                    sent_metadata = true;
                }
                else if (m_previous_play_state == NEBULA_Connecting && m_av_object && !sent_metadata)
                {
                    if (source_desc->m_received_packet_count == 1)
                    {
                        m_queue.enqueueCommand(new mumin::SendMetaDataCommand(*this));
                        sent_metadata = true;
                    }
                }
            }
#ifdef WORKSHOP_LOGS
           double current_position_in_secs = m_av_object->getCurrentPosition() / 1000.0;
           double pts = samples[i]->pts_sec;
           double dts_sec = samples[i]->dts_sec;
           double duration_sec = samples[i]->duration_sec;
           int is_video = 1;
           unsigned int video_height =0;
           unsigned int video_width = 0;

           if  (UVAStreamConfig::av(samples[i]->config.codec) == UVAStreamConfig::AUDIO_STREAM)
           {
               is_video = 0;
           }
           if (pFile)
           {
                if (is_video == 0)
                {
                    fprintf(pFile, "####  In %s(): Source: %u, sample i= %d  current_position_in_secs= %f need_data_event= %p pts= %f dts_sec =%f  duration_sec = %f   is_video %d AUDIO codec %d \n", __FUNCTION__, mse_ev->source_id, i,
                    current_position_in_secs,  need_data_event, pts, dts_sec, duration_sec,
                    is_video, samples[i]->config.codec);
                }
                else
                {
                    video_height = samples[i]->config.details.video.height;
                    video_width =  samples[i]->config.details.video.width;
                    fprintf(pFile, "####  In %s(): Source: %u, sample i= %d  current_position_in_secs= %f need_data_event= %p pts= %f dts_sec =%f  duration_sec = %f   is_video %d  video_height %u video_width %u  VIDEO CODEC %d\n", __FUNCTION__, mse_ev->source_id, i,  current_position_in_secs,  need_data_event, pts, dts_sec, duration_sec,
                    is_video, video_height, video_width,  samples[i]->config.codec);
                }
           }
#endif
            UVADRMBackend::UVADRMDecrypter* decrypter = NULL;
            if (samples[i]->GetKID())
            {
                addDecrypter(samples[i]->GetKID(), samples[i]->GetKIDSize());
                mumin::DecrypterDescription* desc  = NULL;
                desc = findDecrypter(samples[i]->GetKID(), samples[i]->GetKIDSize());
                if (desc)
                {
                    decrypter = desc->m_decrypter;
                }
            }

            bool secure_decrypt = false;
            if (m_drm_type == drm_type_widevine)
            {
                if (UVAStreamConfig::av(samples[i]->config.codec)
                        == UVAStreamConfig::VIDEO_STREAM)
                {
                    secure_decrypt = true;
                }
            }

#ifndef INCLUDE_IP_TUNER
            OperaMseStreamData* mse_stream_data = createOperaMseStreamData(samples[i], m_drm_backend, decrypter, secure_decrypt);
            mse_stream_data->setDrmType(m_drm_type);
            if (samples[i]->config.codec == UVAStreamConfig::VORBIS)
            {
                if (samples[i]->pts_sec == 0)
                {
                    MseStreamData* extra_mse_stream_data = createMseStreamData(samples[i]->config.extra_data,samples[i]->config.extra_data_size,
                        samples[i]->pts_sec,samples[i]->dts_sec,samples[i]->duration_sec);
                    extra_mse_stream_data->streamConfig().setStreamConfigCodecInfo(audio_stream, audio_vorbis);
                    mse_stream_data_list = extra_mse_stream_data;
                    last_in_list = extra_mse_stream_data;
                    last_in_list->setNextSample(mse_stream_data);
                    last_in_list = mse_stream_data;
                }
                else
                {
                    if (i == 0)
                    {
                        mse_stream_data_list = mse_stream_data;
                        last_in_list = mse_stream_data;
                    }
                    else
                    {
                        last_in_list->setNextSample(mse_stream_data);
                        last_in_list = mse_stream_data;
                    }
                }
            }
            else
            {
                if (i == 0)
                {
                    mse_stream_data_list = mse_stream_data;
                    last_in_list = mse_stream_data;
                }
                else
                {
                    last_in_list->setNextSample(mse_stream_data);
                    last_in_list = mse_stream_data;
                }
            }
#endif
        }
#ifdef WORKSHOP_LOGS
        if (pFile)
        {
            fprintf(pFile,"####  In %s(): Source: %u, Sending %d samples to media player    current_position_in_secs %f need_data_event %p \n", __FUNCTION__, mse_ev->source_id, i,
            current_position_in_secs, need_data_event);
        }
#endif
        if (i == 0)
        {
            // 0 samples received, send dummy data to signal end of stream
            MseStreamData* dummy_data = createMseStreamData(0, 0, 0.0f, 0.0f, 0.0f);
            dummy_data->streamConfig().setStreamConfigCodecInfo(other_stream, unknown_codec);
            mse_stream_data_list = dummy_data;
        }

        if (mse_stream_data_list)
        {
            writeDataInternalLocked(mse_ev->source_id, mse_stream_data_list, eos);
        }

        delete [] samples;
    }

    delete need_data_event;

    return success;

}

UVA_STATUS
OperaMuminMediaBackend::writeData(UVAEvent* need_data_event, size_t bytes_written, UVABackend::WriteDataStatus status)
{
    TRACE_VERBOSE(("Writing data bytes/packets(%zu), status(%d)\n", bytes_written, status));

    if (m_async_cancel_in_progress)
    {
        TRACE_WARN(("Ignoring data because async seek is in progress\n"));
    }
    else
    {
        m_queue.enqueueCommand(new mumin::WriteDataCommand(*this, need_data_event, bytes_written, status));
    }
    return UVA_OK;
}

bool
OperaMuminMediaBackend::needData(uintptr_t source_id, size_t req_len, unsigned char* buffer)
{
    mumin::NeedDataCommand* need_data_command = new mumin::NeedDataCommand(*this, source_id, req_len,
            buffer);

    return m_queue.enqueueCommand(need_data_command);
}

void
OperaMuminMediaBackend::eventCompleted(const UVAAsyncEvent* event, bool success)
{
    CABOT_ASSERT(event->GetType() == UVAEvent::CancelDataRequestsAsync);
    TRACE_INFO(("Event %d completed success?%d\n", event->GetType(), success));

    if (success)
    {
        m_queue.enqueueCommand(new mumin::CancelDataRequestsCompleteCommand(*this));
    }
    delete event;
}

void
OperaMuminMediaBackend::cancelDataRequestsComplete()
{
    (void)m_media_data_sink.prepareForDataFromNewPosition();
    FrostMutexLock lock(m_source_mutex);
    const SourceIterator end = m_source_list.end();

    for (SourceIterator ii = m_source_list.begin(); ii != end; ++ii)
    {
        mumin::MuminSourceDescription* source = *ii;
        source->m_first_data_after_jump = true;
    }

    m_async_cancel_in_progress = false;
}

bool
OperaMuminMediaBackend::cancelRequestsForSeek(double new_position_msecs)
{
    return m_queue.enqueueCommand(new mumin::CancelDataRequestsCommand(*this, new_position_msecs));
}

bool
OperaMuminMediaBackend::cancelRequestsForSeekInternal(double new_position_msecs)
{
    m_async_cancel_in_progress = true;
    double seek_pos_secs = new_position_msecs / 1000.0;
    TRACE_INFO(("(%d): Cancel requests to (%f secs)\n", m_backend_id, seek_pos_secs));
    if (!sendEvent(new CancelDataRequestsAsyncEvent(seek_pos_secs)))
    {
        TRACE_ERROR(("Cannot send the cancel data event\n"));
        m_async_cancel_in_progress = false;
        return false;
    }

    return true;
}

bool
OperaMuminMediaBackend::needDataInternal(uintptr_t source_id, size_t req_len, unsigned char* buffer)
{
    FrostMutexLock lock(m_source_mutex);
    if (m_source_list.size() == 0)
    {
        TRACE_ERROR(("[%s @ %d] No source available!.\n", __FUNCTION__, __LINE__));
        return false;
    }

    mumin::MuminSourceDescription* source_desc = findSourceLocked(source_id);

    if (!m_client || !source_desc)
    {
        return false;
    }

    if (m_async_cancel_in_progress)
    {
        TRACE_INFO(("Cancel in progress. Source(%zu) Cannot accept need data now\n", source_id));
        return true;
    }

    bool result = false;
    UVAEvent *event = 0;
    unsigned char* req_buffer = buffer;

    CABOT_ASSERT(source_desc->m_source_type == UVABackend::SourceDescription::MSE_STREAMING);

    if (source_desc->m_source_type == UVABackend::SourceDescription::MSE_STREAMING)
    {
        event = new NeedMSEDataEvent(source_id, req_len);
    }
    else
    {
        SHOULD_NOT_BE_HERE();   // All Opera streaming is MSE now
        mumin::NeedStreamDataEvent::Requester requester = mumin::NeedStreamDataEvent::tuner;
        if (!buffer)
        {
            // buffer will need to be alloc'ed and dealloc'ed manually
            requester = mumin::NeedStreamDataEvent::media_player;
            req_buffer = static_cast<unsigned char*>(malloc(req_len * sizeof(unsigned char)));
        }
        else
        {
            // buffer is allocated inside the decoder
            requester = mumin::NeedStreamDataEvent::tuner;
        }

        event = new mumin::NeedStreamDataEvent(requester,
                                              source_id, m_stream_offset,
                                              req_buffer, req_len);
    }

    source_desc->m_pending_need_data = true;
    result = sendEvent(event);
    if (result != true)
    {
        source_desc->m_pending_need_data = false;
    }

#ifdef WORKSHOP_LOGS
    if (pFile)
    {
        double current_position_in_secs = m_av_object->getCurrentPosition() / 1000.0;
        fprintf(pFile," \n ##### In %s(): Source: %u, request sent?(%d) req_len %d  current_position_in_secs %f   event  %p\n", __FUNCTION__, source_id,  result,   req_len,
        current_position_in_secs, event);
    }
#endif

    TRACE_VERBOSE(("Requested Opera to send data for source(%zu), success(%d)\n", source_id, result));
    return result;
}

UVA_STATUS
OperaMuminMediaBackend::canPlayType(const ContentDescription& desc)
{
    TRACE_VERBOSE(("canPlayType(): type: %s, drm id: %s, width: %s, height: %s bitrate: %s framerate: %s channels: %s cryptoblockformat: %s\n",
            desc.type.c_str(), desc.drmID.c_str(), desc.width.c_str(), desc.height.c_str(), desc.bitrate.c_str(), desc.framerate.c_str(), desc.channels.c_str(), desc.cryptoblockformat.c_str()));
    int width;
    int height;
    if (!NEBULA_SystemInformationProviderGetPanelInformation(width, height))
    {
        width = browser_video_default_width;
        height = browser_video_default_height;
    }
/*
#ifndef NOVATEK_PLATFORM
    // Limit YT resolution to 3K. It will be reverted once 4K problems are solved.
    if (atoi(desc.width.c_str()) > 2880 || atoi(desc.height.c_str()) > 1620)
    {
        TRACE_ERROR(("canPlayType()-%d: type: %s, width: %s, height: %s - NO\n",__LINE__, desc.type.c_str(), desc.width.c_str(), desc.height.c_str()));
        return UVA_ERROR;
    }
#endif
*/
    if (atoi(desc.width.c_str()) > width || atoi(desc.height.c_str()) > height)
    {
        TRACE_ERROR(("canPlayType()-%d: type: %s, width: %s, height: %s - NO\n",__LINE__, desc.type.c_str(), desc.width.c_str(), desc.height.c_str()));
        return UVA_ERROR;
    }

#ifndef NOVATEK_PLATFORM
    if (atoi(desc.width.c_str()) == 3840 || atoi(desc.height.c_str()) == 2160)
    {
        NebulaMediaPlayer::changeYoutubeThreadPriorities(true);
    }
#endif

    int max_frame_rate;
    int max_bit_rate;
    int audio_channel_num;
    if (!NebulaMediaPlayer::getVideoCaps(&max_frame_rate, &max_bit_rate, &audio_channel_num))
    {
        max_frame_rate = default_max_frame_rate;
        max_bit_rate = default_max_bit_rate;
        audio_channel_num = default_audio_channel_num;
    }

    if (atoi(desc.bitrate.c_str()) > max_bit_rate || atoi(desc.framerate.c_str()) > max_frame_rate || atoi(desc.channels.c_str()) > audio_channel_num)
    {
        TRACE_ERROR(("canPlayType()-%d: type: %s, bitrate: %s, framerate: %s , audio channels : %s - NO\n", __LINE__, desc.type.c_str(), desc.bitrate.c_str(), desc.framerate.c_str(), desc.channels.c_str()));
        return UVA_ERROR;
    }
    UString crypto_block = desc.cryptoblockformat.c_str();
    if (!crypto_block.empty() && crypto_block != "subsample")
    {
        TRACE_ERROR(("canPlayType()-%d: type: %s, cryptoblockformat : %s - NO\n", __LINE__, desc.type.c_str(), desc.cryptoblockformat.c_str()));
        return UVA_ERROR;
    }
    if (!desc.type.length()) //mse-streaming
    {
        TRACE_VERBOSE(("canPlayType()-%d: type: %s, width: %s, height: %s - YES\n", __LINE__, desc.type.c_str(), desc.width.c_str(), desc.height.c_str()));
        return UVA_OK;    // TODO: Should be confirmed...
    }

    UVA_STATUS status = UVA_ERROR;
    if (desc.drmID.length())
    {
       TRACE_INFO(("Drm id : %s\n", desc.drmID.c_str()));
        std::vector<UVAKeySystemInfo> key_systems;
        if (UVADRMBackendManager::getSupportedKeySystems(key_systems) != UVA_OK) 
        {
            TRACE_ERROR(("canPlayType()-%d: No DRM backend available\n", __LINE__));
            return status;
        }
        
        for (auto& info : key_systems) 
        {
            TRACE_INFO(("Check drm : %s\n", desc.drmID.c_str()));
            if (!desc.drmID.compare(info.key_system.c_str()))
            {
                status = UVA_OK;
                break;
            }
        }
        if (status == UVA_ERROR)
        {
            TRACE_ERROR(("canPlayType()-%d: drm id %s is not supported\n", __LINE__, desc.drmID.c_str()));
            return status;
        }
    }

#ifndef INCLUDE_IP_TUNER
    //HDR related check - EOTF stands for electro-optical transfer function
    if (NEBULA_IsHDRSupportEnabled() == frost_true)
    {
        UString eotf_value = desc.eotf.c_str();
        if (eotf_value != NULL)
        {
            status = UVA_ERROR;
            TRACE_INFO(("canPlayType eotf : %s \n", eotf_value.charArray()));
            if (eotf_value == "bt709" || eotf_value == "smpte2084" || eotf_value == "arib-std-b67")
            {
                status = UVA_OK;
            }

            if (status == UVA_ERROR)
            {
                TRACE_ERROR(("canPlayType()-%d: eotf %s is not supported - NO\n", __LINE__
                                                                , eotf_value.charArray()));
                return status;
            }
        }
    }
    else
    {
        UString eotf_value = desc.eotf.c_str();
        if (eotf_value != NULL)
        {
            TRACE_ERROR(("canPlayType()-%d: eotf %s is not supported - NO\n", __LINE__, eotf_value.charArray()));
            return UVA_ERROR;
        }
    }
#endif

    status = UVA_ERROR;
    const int codec_count = desc.codecs.size();
    const char** codecs = new const char*[codec_count + 1];
    
    if (codecs)
    {
        for (int index=0; index<codec_count; ++index)
        {
            TRACE_VERBOSE(("codec : %s \n",desc.codecs[index].c_str()));
            codecs[codec_count] = desc.codecs[index].c_str();
        }
        codecs[codec_count] = NULL;
    }


    if (NebulaMediaPlayer::canPlayType(desc.type.c_str(), codec_count, codecs) == frost_true)
    {
        status = UVA_OK;
        if (desc.type.compare("video/webm") == 0)
        {
            if (desc.framerate.length() > 0)
            {
                int framerate = atoi(desc.framerate.c_str());
                TRACE_VERBOSE(("canPlayType()-%d: framerate: %d\n",__LINE__, framerate));
#ifndef INCLUDE_IP_TUNER
                nebula::setYoutubeFramerate(framerate);
#endif
            }
        }
        TRACE_INFO(("canPlayType()-%d: type: %s- YES\n",__LINE__, desc.type.c_str()));
#ifndef NOVATEK_PLATFORM
#ifndef INCLUDE_IP_TUNER
        nebula::setMseMimeType(desc.type.c_str(), false);
#endif
#endif
    }

    delete [] codecs;

    if (status == UVA_ERROR)
    {
        TRACE_ERROR(("canPlayType()-%d: type: %s - NO\n",__LINE__, desc.type.c_str()));
    }
    return status;
}

UVA_STATUS
OperaMuminMediaBackend::setFullScreen(bool fullscreen)
{
    TRACE_INFO(("setFullScreen(): %d \n", fullscreen));
    bool ok = m_av_object && m_av_object->setFullScreen(fullscreen);
    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::getSupportedMimetypes(char**& mime_types)
{
    TRACE_INFO(("getSupportedMimetypes() \n"));
    const char* types[]  = { "application/ogg", "audio/ogg", "video/ogg", "audio/vorbis",
                             "audio/flac", "audio/x-flac", "audio/wav", "audio/wave",
                             "audio/x-wav", "audio/mpeg", "audio/mp2", "audio/mp3",
                             "video/mpeg", "video/mp4", "video/quicktime", "video/3gpp",
                             "video/3gpp2", "video/h264", "video/H264", "video/mpeg4",
                             "audio/mp4", "audio/3gpp", "audio/3gpp2", "audio/amr",
                             "audio/amr-wb", "audio/aac", "audio/aacp", "audio/MP4A-LATM",
                             "audio/mpeg4-generic", "video/avi", "video/msvideo", "video/x-msvideo",
                             "video/x-ms-asf", "video/x-ms-wmv", "audio/x-ms-wma", "video/x-matroska",
                             "audio/x-matroska", "audio/x-wavpack", "video/x-flv", "audio/aiff",
                             "audio/x-aiff", "audio/mid", "audio/midi", "audio/x-midi",
                             "video/webm", "audio/webm",  "video/x-m4v", NULL };

    mime_types = new char*[sizeof(types)];
    if (mime_types)
    {
        unsigned int num_supported = 0;
        for (unsigned int i = 0; types[i] != NULL; i++)
        {
            const char* type = types[i];
            mime_types[num_supported] = strdup(type);
            if (mime_types[num_supported] == NULL)
            {
                break;
            }
            num_supported++;
        }
        mime_types[num_supported] = NULL;
    }
    return UVA_OK;
}

UVA_STATUS
OperaMuminMediaBackend::setLoop(int value)
{
    TRACE_INFO(("setLoop to %d\n", value));
    m_loop_count = value;
    return UVA_OK;
}

UVA_STATUS
OperaMuminMediaBackend::selectComponent(AVComponent* component)
{
    TRACE_INFO(("selectComponent() component: %p\n", component));
    if (m_component_helper)
    {
        return m_component_helper->selectComponent(component) ? UVA_OK : UVA_ERROR;
    }
    return UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::unselectComponent(AVComponent* component)
{
    TRACE_INFO(("unselectComponent() component: %p\n", component));
    if (m_component_helper)
    {
        return m_component_helper->unselectComponent(component) ? UVA_OK : UVA_ERROR;
    }
    return UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::setKeySystem(const char* key_system_id)
{
    if (key_system_id)
    {
        TRACE_INFO(("Key system is '%s'\n", key_system_id));
        m_key_system_id = key_system_id;

        m_drm_backend = m_client->getDRMBackend(key_system_id);
        if (m_drm_backend == NULL)
        {
            TRACE_ERROR((" Drm backend NULL \n"));
        }
        else
        {
            TRACE_INFO(("DRM Backend is ALIVE\n"));

            if (m_key_system_id != "com.widevine.alpha")
            {
                m_drm_type = drm_type_widevine;
            }
            else if ((m_key_system_id != "com.youtube.playready")
                    || (m_key_system_id != "urn:dvb:casystemid:19219")
                    || (m_key_system_id != "com.microsoft.playready")
                    )
            {
                m_drm_type = drm_type_playready;
            }
            else if (m_key_system_id != "com.verimatrix.viewright")
            {
                m_drm_type = drm_type_verimatrix;
            }
        }
    }
    else
    {
        m_key_system_id.clear();
        m_drm_type = drm_type_none;
    }

    return UVA_OK;
}

void
OperaMuminMediaBackend::generateFullScreenChangedEvent()
{
    TRACE_INFO(("generateFullScreenChangedEvent()\n"));
}

void
OperaMuminMediaBackend::generatePlayStateChangedEvent(NEBULA_MediaPlayerStatus state)
{
    TRACE_INFO(("(%d): Play state: %s for %s\n", m_backend_id, ::mediaPlayerStateName(state), (m_streaming_type==mse)?"MSE":"BACKEND"));

    if (!m_client || !m_av_object)
    {
        TRACE_ERROR(("In %s(): No Client, cannot send play state\n", __FUNCTION__));
        return;
    }

    if (state == NEBULA_Finished)
    {
        if (isLoopEnabled() && ((m_current_loop_count < m_loop_count) || m_loop_count == -1))
        {
            m_current_loop_count++;
            prepareForPlay(true);
        }
        else
        {
            if (isLoopEnabled())
            {
                m_current_loop_count = 0;
            }
            if (m_previous_play_state == NEBULA_Buffering ||
                m_sent_buffer_underrun == true)
            {
                sendEvent(new BufferingStreamBufferedEvent);
                m_sent_buffer_underrun = false;
            }
            sendEvent(new SeekCompletedExEvent(SeekOutcome::Succeeded));
            sendEvent(new PlaybackFinishedEvent);
            m_play_requested = false;
        }
        m_stream_offset = 0;
    }
    else if (state == NEBULA_Error)
    {
        if (m_stop_request_in_nebula_connecting_state)
        {
            //No need to generate error event if stop request is made.
            m_stop_request_in_nebula_connecting_state = false;
        }
        else
        {
            NEBULA_MediaPlayerError error =  m_av_object->getError();

            if ((error == NEBULA_CANNOTCONNECT_OR_CONNECTIONLOST ||
                 error == NEBULA_CONTENT_NOT_AVAILABLE) &&
                m_streaming_type == backend)
            {
                if(error == NEBULA_CANNOTCONNECT_OR_CONNECTIONLOST)
                {
                    sendEvent(new BufferingNetworkErrorEvent(BufferingNetworkErrorEvent::CONNECTION_LOST_OR_IMPOSSIBLE));
                }
                else
                {
                    sendEvent(new BufferingNetworkErrorEvent(BufferingNetworkErrorEvent::CONTENT_NOT_AVAILABLE));
                }
            }
            else if(error == NEBULA_AVFORMATNOTSUPPORTED)
            {
                sendEvent(new DecodeErrorEvent(DecodeErrorEvent::FORMAT_NOT_SUPPORTED));
            }
            else
            {
                sendEvent(new DecodeErrorEvent(DecodeErrorEvent::CONTENT_CORRUPTED));
            }
        }
    }
    else if (state == NEBULA_Playing)
    {
        m_file_details_parsed = true;
        m_playback_started = true;
        if (m_stop_request_in_nebula_connecting_state)
        {
            TRACE_INFO(("At NEBULA_playing state m_stop_request_in_nebula_connecting_state= true thus will call av object stop \n"));
            m_av_object->stop();
            m_stop_request_in_nebula_connecting_state = false;
        }

        if (!m_seek_is_active &&
            m_setposition_request)
        {
            seekActual(m_setposition_time);
        }

        if ((m_previous_play_state == NEBULA_Buffering && m_streaming_type == backend) ||
            m_sent_buffer_underrun == true)
        {
            if (m_seek_ok)
            {
                m_av_object->setSpeed(static_cast<int>(m_playback_rate));
                m_seek_ok = false;
            }
            sendEvent(new BufferingStreamBufferedEvent);
            m_sent_buffer_underrun = false;
        }

        if (m_streaming_type == backend)
        {
#if defined(NOVATEK_PLATFORM) || defined(_LINUX_PC_PLATFORM)

            float exact_ar = 1.777777778;
            if (m_av_object->getVideoExactAspectRatio(exact_ar) && (m_last_exact_aspect_ratio != (double)exact_ar))
            {
                int x = m_last_requested_video_window.x;
                int y = m_last_requested_video_window.y;
                int w = m_last_requested_video_window.w;
                int h = m_last_requested_video_window.h;

                adjustVideoAspectRatio(x, y, w, h);

                NEBULA_DisplayWindow nebula_window;
                nebula_window.x_position = static_cast<float>(x * sd_video_width / (double) m_browser_max_video_width);
                nebula_window.y_position = static_cast<float>(y * sd_video_height / (double) m_browser_max_video_height);
                nebula_window.width      = static_cast<float>(w * sd_video_width / (double) m_browser_max_video_width);
                nebula_window.height     = static_cast<float>(h * sd_video_height / (double) m_browser_max_video_height);

                applyVideoPosition(nebula_window);
            }
#endif
            //get&send duration in case we could not get duration when the file details are parsed.
            double duration = 0.0;
            if (getDuration(duration) == UVA_OK)
            {
                sendMetadata();
            }
        }
        m_av_object->setVisibility(true);
#ifndef NOVATEK_PLATFORM
        cabot::String current_url = bpi::windowManagerGetCurrentUrl();
        if (current_url.find("/ads/") != cabot::String::npos)
        {
            m_youtube_ad = true;
        }
#endif
    }
    else if (state == NEBULA_Buffering)
    {
        if (!(m_previous_play_state == NEBULA_Connecting && m_streaming_type == mse))
        {
            sendBufferUnderrun();
        }

        if (!m_seek_is_active &&
            m_setposition_request)
        {
            seekActual(m_setposition_time);
        }
    }
    else if(state == NEBULA_Started)
    {
        if (!m_youtube_video && (m_streaming_type == mse))
        {
            double cur_pixel_ar = calculatePixelAspectRatio();
            if (m_pixel_aspect_ratio != cur_pixel_ar)
            {
                TRACE_INFO(("Video position should be updated according to new pixel aspect ratio\n"));
                sendMetadata();
            }
        }
        //Started event is sent when video play is really started but current state should be playing
        if (m_previous_play_state != NEBULA_Playing)
        {
            state = NEBULA_Playing;
        }
    }
    m_previous_play_state = state;
}

void
OperaMuminMediaBackend::generatePlaySpeedChangedEvent(int play_speed)
{
    TRACE_INFO((" new speed: %d \n", play_speed));
    double actual_play_speed = ((double)play_speed) / (double)media_speed_coefficient;

    sendEvent(new PlaySpeedChangedEvent(actual_play_speed));
}

void
OperaMuminMediaBackend::generatePlayPositionChangedEvent(std::int64_t play_position)
{
    TRACE_INFO(("%s() new position: %lld , m_streaming_type: %d\n", __FUNCTION__, play_position, m_streaming_type));

    if (m_client)
    {
        if (m_streaming_type == backend && m_sent_buffer_underrun == true)
         {
             sendEvent(new BufferingStreamBufferedEvent());
             m_sent_buffer_underrun = false;
        }
        sendEvent(new SeekCompletedExEvent(m_setposition_request ? SeekOutcome::Aborted : SeekOutcome::Succeeded));
        FrostMutexLock lock(m_seek_mutex);
        if (m_seek_is_active)
        {
            m_seek_is_active = false;
            if (m_setposition_request)
            {
                seekActual(m_setposition_time);
            }
        }
    }
}

void
OperaMuminMediaBackend::generateFileDetailsParsedEvent(NEBULA_MediaFileDetails* source_details)
{
    TRACE_INFO(("(%d): generateFileDetailsParsedEvent() - ENTRY\n", m_backend_id));

    if (m_source_list.size() == 0)
    {
        TRACE_ERROR(("No source available\n"));
        return;
    }

    if (!m_client)
    {
        return;
    }

    m_file_details_parsed = true;

    if (m_component_helper)
    {
        m_component_helper->updateComponents(true);
        m_component_helper->selectInitialComponents((UVABackend::SourceDescription::SourceType)m_streaming_type);
    }

    if (m_streaming_type != adaptive && m_streaming_type != mse) //YT MSE pass TimestampOffsetVideo
    {
        m_total_playback_duration_in_secs = static_cast<double>(source_details->duration_in_miliseconds) / 1000;
    }

    if (m_streaming_type == backend)
    {
        m_backend_video_res_width = source_details->res_width;
        m_backend_video_res_height = source_details->res_height;
    }

    if (m_streaming_type != mse)
    {
        sendBufferUnderrun();
        sendMetadata(false);
    }

    if (m_play_requested)
    {
        if (!m_av_object->play())
        {
            sendEvent(new BufferingNetworkErrorEvent(BufferingNetworkErrorEvent::CONNECTION_LOST_OR_IMPOSSIBLE));
        }
        else
        {
            m_playback_rate = normal_playback_rate;
        }
    }
    if (m_streaming_type != mse)
    {
        sendEvent(new BufferingProgressEvent);
        sendEvent(new BufferingStreamBufferedEvent);
        m_sent_buffer_underrun = false;
    }
    TRACE_INFO(("generateFileDetailsParsedEvent() - END\n"));
}

void
OperaMuminMediaBackend::generateComponentsChangedEvent()
{
    if (m_component_helper)
    {
        m_component_helper->updateComponents();
    }
}

void
OperaMuminMediaBackend::generateComponentActivenessChangedEvent(
                                           const NEBULA_AVComponent *component_data,
                                           bool active)
{
    if (m_component_helper)
    {
        if (component_data)
        {
            m_component_helper->updateComponentActiveness(component_data,active);
        }
        else
        {            
            m_component_helper->updateComponents();
        }
    }
}

void
OperaMuminMediaBackend::generateUpdateAudioComponentEvent(char const* language, bool ad_on)
{
    if (m_component_helper)
    {
        UString lang_code(language);
        m_component_helper->selectProperAudioComponent(lang_code, ad_on);
    }
}

bool
OperaMuminMediaBackend::isLoopEnabled() const
{
    return m_loop_count > 0 || m_loop_count == -1;
}

UVA_STATUS
OperaMuminMediaBackend::attachSource(UVABackend::SourceDescription* source)
{
    // add the new stream to the playback and synchronize it with other streams
    // which may already exist. Return UVA_ERROR if not possible.
    TRACE_INFO(("Type : %d, source_id: %zu\n", source->type, source->source_id));

    return addSource(source);
}

UVA_STATUS
OperaMuminMediaBackend::removeSource(uintptr_t source_id)
{
    TRACE_INFO(("Removing source(%zu)\n", source_id));

    bool result = false;

    FrostMutexLock lock(m_source_mutex);
    const SourceIterator end = m_source_list.end();

    for (SourceIterator ii = m_source_list.begin(); ii != end; ++ii)
    {
        if ((*ii)->m_source_id == source_id)
        {
            result = m_av_object && m_av_object->detachSource(source_id);
            delete *ii;
            m_source_list.erase(ii);
            result = true;
            break;
        }
    }

    // if a seek was started before all the sources are removed, it must have been aborted
    if (m_seek_is_active && m_source_list.empty())
    {
        sendEvent(new SeekCompletedExEvent(SeekOutcome::Aborted));
    }
    return result ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminMediaBackend::setWindowId(uintptr_t window_id)
{
    TRACE_INFO(("Window ID is %p\n", (void*)window_id));

    // Important note: This is a very ugly hack to bypass an MStar driver limitation.
    // DO NOT extend this by adding new URL comparisons.
    cabot::String page_url = bpi::windowManagerGetPageUrl(window_id);

    TRACE_INFO(("page_url: [%s] \n", page_url.charArray()));

    if (page_url.find("bbc") != cabot::String::npos)
    {
        TRACE_INFO(("Seems to be a BBC video\n"));
        m_bbc_video = true;
    }
    else if (page_url.find("youtube.com/tv") != cabot::String::npos)
    {
        TRACE_INFO(("Seems to be a youtube video\n"));
        m_youtube_video = true;
    }
    else if ((page_url.find("channel4") != cabot::String::npos) ||
            (page_url.find("stv") != cabot::String::npos) ||
            (page_url.find("uktv") != cabot::String::npos) ||
            (page_url.find("channel5") != cabot::String::npos))
    {
          TRACE_INFO(("Seems to be a fvp video\n"));
          m_fvp_video = true;
    }
    else if(page_url.find("hbbtv.test") != cabot::String::npos ||
            page_url.find("hbbtv1.test") != cabot::String::npos ||
            page_url.find("hbbtv2.test") != cabot::String::npos)
    {
        TRACE_INFO(("Ligada URL found!!!\n"));
        m_ligada_is_active = true;

        if (page_url.find("ADINS") != cabot::String::npos)
        {
            m_ligada_is_ad_insertion_active = true;
        }
    }
    else if (page_url.find("itv") != cabot::String::npos )
    {
        TRACE_INFO(("Seems to be an ITV video and FVP video\n"));
        m_fvp_video = true;
        m_itv_video = true;
    }
    else if (page_url.find("magine/") != cabot::String::npos ||
             page_url.find("2968523233.staging.foxxum") != cabot::String::npos)
    {
        m_magine_tv_video = true;
    }
    else if (page_url.find("OpenSport") != cabot::String::npos)
    {
        TRACE_INFO(("Seems to be an OpenSport Video\n"));
        m_high_bitrate_video = true;
    }


    return UVA_OK;
}

mumin::DecrypterDescription*
OperaMuminMediaBackend::findDecrypter(const unsigned char* kid, size_t kid_size)
{
    mumin::DecrypterDescription* result = NULL;
    const DecrypterIterator end = m_decrypter_list.end();

    for (DecrypterIterator ii = m_decrypter_list.begin(); ii != end; ++ii)
    {
        if (((*ii)->m_kid_size == kid_size) && (memcmp((*ii)->m_kid, kid,  kid_size) == 0))
        {
            result = *ii;
            break;
        }
    }
    return result;

}

void
OperaMuminMediaBackend::addDecrypter(const unsigned char* kid, size_t kid_size)
{
    if (kid)
    {
        mumin::DecrypterDescription* existing_decrypter = findDecrypter(kid, kid_size);
        if (!existing_decrypter)
        {
            UVADRMBackend::UVADRMDecrypter* decrypter = NULL;
            m_drm_backend->getDecrypter(decrypter, kid, kid_size
            );
            if (decrypter)
            {
                m_decrypter_list.push_back(new mumin::DecrypterDescription(decrypter, kid, kid_size));
            }
        }
    }
}

void
OperaMuminMediaBackend::removeDecrypters()
{
    const DecrypterIterator end = m_decrypter_list.end();

    for (DecrypterIterator ii = m_decrypter_list.begin(); ii != end; ++ii)
    {
        if ((*ii)->m_decrypter && m_drm_backend)
        {
            m_drm_backend->releaseDecrypter((*ii)->m_decrypter);
            (*ii)->m_decrypter = NULL;
        }
        delete *ii;
    }

    m_decrypter_list.clear();
}

mumin::MuminSourceDescription*
OperaMuminMediaBackend::findSourceLocked(uintptr_t source_id)
{
    mumin::MuminSourceDescription* result = 0;

    const SourceIterator end = m_source_list.end();

    for (SourceIterator ii = m_source_list.begin(); ii != end; ++ii)
    {
        if ((*ii)->m_source_id == source_id)
        {
            result = *ii;
            break;
        }
    }
    return result;
}

UVA_STATUS
OperaMuminMediaBackend::addSource(UVABackend::SourceDescription* source_desc)
{
    UVA_STATUS result = UVA_ERROR;
    if (source_desc)
    {
        FrostMutexLock lock(m_source_mutex);
        mumin::MuminSourceDescription* existing_source = findSourceLocked(source_desc->source_id);

        if (!existing_source)
        {
            m_source_list.push_back(new mumin::MuminSourceDescription(source_desc));

            TRACE_INFO(("(%d): Adding source desc type(%d)\n", m_backend_id, source_desc->type));
            if (source_desc->type == UVABackend::SourceDescription::BACKEND_STREAMING)
            {
                char *url = source_desc->description.backend_streaming.url;
                TRACE_INFO(("In %s() - BACKEND, url (%s)\n", __FUNCTION__, url));

                AVControlObjectType type = av_control_object_type_net_stream;

                if (strncmp("mediafile:", url, 10) == 0)
                {
                    type = av_control_object_type_media_file;
                }
                else if (strncmp("pvr:", url, 4) == 0)
                {
                    type = av_control_object_type_pvr;
                }

#if 0
                if (m_av_object->getObjectType() != type)
                {
                    m_object_factory.discardOipfVideoOnDemandObject(m_av_object);
                    createVideoOnDemandObject(type);
                }
                else
                {
                    m_object_factory.markAVControlObjectAsUsed(m_av_object);
                }
#endif

                if (m_av_object->validURL(url))
                {
                    if (type == av_control_object_type_media_file)
                    {
                        url = &url[10]; // trim mediafile:
                    }

                    m_playback_url = url;
                    m_av_object->setSource(url);
                    m_av_object->setCookies(source_desc->description.backend_streaming.cookies);
                    TRACE_INFO(("initBackend(): OK\n"));
                    result = UVA_OK;
                }
                else
                {
                    result = UVA_NOT_SUPPORTED;
                }
            }
            else if (source_desc->type == UVABackend::SourceDescription::MSE_STREAMING)
            {
                result = addMseSource(source_desc);
            }
            else
            {
                SHOULD_NOT_BE_HERE();
                result =  UVA_NOT_SUPPORTED;
            }
        }
    }

    TRACE_INFO(("In %s(): Added source_id(%zu)\n", __FUNCTION__, source_desc->source_id));
    return result;
}

void
OperaMuminMediaBackend::removeSources()
{
    FrostMutexLock lock(m_source_mutex);
    const auto end = m_source_list.end();
    if (m_av_object)
    { 
        for (auto ii = m_source_list.begin(); ii != end; ++ii)
        {
            m_av_object->detachSource((*ii)->m_source_id);
            delete *ii;
        }
        m_source_list.erase(m_source_list.begin(), end);
    }
}

UVA_STATUS
OperaMuminMediaBackend::addMseSource(UVABackend::SourceDescription* source_desc)
{
    UVA_STATUS result = UVA_ERROR;

    CABOT_ASSERT(m_av_object);

    TRACE_INFO(("In %s() - MSE_STREAMING\n", __FUNCTION__));
    m_playback_url = "external://mse-streamer.ts";   // This is hardcoded right now

    if (source_desc)
    {
        // content_size is the actual file size for regular Opera streaming.
        // It is content duration for adaptive streaming
        long long unsigned int content_size = source_desc->description.client_streaming.content_size;
        bool seekable       = source_desc->description.client_streaming.seekable;
        uintptr_t source_id = source_desc->source_id;
        if (content_size != 0)
        {
            m_total_playback_duration_in_secs = static_cast<double>(content_size) / 1000;
        }
        TRACE_INFO(("%s(): Source duration: %f\n", __FUNCTION__, m_total_playback_duration_in_secs));
        TRACE_INFO(("%s(): Content size from Opera: %llu\n", __FUNCTION__, content_size));

        if (sendEvent(new BufferingStreamUnderrunEvent(source_id)))
        {
            m_sent_buffer_underrun = true;
        }
#ifndef NOVATEK_PLATFORM
#ifndef INCLUDE_IP_TUNER
        if (source_desc)
        {
             TRACE_INFO(("%s setting mime type to: %s \n", __FUNCTION__, source_desc->mime_type));
             nebula::setMseMimeType(source_desc->mime_type, true);
        }
#endif
#endif

        // TODO: Finish this. For audio only MSE streams
        if (source_desc->mime_type &&
            strncmp(source_desc->mime_type, "xxx-xxxx", 3) == 0 &&
            m_source_list.size() == 1)
        {
            sendEvent(new NewMetadataEvent(m_total_playback_duration_in_secs));
            TRACE_INFO(("%s(): Sending NewMetadataEvent: %g\n", __FUNCTION__, m_total_playback_duration_in_secs));
        }

        result = m_av_object->attachSource(source_id) ? UVA_OK : UVA_ERROR;
    }

    return result;
}

void
OperaMuminMediaBackend::getNativeVideoSize(int& w, int& h, double& aspect_ratio)
{
    aspect_ratio = 1.0;
    w            = 0;
    h            = 0;

    if (m_av_object)
    {
        w = static_cast<int>(m_av_object->getVideoDecodeWidth());
        h = static_cast<int>(m_av_object->getVideoDecodeHeight());

        aspect_ratio = m_pixel_aspect_ratio;
    }
}

bool
OperaMuminMediaBackend::prepareForPlay(bool start_playback)
{
    m_play_requested = start_playback;

    // start playback when buffering event is received by the backend
    return m_av_object->setSource(m_playback_url.charArray());
}

void
OperaMuminMediaBackend::sendMetadata(bool aspect_ratio_ready)
{
    double duration = INFINITY;
    int width  = browser_video_default_width;
    int height = browser_video_default_height;
    double ar  = 1.0;

    if (m_total_playback_duration_in_secs != 0)
    {
        duration = m_total_playback_duration_in_secs;
    }

    if (m_streaming_type == mse)
    {

        width = m_res_video_width;
        height = m_res_video_height;

        if (m_youtube_video)
        {
            ar = 1.0;
        }
        else
        {
            ar = calculatePixelAspectRatio();
        }
    }
    else if(m_streaming_type == backend)
    {
        if (m_backend_video_res_width != 0 && m_backend_video_res_height != 0)
        {
            width = m_backend_video_res_width;
            height = m_backend_video_res_height;
            if (aspect_ratio_ready)
            {
                float exact_aspect_ratio = 1.0;
                if (m_av_object && m_av_object->getVideoExactAspectRatio(exact_aspect_ratio))
                {
                    ar = ((double)m_backend_video_res_height / m_backend_video_res_width) * exact_aspect_ratio;
                }
            }
            else
            {
                ar = 1.0;
            }
        }
    }
    m_pixel_aspect_ratio = ar;
    sendEvent(new NewMetadataEvent(duration, width, height, ar));
    TRACE_INFO(("%s(): NewMetadataEvent, width/height %d/%d, duration %f, ar: %f\n", __FUNCTION__, width, height, duration, ar));
}

double
OperaMuminMediaBackend::calculatePixelAspectRatio()
{
    double pixel_aspect_ratio = 1.0;
    if ((m_res_video_width > 0) && (m_res_video_height > 0))
    {
        if (m_av_object)
        {
            //Default aspect ratio is 16:9
            float exact_aspect_ratio = 1.777777778;
            if (!m_av_object->getVideoExactAspectRatio(exact_aspect_ratio))
            {
                TRACE_ERROR(("%s() Cannot get exact aspect ratio\n", __FUNCTION__));
            }
            TRACE_INFO(("%s() exact_aspect_ratio: %f\n", __FUNCTION__, exact_aspect_ratio));
            pixel_aspect_ratio = ((double)m_res_video_height / m_res_video_width) * exact_aspect_ratio;
        }
    }
    return pixel_aspect_ratio;
}


bool
OperaMuminMediaBackend::sendBufferUnderrun()
{
    int err_cnt = 0;
    FrostMutexLock lock(m_source_mutex);
    const SourceIterator end = m_source_list.end();

    for (SourceIterator ii = m_source_list.begin(); ii != end; ++ii)
    {
        if (!sendEvent(new BufferingStreamUnderrunEvent((*ii)->m_source_id)))
        {
            ++err_cnt;
        }
        else
        {
            m_sent_buffer_underrun = true;
        }
    }
    return err_cnt > 0;
}

bool
OperaMuminMediaBackend::seekActual(double position_secs)
{
    TRACE_INFO(("(%d): Actual seek to %f\n", m_backend_id, position_secs));
    bool ok = false;
    const unsigned int msecs_in_a_second = 1000;
    double new_position_msecs = position_secs * msecs_in_a_second;

    if (m_streaming_type == backend)
    {
        sendBufferUnderrun();
    }
    ok = m_av_object->seek(new_position_msecs);
    if (ok)
    {
        m_setposition_request = false;
        m_setposition_time = 0;
        m_seek_is_active = true;
        m_seek_ok = true;
    }
    return ok;
}

bool
OperaMuminMediaBackend::sendEvent(UVAEvent* ev)
{
    bool result = false;
    bool print_event_name_info = true;
    cabot::String ev_name = "UNKNOWN";
    if (m_client && ev)
    {
#ifdef CABOT_TRACE
        switch(ev->GetType())
        {
        case UVAEvent::BackendResourceRevoked:   ev_name = "BackendResourceRevoked"; break;
        case UVAEvent::NeedData:                 ev_name = "NeedData"; break;
        case UVAEvent::NeedMSEData:              ev_name = "NeedMSEData";
            print_event_name_info = false;
            break;
        case UVAEvent::CancelDataRequestsAsync:  ev_name = "CancelDataRequestsAsync"; break;
        case UVAEvent::PlaybackFinished:         ev_name = "PlaybackFinished"; break;
        case UVAEvent::NewMetadata:              ev_name = "NewMetadata"; break;
        case UVAEvent::DecodeError:              ev_name = "DecodeEror"; break;
        case UVAEvent::SeekCompletedEx :         ev_name = "SeekCompletedExEvent"; break;
        case UVAEvent::VideoFrameAvailable:      ev_name = "VideoFrameAvailable"; break;
        case UVAEvent::DRMRightsError:           ev_name = "DRMRightsError"; break;
        case UVAEvent::ParentalRatingError:      ev_name = "ParentalRatingError"; break;
        case UVAEvent::ParentalRatingChange:     ev_name = "ParentalRatingChange"; break;
        case UVAEvent::PlaySpeedChanged:         ev_name = "PlaySpeedChanged"; break;
        case UVAEvent::BroadcastPlaystateChange: ev_name = "BroadcastPlaystateChange"; break;
        case UVAEvent::ChannelChangeError:       ev_name = "ChannelChangeError"; break;
        case UVAEvent::ChannelChangeSucceeded:   ev_name = "ChannelChangeSucceeded"; break;
        case UVAEvent::ProgrammesChanged:        ev_name = "ProgrammesChanged"; break;
        /*
        RecordingError,
        RecordingStateChange,
        */
        case UVAEvent::BufferingProgress:       ev_name = "BufferingProgress"; break;
        case UVAEvent::BufferingNetworkError:   ev_name = "BufferingNetworkError"; break;
        case UVAEvent::BufferingIdle:           ev_name = "BufferingIdle"; break;
        case UVAEvent::BufferingStreamUnderrun: ev_name = "BufferingStreamUnderrun"; break;
        case UVAEvent::BufferingStreamBuffered: ev_name = "BufferingStreamBuffered"; break;
        /*
        SubtitlesChanged,
        */
        case UVAEvent::ComponentChanged:        ev_name = "ComponentChanged"; break;
        }
#endif
        result = m_client->handleEvent(ev) == UVA_OK;
        FROST_Sleep(1);
        ev = nullptr;
    }

#ifdef CABOT_TRACE
    if (!result)
    {
        TRACE_ERROR(("(%d): Failed to send playback event (%s) to UVA client\n", m_backend_id, ev_name.charArray()));
    }
    else
    {
        if (print_event_name_info)
        {
            TRACE_INFO(("(%d): UVA Event(%s) sent for %s\n", m_backend_id, ev_name.charArray(), (m_streaming_type==mse)?"MSE":"BACKEND"));
        }
        else
        {
            TRACE_VERBOSE(("(%d): UVA Event(%s) sent\n", m_backend_id, ev_name.charArray()));
        }
    }
#endif

    return result;
}

bool
OperaMuminMediaBackend::setVolumeInternal(frost_int volume_level)
{
    if ((m_playback_volume * 100) == volume_level)
    {
        return true;
    }

    TRACE_INFO(("(%d): Setting volume to %d/100 \n", m_backend_id, volume_level));

    // TODO: Volume levels should be managed by each video backend.
    bool volume_set = NEBULA_SetVolumeLevel(volume_level, frost_true) == frost_true;

    return volume_set;
}

OperaMuminMediaBackend::StreamingType
OperaMuminMediaBackend::setStreamingType(UVABackend::SourceDescription** sources)
{
    UString url_string;
    m_streaming_type = mse;
    if (sources && sources[0])
    {
        switch (sources[0]->type)
        {
            case UVABackend::SourceDescription::BACKEND_STREAMING:
                m_streaming_type = backend;
                url_string = UString(sources[0]->description.backend_streaming.url);
                TRACE_INFO(("%s(): sources[0]->description.backend_streaming.url is %s\n", __FUNCTION__, sources[0]->description.backend_streaming.url));
                if (url_string.find("rtsp:") != UString::invalid_index)
                {
                    m_is_rtsp = true;
                    TRACE_INFO(("%s(): Setting m_is_rtsp to %d\n", __FUNCTION__, m_is_rtsp));
                }
                 else if (url_string.find("udp:") != UString::invalid_index)
                 {
                     m_is_udp = true;
                     TRACE_INFO(("%s(): Setting m_is_udp to %d\n", __FUNCTION__, m_is_udp));
                 }
            break;
            case UVABackend::SourceDescription::MSE_STREAMING: m_streaming_type = mse; break;
            case UVABackend::SourceDescription::BROWSER_STREAMING: m_streaming_type = opera; break;
        }
    }

    return m_streaming_type;
}

UVA_STATUS
OperaMuminMediaBackend::setContentSize(uintptr_t source_id, uva_uint64 content_size)
{
    if (m_streaming_type == mse)
    {
        m_total_playback_duration_in_secs = static_cast<double>(content_size / 1000);
        TRACE_INFO(("(%d): Setting total playback duration to %f\n", m_backend_id, m_total_playback_duration_in_secs));
    }
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackend::getBufferedTimeRanges(uintptr_t source_id, const std::vector<ByteRange>& input_byte_ranges, std::vector<TimeRange>& output_time_ranges)
{
    double start = 0.0, end = 0.0;
    if (m_av_object && m_av_object->getBufferedTimeRange(&start,&end))
    {
        if (m_streaming_type == mse)
        {
            FrostMutexLock lock(m_source_mutex);
            mumin::MuminSourceDescription* source_desc = findSourceLocked(source_id);
            if (source_desc && source_desc->m_source_id == source_id)
            {
                if(source_desc->m_last_received_pts > end)
                {
                    end = source_desc->m_last_received_pts;
                }
            }
        }
        output_time_ranges.push_back({start,end});
    }
    return UVA_OK;
}

UVA_STATUS 
OperaMuminMediaBackend::getPlaybackSpeedRanges(std::vector<SpeedRange>& speed_ranges)
{
    TRACE_INFO(("getPlaybackSpeedRanges for playback type: %d \n", m_streaming_type));

    bool result = false;
    
    int* discrete_speed_values = (int*) malloc(sizeof(int) * max_supported_trickmode_count);    
    if (!discrete_speed_values)
    {
        return UVA_ERROR;
    }
    
    memset(discrete_speed_values, invalid_trickmode, sizeof(int) * max_supported_trickmode_count);
    if (m_streaming_type == mse)
    {
        result = m_av_object->getSupportedTrickModes(discrete_speed_values, 
                                                     max_supported_trickmode_count);
    }

    for (int i = 0; discrete_speed_values[i] != invalid_trickmode; i++)
    {
        speed_ranges.push_back({ (double)discrete_speed_values[i] / 1000, 
                                 (double)discrete_speed_values[i] / 1000});    
    }
    free(discrete_speed_values);
    return result ? UVA_OK : UVA_ERROR;
}

void
OperaMuminMediaBackend::adjustVideoAspectRatio(int& x, int& y, int& w, int& h)
{
#if defined(NOVATEK_PLATFORM) || defined(_LINUX_PC_PLATFORM)
    float exact_ar = 1.777777778;

    TRACE_INFO(("IN %s(): (%d, %d, %d, %d)\n", __FUNCTION__, x, y, w, h));

    if (m_av_object && m_av_object->getVideoExactAspectRatio(exact_ar))
    {
        m_last_exact_aspect_ratio = exact_ar;
        double aspect_ratio = (double)w / h;

        TRACE_INFO(("%s() ar: %f\n", __FUNCTION__, aspect_ratio));
        if (aspect_ratio > 1.8)
        {
            TRACE_INFO(("%s() %d exact_ar: %f, ar > 1.8 \n", __FUNCTION__, __LINE__, exact_ar));

            double new_w = h * exact_ar;
            double new_x = x + double(w - new_w) / 2;

            x = new_x;
            w = new_w;
        }
        else if (aspect_ratio < 1.3)
        {
            TRACE_INFO(("%s() %d exact_ar: %f, ar < 1.3 \n", __FUNCTION__, __LINE__, exact_ar));

            double new_h = (double)w / exact_ar;
            double new_y = y + double(h - new_h) / 2;

            y = new_y;
            h = new_h;
        }
        else
        {
            if ((w < m_browser_max_video_width) && (h < m_browser_max_video_width))
            {
                TRACE_INFO(("%s() %d exact_ar: %f, ar: %f\n", __FUNCTION__, __LINE__, exact_ar, aspect_ratio));
                if (aspect_ratio > m_last_exact_aspect_ratio)
                {
                    double new_w = h * exact_ar;
                    double new_x = x + double(w - new_w) / 2;

                    x = new_x;
                    w = new_w;
                }
            }
            else
            {
                TRACE_INFO(("%s() %d exact_ar: %f, ar: %f\n", __FUNCTION__, __LINE__, exact_ar, aspect_ratio));

                if (m_last_exact_aspect_ratio < 1.3)
                {
                    double new_w = h * exact_ar;
                    double new_x = x + double(w - new_w) / 2;

                    x = new_x;
                    w = new_w;
                }
                else if (m_last_exact_aspect_ratio > 2.3)
                {
                    double new_h = (double)w / exact_ar;
                    double new_y = y + double(h - new_h) / 2;

                    y = new_y;
                    h = new_h;
                }
            }
        }
    }

    TRACE_INFO(("OUT %s() (%d, %d, %d, %d)\n", __FUNCTION__, x, y, w, h));
#endif
}

bool
OperaMuminMediaBackend::applyVideoPosition(NEBULA_DisplayWindow &nebula_window)
{
    if ((m_last_video_window.x_position == nebula_window.x_position) &&
        (m_last_video_window.y_position == nebula_window.y_position) &&
        (m_last_video_window.width == nebula_window.width) &&
        (m_last_video_window.height == nebula_window.height))
    {
        return true;
    }

    bool ok = false;
    ok = m_av_object && m_av_object->setVideoOutputWindow(&nebula_window, true);
    if (ok)
    {
        m_last_video_window.x_position = nebula_window.x_position;
        m_last_video_window.y_position = nebula_window.y_position;
        m_last_video_window.width = nebula_window.width;
        m_last_video_window.height = nebula_window.height;
    }
    return ok;
}

