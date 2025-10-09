/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMuminMediaBackend
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINMEDIABACKEND_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINMEDIABACKEND_HPP_INCLUDED

#include "uva_backend.h"
#include "uva_event.h"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_public/AnyCommand.hpp"
#include "utilities_private/CommandThread.hpp"
#include "utilities_public/AnyMediaDataProvider.hpp"
#include "nebula/core/browser_client/AnyAVControlObject.hpp"

#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"

#include "utilities_debug/unused.h"

#include <string>
#include <stdio.h>
#include <vector>

class OperaMuminAVComponentHelper;
class AnyStreamData;
class OperaMuminObjectFactory;
class OperaMuminMediaBackend;
class PlayReadyDRMBackend;
namespace mumin
{
class NeedDataCommand;
}

namespace mumin
{
class MuminSourceDescription
{
public:
    MuminSourceDescription(UVABackend::SourceDescription* uva_source_desc)
        : m_pending_need_data(false)
        , m_source_id(uva_source_desc->source_id)
        , m_source_type(uva_source_desc->type)
        , m_first_data_after_jump(true)
        , m_received_packet_count(0)
        , m_last_received_pts(0.0)
    {}

    bool m_pending_need_data;
    uintptr_t m_source_id;
    UVABackend::SourceDescription::SourceType m_source_type;
    bool m_first_data_after_jump;
    unsigned int m_received_packet_count;
    double m_last_received_pts;
};

class DecrypterDescription
{
public:
   DecrypterDescription(UVADRMBackend::UVADRMDecrypter* decrypter,
                   const unsigned char* kid,
                   size_t kid_size);

   virtual ~DecrypterDescription();

    UVADRMBackend::UVADRMDecrypter* m_decrypter;
    unsigned char* m_kid;
    size_t m_kid_size;

private:
    //copy constructor & assignment operations disallowed
    DecrypterDescription(const DecrypterDescription& obj);
    DecrypterDescription& operator=(const DecrypterDescription& rhs);
};

class SendMetaDataCommand : public AnyCommand
{
public:
    SendMetaDataCommand(OperaMuminMediaBackend& media_backend)
        : m_media_backend(media_backend)
    {}

    virtual void execute();
private:
    OperaMuminMediaBackend& m_media_backend;
};

class WriteDataCommand : public AnyCommand
{
public:
    WriteDataCommand(OperaMuminMediaBackend& media_backend,
            UVAEvent* need_data_event, size_t bytes_written, UVABackend::WriteDataStatus status)
        : m_media_backend(media_backend)
        , m_need_data_event(need_data_event)
        , m_bytes_written(bytes_written)
        , m_write_status(status)
    {}
    virtual void execute();
private:
    OperaMuminMediaBackend& m_media_backend;
    UVAEvent* m_need_data_event;
    size_t m_bytes_written;
    UVABackend::WriteDataStatus m_write_status;
};

class NeedDataCommand : public AnyCommand
{
public:
    NeedDataCommand(OperaMuminMediaBackend& media_backend,
            uintptr_t source_id, size_t req_len, unsigned char* buffer)
        : m_media_backend(media_backend)
        , m_source_id(source_id)
        , m_req_len(req_len)
        , m_buffer(buffer)
    {}
    virtual void execute();

private:
    OperaMuminMediaBackend& m_media_backend;
    uintptr_t m_source_id;
    size_t m_req_len;
    unsigned char* m_buffer;
};

class CancelDataRequestsCommand : public AnyCommand
{
public:
    CancelDataRequestsCommand(OperaMuminMediaBackend& media_backend, double for_position_msecs)
        : m_media_backend(media_backend)
        , m_position_msecs(for_position_msecs)
    {}
    virtual void execute();

private:
    OperaMuminMediaBackend& m_media_backend;
    double m_position_msecs;
};

class CancelDataRequestsCompleteCommand : public AnyCommand
{
public:
    CancelDataRequestsCompleteCommand(OperaMuminMediaBackend& media_backend)
        : m_media_backend(media_backend)
    {}
    virtual void execute();

private:
    OperaMuminMediaBackend& m_media_backend;
};

class NeedStreamDataEvent : public NeedDataEvent
{
public:
    enum Requester
    {
        tuner,
        media_player
    };

    NeedStreamDataEvent(Requester requester,
                        uintptr_t _source_id,
                        size_t _offset,
                        unsigned char* _buffer,
                        size_t _size_request)
        : NeedDataEvent(source_id, offset, buffer, size_request)
        , m_requester(requester)
    {
        CABOT_UNUSED_PARAM(_source_id);
        CABOT_UNUSED_PARAM(_offset);
        CABOT_UNUSED_PARAM(_buffer);
        CABOT_UNUSED_PARAM(_size_request);
    }

