/**
 * Copyright 2015 Vestel. All rights reserved.
 *
 * @file OperaWebAudioBackend.cpp
 *
 * @brief This file contains the implementation of the class
 * OperaWebAudioBackend
 */
#include "OperaWebAudioBackend.hpp"

#include "uva_client.h"
#include "uva_event.h"
#include <stdio.h>
#include "frost/frost_threads.h"
#include "frost/frost_time.h"
#include "nebula/core/browser_client/WebAudio.hpp"
#include "utilities_public/AnyCommand.hpp"
#include "utilities_debug/trace.h"
#include "utilities_debug/cabot_assert.h"
TRACE_IMPLEMENT("mumin/web_audio_backend");

#define REQUEST_DATA_SIZE 4096

class RequestDataCommand : public AnyCommand
{
public:
    RequestDataCommand(OperaWebAudioBackend* opera_web_audio_backend)
    : m_opera_web_audio_backend(opera_web_audio_backend)
    {
    }

    virtual void execute()
    {
        m_opera_web_audio_backend->needData(REQUEST_DATA_SIZE);
    }
private:
    OperaWebAudioBackend* m_opera_web_audio_backend;
};

WebAudioNeedDataEvent::WebAudioNeedDataEvent(uintptr_t source_id,
                                             size_t offset,
                                             unsigned char* buffer,
                                             size_t size_request)
    : NeedDataEvent(source_id, offset, buffer, size_request)
{
}

WebAudioNeedDataEvent::~WebAudioNeedDataEvent()
{
}

OperaWebAudioBackend::OperaWebAudioBackend(UVAClient* client,
                                           WebAudio& web_audio)
    : UVAMediaBackend(UVAWebAudioBackendType)
    , m_client(client)
    , m_web_audio(web_audio)
    , m_beep_source()
    , m_beep_offset(0)
{
}

OperaWebAudioBackend::~OperaWebAudioBackend()
{
    if (m_beep_source)
    {
        delete m_beep_source;
        m_beep_source = 0;
    }

    m_command_thread.terminateAndWaitForExit();
}

UVA_STATUS
OperaWebAudioBackend::initBackend(const char* origin,
                                  UVABackend::SourceDescription** sources,
                                  bool automatic_key_system_selection)
{
    UVA_STATUS result = UVA_OK;

    if (sources[0])
    {
        result = attachSource(sources[0]);
    }

    if (!m_command_thread.initialise("OperaWebAudioBackendCommand",
                                      FROST_high_priority,
                                      512))
    {
        TRACE_ERROR(("initialise() could not create thread\n"));
        result = UVA_ERROR;
    }

    if (result != UVA_OK)
    {
        for (int i = 0; sources[i]; ++i)
        {
            delete sources[i];
        }
        delete sources;
    }

    return result;
}

UVA_STATUS
OperaWebAudioBackend::attachSource(SourceDescription* source)
{
    UVA_STATUS result = UVA_ERROR;

    if (source)
    {
        m_beep_source = new WebAudioSourceDescription(source);
        if (m_beep_source)
        {
            result = UVA_OK;
        }
    }

    return result;
}

UVA_STATUS
OperaWebAudioBackend::setPlaybackRate(double rate)
{
    return UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::setVolume(double volume)
{
    return UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::setContentSize(uintptr_t source_id,
                                     uva_uint64 content_size)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getDuration(double& duration)
{
    return UVA_OK;
}


UVA_STATUS
OperaWebAudioBackend::setVisibility(bool visible,
                                    bool unloaded)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getSupportedMimetypes(char**& mime_types)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::setLoop(int value)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::queue(const char* url)
{
    return UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::setWindowId(uintptr_t window_id)
{
    return UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::setVideoWindow(int x,
                                     int y,
                                     int w,
                                     int h)
{
  return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::setClipRect(int x,
                                  int y,
                                  int w,
                                  int h)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getNativeSize(int& w,
                                    int& h,
                                    double& pixel_aspect)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getVideoFrame(unsigned char* buffer,
                                    size_t buffer_size)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getBufferedTimeRanges(ByteRange** input_byte_ranges,
                                            TimeRange**& output_time_ranges)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::pause(bool get_resources)
{
    return UVA_NOT_SUPPORTED;
}


UVA_STATUS
OperaWebAudioBackend::getPosition(double& position)
{
    return UVA_NOT_SUPPORTED;
}


UVA_STATUS
OperaWebAudioBackend::getStartDate(double& date)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::setPosition(double position)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getSizeFromDuration(double duration,
                                          uva_uint64& size)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::canPlayType(const char* type,
                                  const char** codecs,
                                  const char* drm_id)
{
    return UVA_ERROR;
}

UVA_STATUS
OperaWebAudioBackend::setFullScreen(bool fullscreen)
{
    return UVA_OK;
}


UVA_STATUS
OperaWebAudioBackend::selectComponent(AVComponent* component)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::unselectComponent(AVComponent* component)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::onListenerChanged(const char* event_name)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::getBytesInBuffer(uva_uint64& bytes)
{
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaWebAudioBackend::setKeySystem(const char* key_system_id)
{
    return UVA_NOT_SUPPORTED;
}

bool
OperaWebAudioBackend::needData(size_t req_len)
{
    m_web_audio_buffer = static_cast<unsigned char*>(malloc(REQUEST_DATA_SIZE * sizeof(unsigned char)));
    if (!m_web_audio_buffer)
    {
        TRACE_ERROR(("m_web_audio_buffer cannot be allocated! \n"));
        return false;
    }

    if (!m_client || !m_beep_source)
    {
        return false;
    }

    unsigned char* req_buffer = m_web_audio_buffer;

    UVAEvent* event = new WebAudioNeedDataEvent
                             (m_beep_source->m_uva_source_desc->source_id,
                              m_beep_offset,
                              req_buffer,
                              req_len);

    UVA_STATUS status = UVA_ERROR;
    if (event)
    {
        status = m_client->handleEvent(event);
    }

    return status == UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::play()
{
    m_command_thread.enqueueCommand(new RequestDataCommand(this));
    return UVA_OK;
}

UVA_STATUS
OperaWebAudioBackend::stop()
{
    return UVA_NOT_INITIALIZED;
}


UVA_STATUS
OperaWebAudioBackend::writeData(UVAEvent* need_data_event, size_t bytes_written, WriteDataStatus status)
{
    CABOT_ASSERT(need_data_event);
    
    WebAudioNeedDataEvent* ev = static_cast<WebAudioNeedDataEvent *>(need_data_event);
    m_web_audio.writeWebAudioData(ev->buffer, bytes_written);
    if (m_web_audio_buffer)
    {
        delete m_web_audio_buffer;
    }

    m_beep_offset += bytes_written;

    m_command_thread.enqueueCommand(new RequestDataCommand(this));
    delete need_data_event;
    return UVA_OK;
}
