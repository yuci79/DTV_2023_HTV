/**
 * Copyright 2015 Vestel.  All rights reserved.
 *
 * @file OperaWebAudioBackend.hpp
 *
 * @brief This file defines the class OperaWebAudioBackend
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAWEBAUDIOBACKEND_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAWEBAUDIOBACKEND_HPP_INCLUDED

#include "uva_backend.h"
#include "uva_event.h"
#include "utilities_private/CommandThread.hpp"

class WebAudio;
class AnyCommandThread;

class WebAudioNeedDataEvent : public NeedDataEvent
{
public:
    WebAudioNeedDataEvent(uintptr_t source_id,
                          size_t offset,
                          unsigned char* buffer,
                          size_t size_request);
    ~WebAudioNeedDataEvent();
};

class WebAudioSourceDescription
{
public:
    WebAudioSourceDescription(UVABackend::SourceDescription* uva_source_desc)
    : m_uva_source_desc(uva_source_desc)
    {
    }

    UVABackend::SourceDescription* m_uva_source_desc;
};

class OperaWebAudioBackend : public UVAMediaBackend
{
public:
    OperaWebAudioBackend(UVAClient* client,
                         WebAudio& web_audio);

    virtual ~OperaWebAudioBackend();

    virtual UVA_STATUS initBackend(const char* origin,
                                   UVABackend::SourceDescription** sources,
                                   bool automatic_key_system_selection);

    virtual UVA_STATUS play();
    virtual UVA_STATUS stop();
    virtual UVA_STATUS writeData(UVAEvent* need_data_event, size_t bytes_written, WriteDataStatus status);
    virtual UVA_STATUS attachSource(SourceDescription* source);
    bool needData(size_t req_len);
    // dummies
    virtual UVA_STATUS setWindowId(uintptr_t window_id);
    virtual UVA_STATUS setVideoWindow(int x, int y, int w, int h);
    virtual UVA_STATUS setClipRect(int x, int y, int w, int h);
    virtual UVA_STATUS getNativeSize(int& w, int& h, double& pixel_aspect);
    virtual UVA_STATUS getVideoFrame(unsigned char* buffer, size_t buffer_size);
    virtual UVA_STATUS getBufferedTimeRanges(ByteRange** input_byte_ranges,
                                             TimeRange**& output_time_ranges);
    virtual UVA_STATUS pause(bool get_resources);
    virtual UVA_STATUS getPosition(double& position);
    virtual UVA_STATUS getStartDate(double& date);
    virtual UVA_STATUS setPosition(double position);
    virtual UVA_STATUS getSizeFromDuration(double duration, uva_uint64& size);
    virtual UVA_STATUS canPlayType(const char* type, const char** codecs, const char* drm_id);
    virtual UVA_STATUS setFullScreen(bool fullscreen);
    virtual UVA_STATUS selectComponent(AVComponent* component);
    virtual UVA_STATUS unselectComponent(AVComponent* component);
    virtual UVA_STATUS onListenerChanged(const char* event_name);
    virtual UVA_STATUS getBytesInBuffer(uva_uint64& bytes);
    virtual UVA_STATUS setKeySystem(const char* key_system_id);
    virtual UVA_STATUS setPlaybackRate(double rate);
    virtual UVA_STATUS setVolume(double volume);
    virtual UVA_STATUS setContentSize(uintptr_t source_id, uva_uint64 content_size);
    virtual UVA_STATUS getDuration(double& duration);
    virtual UVA_STATUS setVisibility(bool visible, bool unloaded);
    virtual UVA_STATUS getSupportedMimetypes(char**& mime_types);
    virtual UVA_STATUS queue(const char* url);
    virtual UVA_STATUS setLoop(int value);

private:
    // copy constructor & assignment operations disallowed
    OperaWebAudioBackend(OperaWebAudioBackend const&); // not to be implemented
    OperaWebAudioBackend& operator=(OperaWebAudioBackend const&); // not to be implemented

    UVAClient* m_client;
    unsigned char* m_web_audio_buffer;
    WebAudioSourceDescription* m_beep_source;
    size_t m_beep_offset;
    WebAudio& m_web_audio;
    CommandThread m_command_thread;
};
#endif