    Requester m_requester;
};

}

/** Array of MuminSourceDescription pointers. */
typedef std::vector<mumin::MuminSourceDescription*> VectorOfSourceDescriptionPtr;

/** Array of DecrypterDescription pointers. */
typedef std::vector<mumin::DecrypterDescription*> VectorOfDecrypterDescriptionPtr;

class OperaMuminMediaBackend : public UVAMediaBackendBase,
                               public AnyAVControlObjectEventGenerator,
                               public AnyMediaDataProvider
{
public:
    enum StreamingType
    {
        adaptive,
        mse,
        opera,
        backend
    };

    OperaMuminMediaBackend(UVABackendType type,
                           UVAClient* _client,
                           OperaMuminObjectFactory& object_factory,
                           CommandThread& queue);
    virtual ~OperaMuminMediaBackend() override;

    virtual UVA_STATUS initBackend(const char* origin,
            UVABackend::SourceDescription** sources,
            bool automatic_key_system_selection) override;
    virtual UVA_STATUS setVideoWindowAndClip(const Rect& window_rect, const Rect& clip_rect) override;
    virtual UVA_STATUS load() override;
    virtual UVA_STATUS play() override;
    virtual UVA_STATUS stop();
    virtual UVA_STATUS pause() override;
    virtual UVA_STATUS suspend() override;
    virtual UVA_STATUS setPlaybackRate(double rate) override;
    virtual UVA_STATUS setVolume(double volume) override;
    virtual UVA_STATUS getDuration(double &duration) override;
    virtual UVA_STATUS getPosition(double &position) override;
    virtual UVA_STATUS setPosition(double position) override;
    virtual UVA_STATUS setVisibility(bool visible) override;
    virtual UVA_STATUS writeData(UVAEvent* need_data_event, size_t bytes_written, WriteDataStatus status) override;
    virtual UVA_STATUS canPlayType(const ContentDescription& desc) override;
    virtual UVA_STATUS setFullScreen(bool fullscreen) override;
    virtual UVA_STATUS getSupportedMimetypes(char**& mime_types) override;
    virtual UVA_STATUS setLoop(int value) override;
    virtual UVA_STATUS setKeySystem(const char* key_system_id) override;
    virtual UVA_STATUS selectComponent(AVComponent* component) override;
    virtual UVA_STATUS unselectComponent(AVComponent* component) override;
    virtual UVA_STATUS attachSource(SourceDescription* source) override;
    virtual UVA_STATUS removeSource(uintptr_t source_id) override;
    virtual UVA_STATUS setWindowId(uintptr_t window_id) override;
    virtual UVA_STATUS setContentSize(uintptr_t source_id, uva_uint64 content_size) override;
    virtual UVA_STATUS getBufferedTimeRanges(uintptr_t source_id, const std::vector<ByteRange>& input_byte_ranges, std::vector<TimeRange>& output_time_ranges);
    virtual UVA_STATUS getPlaybackSpeedRanges(std::vector<SpeedRange>& speed_ranges) override;
    virtual void eventCompleted(const UVAAsyncEvent* event, bool success) override;

public:     // implement AnyMediaDataProvider
    virtual bool needData(uintptr_t source_id, size_t req_len, unsigned char* buffer) override;
    virtual bool cancelRequestsForSeek(double new_position_msecs) override;

public:
    virtual void generateFullScreenChangedEvent();
    virtual void generatePlayStateChangedEvent(NEBULA_MediaPlayerStatus state);
    virtual void generatePlaySpeedChangedEvent(int play_speed);
    virtual void generatePlayPositionChangedEvent(std::int64_t play_position);
    virtual void generateFileDetailsParsedEvent(NEBULA_MediaFileDetails* source_details);
    virtual void generateComponentsChangedEvent();
    virtual void generateComponentActivenessChangedEvent(const NEBULA_AVComponent *component_data, bool active);
    virtual void generateUpdateAudioComponentEvent(char const* language, bool ad_on);

private:
    // copy constructor & assignment operations disallowed
    OperaMuminMediaBackend(OperaMuminMediaBackend const &);
    OperaMuminMediaBackend& operator=(OperaMuminMediaBackend const &);

    friend class mumin::WriteDataCommand;
    friend class mumin::SendMetaDataCommand;
    friend class mumin::NeedDataCommand;
    friend class mumin::CancelDataRequestsCommand;
    friend class mumin::CancelDataRequestsCompleteCommand;

    bool isLoopEnabled() const;
    mumin::MuminSourceDescription* findSourceLocked(uintptr_t source_id);
    mumin::DecrypterDescription* findDecrypter(const unsigned char* kid, size_t kid_size);
    void addDecrypter(const unsigned char* kid, size_t kid_size);
    void removeDecrypters();
    void removeSources();
    UVA_STATUS addSource(UVABackend::SourceDescription* source_desc);
    UVA_STATUS addMseSource(UVABackend::SourceDescription* source_desc);
    bool writeDataInternalLocked(uintptr_t source_id, AnyStreamData* stream_data, bool end_of_data);
    bool processWriteDataEvent(UVAEvent* need_data_event, size_t bytes_written, UVABackend::WriteDataStatus status);
    bool needDataInternal(uintptr_t source_id, size_t req_len, unsigned char* buffer);
    void getNativeVideoSize(int& w, int &h, double& aspect_ratio);
    bool sendBufferUnderrun();
    bool sendEvent(UVAEvent* ev);
    bool prepareForPlay(bool start_playback);
    bool seekActual(double position);
    void refreshComponentHelper();
    bool createVideoOnDemandObject(AVControlObjectType type, char* origin);
    bool setVolumeInternal(frost_int volume_level);
    StreamingType setStreamingType(UVABackend::SourceDescription** sources);
    void sendMetadata(bool aspect_ratio_ready = true);
    double calculatePixelAspectRatio();
    bool cancelRequestsForSeekInternal(double new_position_msecs);
    void cancelDataRequestsComplete();

    void adjustVideoAspectRatio(int& x, int& y, int& w, int& h);

    bool applyVideoPosition(NEBULA_DisplayWindow &window);

private:
    UVAClient* m_client;
    AnyAVControlObject* m_av_object;
    OperaMuminAVComponentHelper* m_component_helper;
    NEBULA_DisplayWindow m_last_video_window;
    int m_loop_count;
    int m_current_loop_count;
    UString m_playback_url;
    double m_total_playback_duration_in_secs;
    unsigned int m_backend_video_res_width;
    unsigned int m_backend_video_res_height;

    double m_setposition_time;
    bool m_setposition_request;
    double m_position;
    FrostMutex m_seek_mutex;

    /// True if stop request arrived in hbb_connecting_state
    bool m_stop_request_in_nebula_connecting_state;
    bool m_file_details_parsed;

    StreamingType m_streaming_type;

    // These are used for debugging purposes
    size_t m_stream_offset;
    FILE* m_stream_fp;

    VectorOfSourceDescriptionPtr m_source_list;
    VectorOfDecrypterDescriptionPtr m_decrypter_list;
    typedef VectorOfSourceDescriptionPtr::iterator SourceIterator;
    typedef VectorOfDecrypterDescriptionPtr::iterator DecrypterIterator;
    FrostMutex m_source_mutex;
    bool m_async_cancel_in_progress;

    double m_playback_volume;
    double m_playback_rate;
    bool   m_play_requested;
    bool   m_load_requested;

    UVADRMBackend*       m_drm_backend;
    PlayReadyDRMBackend* m_playready_drm_backend;
    std::string           m_key_system_id;

    OperaMuminObjectFactory& m_object_factory;

    MediaDataSource m_media_data_source;
    MediaDataSink   m_media_data_sink;

    bool m_seek_ok;
    bool m_seek_is_active;
    NEBULA_MediaPlayerStatus m_previous_play_state;
    static const int media_speed_coefficient = 1000;

    bool m_bbc_video;
    bool m_itv_video;
    bool m_youtube_video;
    bool m_youtube_ad;
    bool m_fvp_video;   //encapsulates all FVP apps including BBC
    bool m_magine_tv_video;
    bool m_high_bitrate_video;  //for Opensport service
    bool m_playback_started;

    int  m_res_video_width;
    int  m_res_video_height;
    double m_pixel_aspect_ratio;
    bool m_init_ok;
    int m_drm_type;
    CommandThread& m_queue;
    bool m_suspended;
    bool m_sent_buffer_underrun;
    bool m_ligada_is_active;
    bool m_is_rtsp;
    bool m_is_udp;
    bool m_ligada_is_ad_insertion_active;
    Rect m_last_requested_video_window;
    double m_last_exact_aspect_ratio;
    unsigned int m_browser_max_video_width;
    unsigned int m_browser_max_video_height;

    // These are added as a debugging aid to identify backends when multiple
    // video backends coexist
    static int m_backend_id_counter;
    int m_backend_id;
};

#endif
