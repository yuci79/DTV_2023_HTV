/**
 * Copyright 2018 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class
 *        OperaMuminCicamPlayerBackend.
 *        
 * The current implementation is incomplete and a work in progress.
 * Functions have been implemented to simulate the process of playing media
 * via the CICAM player, including:
 * 
 * - Sending events to the UVAClient to trigger further calls, e.g.
 *   BufferingStreamBuffered to indicate that playback can be started.
 *    
 * - Returning hard-coded values where they are not currently known, e.g.
 *   video dimensions.
 *   
 * - Returning success from functions when they are not implemented.
 */

#include "OperaMuminCicamPlayerBackend.hpp"
#include "nebula/adaptation_layer/AnyCicamPlayer.hpp"
#include "nebula/adaptation_layer/AnyCicamPlayerPublisher.hpp"
#include "OperaMuminObjectFactory.hpp"

#include "vewd_integration/helper/ServiceLocationXmlBuilder.hpp"

#include "uva_client.h"

#include "utilities_public/AnyCommand.hpp"

#include "utilities_private/Time.hpp"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"

#include "frost/frost_time.h"
#include "frost/frost_time_constants.h"

#include <limits>
#include <sstream>

TRACE_IMPLEMENT("mumin/cicam_player_backend");

namespace
{

// Interval for requests for player information (duration, position and speed).
// The value is fairly arbitrary, but must be less than 30 seconds.
// See ETSI TS 102 796 section K.4.
int const info_request_interval_ms = 20 * milliseconds_per_second;
int const info_request_timer_id = 0;

std::string
serviceLocationtoXml(UVABackend::SourceDescription const* src)
{
    ServiceLocationXmlBuilder builder;
    builder.initialise();

    builder.addContent(src->description.cicam_streaming.url,
                       src->description.cicam_streaming.content_type);
    auto const* drm_info = src->description.cicam_streaming.drm_control_information;
    if (drm_info != nullptr)
    {
        builder.addDrmControlInfo(
            drm_info->drm_system_id, drm_info->drm_content_id,
            drm_info->rights_issuer_url, drm_info->drm_private_data);

    }
    return builder.buildXml();
}

#ifdef CABOT_TRACE
char const * toString(UVAEvent::UVAEventType type)
{
    char const * ev_name = "unknown";
    switch(type)
    {
    case UVAEvent::BackendResourceRequestAsync:
        ev_name = "BackendResourceRequestAsync";
        break;
    case UVAEvent::BackendResourceRevoked:
        ev_name = "BackendResourceRevoked";
        break;
    case UVAEvent::CancelDataRequestsAsync:
        ev_name = "CancelDataRequestsAsync";
        break;
    case UVAEvent::PlaybackFinished:
        ev_name = "PlaybackFinished";
        break;
    case UVAEvent::NewMetadata:
        ev_name = "NewMetadata";
        break;
    case UVAEvent::DecodeError:
        ev_name = "DecodeError";
        break;
    case UVAEvent::SeekCompletedEx:
        ev_name = "SeekCompletedEx";
        break;
    case UVAEvent::VideoFrameAvailable:
        ev_name = "VideoFrameAvailable";
        break;
    case UVAEvent::DRMRightsError:
        ev_name = "DRMRightsError";
        break;
    case UVAEvent::ParentalRatingError:
        ev_name = "ParentalRatingError";
        break;
    case UVAEvent::ParentalRatingChange:
        ev_name = "ParentalRatingChange";
        break;
    case UVAEvent::PlaySpeedChanged:
        ev_name = "PlaySpeedChanged";
        break;
    case UVAEvent::BroadcastPlaystateChange:
        ev_name = "BroadcastPlaystateChange";
        break;
    case UVAEvent::ChannelChangeError:
        ev_name = "ChannelChangeError";
        break;
    case UVAEvent::ChannelChangeSucceeded:
        ev_name = "ChannelChangeSucceeded";
        break;
    case UVAEvent::ProgrammesChanged:
        ev_name = "ProgrammesChanged";
        break;
    case UVAEvent::BufferingProgress:
        ev_name = "BufferingProgress";
        break;
    case UVAEvent::BufferingNetworkError:
        ev_name = "BufferingNetworkError";
        break;
    case UVAEvent::BufferingIdle:
        ev_name = "BufferingIdle";
        break;
    case UVAEvent::BufferingStreamUnderrun:
        ev_name = "BufferingStreamUnderrun";
        break;
    case UVAEvent::BufferingStreamBuffered:
        ev_name = "BufferingStreamBuffered";
        break;
    case UVAEvent::ComponentChanged:
        ev_name = "ComponentChanged";
        break;
    }
    return ev_name;
}

char const *
toString(nebula::CicamPlayerStatus status)
{
    switch (status)
    {
    case nebula::CicamPlayerStatus::content_play_is_not_possible:
        return "content_play_is_not_possible";
    case nebula::CicamPlayerStatus::unrecoverable_error:
        return "unrecoverable_error";
    case nebula::CicamPlayerStatus::content_blocked:
        return "content_blocked";
    }
    return "unknown";
}

std::stringstream &
operator<<(std::stringstream & os, UVABackend::TimeRange const & range)
{
    os << "(" << range.first << ", " << range.second << ")";
    return os;
}

std::string
toString(std::vector<UVABackend::TimeRange> const & time_ranges)
{
    std::stringstream os;
    for (auto const & range : time_ranges)
    {
        os << range << " ";
    }
    return os.str();
}

std::string
toString(UVABackend::TimeRange const * const * time_ranges)
{
    std::stringstream os;
    while (*time_ranges)
    {
        auto & range = **time_ranges;
        os << range << " ";
        ++time_ranges;
    }
    return os.str();
}

#endif

} // namespace

///////////////////////////////////////////////////////////////////////////////

class OperaMuminCicamPlayerBackend::InfoRequestCommand : public AnyCommand
{
public:
    InfoRequestCommand(OperaMuminCicamPlayerBackend & target);
    void execute() override;

private:
    InfoRequestCommand& operator=(const InfoRequestCommand&) = delete;

    OperaMuminCicamPlayerBackend & m_target;
};

OperaMuminCicamPlayerBackend::InfoRequestCommand::InfoRequestCommand(
        OperaMuminCicamPlayerBackend & target)
    : m_target(target)
{
}

void
OperaMuminCicamPlayerBackend::InfoRequestCommand::execute()
{
    m_target.processInfoRequest();
}

///////////////////////////////////////////////////////////////////////////////

OperaMuminCicamPlayerBackend::OperaMuminCicamPlayerBackend(
    UVABackendType backend_type,
    UVAClient* client,
    nebula::AnyCicamPlayer & cicam_player,
    AnyCommandThread & thread)
    : UVAMediaBackendBase(backend_type)
    , m_client(client)
    , m_cicam_player(cicam_player)
    , m_thread(thread)
    , m_init_ok(false)
    , m_buffering(false)
    , m_duration(unknown_duration)
    , m_position(unknown_position)
    , m_position_timestamp_ms(0)
    , m_speed(0)
    , m_info_poll_timer(nullptr)
    // Hard-coded until the required support is implemented.
    , m_video_width(1280)
    , m_video_height(720)
    , m_video_pixel_aspect(1.0)
    , m_can_play_semaphore(UVA::SignalMutex::make())
    , m_can_play_status(false)
{
    TRACE_INFO(("OperaMuminCicamPlayerBackend(backend_type = %d, client = %p\n",
                backend_type, client));

    m_init_ok = m_state_mutex.initialise() &&
                m_can_play_semaphore &&
                m_cicam_player.publisher().registerListener(*this);
}

OperaMuminCicamPlayerBackend::~OperaMuminCicamPlayerBackend()
{
    TRACE_INFO(("~OperaMuminCicamPlayerBackend()\n"));

    FrostMutexLock lock(m_state_mutex);

    cancelInfoPollTimerLocked();
    m_cicam_player.publisher().deregisterListener(*this);
    
    // The UVA documentation (and experimentation) suggests that the only way
    // to specify content is via initBackend() (attachSource() is only for MSE)
    // and that this will only be called once per backend lifetime.
    // Therefore it is only necessary to stop the player session in the
    // destructor.
    //
    // Since we have deregistered, the subsequent stopped() will not be
    // received (and there is nothing we can do with it at the moment anyway).
    m_cicam_player.stop();
}

UVA_STATUS
OperaMuminCicamPlayerBackend::initBackend(
    const char *origin,
    UVABackend::SourceDescription **sources,
    bool automatic_key_system_selection)
{
    TRACE_INFO(("initBackend(origin = %s, ..., automatic_key_system_selection = %d)\n",
                origin, automatic_key_system_selection));
    TRACE_ONLY_PARAM(origin);
    TRACE_ONLY_PARAM(automatic_key_system_selection);

    if (!m_init_ok)
    {
        return UVA_NOT_INITIALIZED;
    }

    FrostMutexLock lock(m_state_mutex);
    UVA_STATUS status = UVA_OK;
    UVABackend::SourceDescription ** source_ptr = sources;
    while (*source_ptr)
    {
        UVABackend::SourceDescription & source = **source_ptr;
        TRACE_INFO(("initBackend(): source: type = %d, source_id = %lu, mime_type = %s, "
                    "is_encrypted = %d\n",
                    source.type, source.source_id,
                    (source.mime_type != nullptr) ? source.mime_type : "(unknown)",
                    source.is_encrypted));
        switch (source.type)
        {
        case SourceDescription::CICAM_STREAMING:
            TRACE_INFO(("initBackend(): CICAM_STREAMING: url = %s, content_type = %s)\n",
                        source.description.cicam_streaming.url,
                        source.description.cicam_streaming.content_type));
            if (!m_source)
            {
                m_source.reset(new UVABackend::SourceDescription(std::move(source)));
            }
            else
            {
                // Currently only handle a single source, flag error if multiple sources.
                TRACE_ERROR(("OperaMuminCicamPlayerBackend::initBackend: "
                             "Multiple source descriptions not supported\n"));
                SHOULD_NOT_BE_HERE();
            }
            break;
        default:
            status = UVA_NOT_SUPPORTED;
            break;
        }
        ++source_ptr;
    }
    return status;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setVideoWindowAndClip(const Rect& window_rect,
                                                    const Rect& clip_rect)
{
    TRACE_INFO(("setVideoWindowAndClip(...)\n"));
    CABOT_UNUSED_PARAM(window_rect);
    CABOT_UNUSED_PARAM(clip_rect);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getSeekableTimeRanges(
        ByteRange** input_byte_ranges,
        TimeRange**& output_time_ranges)
{
    TRACE_VERBOSE(("getSeekableTimeRanges(...)\n"));
    CABOT_UNUSED_PARAM(input_byte_ranges);
    
    FrostMutexLock lock(m_state_mutex);
    UVA_STATUS status = UVA_ERROR;
    TimeRange** time_ranges = nullptr;
    double duration;
    if (getDurationLocked(duration) == UVA_OK)
    {
        // Set the seekable attribute to the full duration of the content
        // based on the duration attribute.
        // See ETSI TS 102 796 section K.4.
        time_ranges = new TimeRange*[2];
        time_ranges[0] = new TimeRange(0.0, duration);
        time_ranges[1] = nullptr;
    }
    else
    {
        // Some common code, but arguably overcomplicated if refactored.
        time_ranges = new TimeRange*[1];
        time_ranges[0] = nullptr;
    }
    if (time_ranges)
    {
        output_time_ranges = time_ranges;
        status = UVA_OK;
        TRACE_VERBOSE(("getSeekableTimeRanges(): %s\n",
                       toString(output_time_ranges).c_str()));
    }
    return status;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getBufferedTimeRanges(
        uintptr_t source_id,
        const std::vector<ByteRange>& input_byte_ranges,
        std::vector<TimeRange>& output_time_ranges)
{
    TRACE_VERBOSE(("getBufferedTimeRanges(source_id = %lu, ...)\n", source_id));
    CABOT_UNUSED_PARAM(input_byte_ranges);
    
    FrostMutexLock lock(m_state_mutex);
    UVA_STATUS status = UVA_ERROR;
    if (source_id == m_source->source_id)
    {
        output_time_ranges.clear();
        double duration;
        if (getDurationLocked(duration) == UVA_OK)
        {
            // Set the buffered attribute to the full duration of the content
            // based on the duration attribute.
            // See ETSI TS 102 796 section K.4.
            output_time_ranges.push_back(TimeRange(0.0, duration));
            status = UVA_OK;
        }
        else
        {
            // Set the buffered attribute representing 5 seconds either side of
            // the current playback position.
            // See ETSI TS 102 796 section K.4.
            double position;
            if (getPositionLocked(position) == UVA_OK)
            {
                // Clamp the minimum to avoid negative time values.
                output_time_ranges.push_back(
                        TimeRange(std::max(0.0, position - 5.0), position + 5.0));
                status = UVA_OK;
            }
        }
        TRACE_VERBOSE(("getBufferedTimeRanges(): %s\n",
                       toString(output_time_ranges).c_str()));
    }
    else
    {
        TRACE_ERROR(("getBufferedTimeRanges(): unknown source\n"));
    }
    return status;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::canPlaySource(const SourceDescription& source)
{
    TRACE_INFO(("canPlaySource(type = %d, source_id = %lu, mime_type = %s, "
                "is_encrypted = %d, ...)\n",
                source.type, source.source_id,
                (source.mime_type != nullptr) ? source.mime_type : "(unknown)",
                source.is_encrypted));

    UVA_STATUS status = UVA_NOT_SUPPORTED;
    bool const supported = (source.type == SourceDescription::CICAM_STREAMING);
    if (m_init_ok && supported)
    {
        TRACE_INFO(("canPlaySource(..., url = %s, content_type = %s)\n",
                    source.description.cicam_streaming.url,
                    source.description.cicam_streaming.content_type));

        m_can_play_status = false;

        m_cicam_player.verify(serviceLocationtoXml(&source));
        TRACE_VERBOSE(("canPlaySource: Waiting for CICAM Response...\n"));
        const double reply_timeout_seconds = 20.0; // Arbitrary timeout
        if (m_can_play_semaphore &&
            m_can_play_semaphore->timedWait(reply_timeout_seconds))
        {
            // The robotester test cicam_player_hbbtv_verify_ok.test.py
            // relies on this trace. Change with care.
            TRACE_INFO(("canPlaySource returns %s\n",
                        m_can_play_status ? "UVA_OK" : "UVA_NOT_SUPPORTED"));

            status = m_can_play_status ? UVA_OK : UVA_NOT_SUPPORTED;
        }
        else
        {
            TRACE_WARN(("canPlaySource: Timeout waiting for CICAM Response\n"));
        }
    }
    return status;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::load()
{
    TRACE_INFO(("load()\n"));
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::play()
{
    TRACE_INFO(("play()\n"));

    FrostMutexLock lock(m_state_mutex);
    bool const ok = startBufferingLocked();
    
    // TODO: Will need to send a CICAM_player_control_req APDU with the
    //       speed set to the correct playback rate (or some other method
    //       to allow playback to start after pause()) if a
    //       suitable playing() callback has been received.

    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::suspend()
{
    TRACE_INFO(("suspend()\n"));
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::pause()
{
    TRACE_INFO(("pause()\n"));

    FrostMutexLock lock(m_state_mutex);
    bool const ok = startBufferingLocked();
    
    // TODO: Will need to send a CICAM_player_control_req APDU with the
    //       speed set to 0 (or use some other method to ensure playback
    //       does not start) until play() is called.
    
    return ok ? UVA_OK : UVA_ERROR;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setPlaybackRate(double rate)
{
    TRACE_INFO(("setPlaybackRate(rate = %g)\n", rate));
    TRACE_ONLY_PARAM(rate);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setVolume(double volume)
{
    TRACE_INFO(("setVolume(volume = %g)\n", volume));
    TRACE_ONLY_PARAM(volume);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getDuration(double &duration)
{
    FrostMutexLock lock(m_state_mutex);
    return getDurationLocked(duration);
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getDurationLocked(double &duration)
{
    // Note: Expects m_state_mutex to be locked.
    
    UVA_STATUS status = UVA_ERROR;
    if (m_duration != unknown_duration)
    {
        duration = m_duration;
        status = UVA_OK;
        TRACE_VERBOSE(("getDuration(): %g\n", duration));
    }
    else
    {
        status = UVA_NOT_INITIALIZED;
        TRACE_VERBOSE(("getDuration(): unknown\n"));
    }
    return status;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setContentSize(uintptr_t source_id,
                                             uva_uint64 content_size)
{
    TRACE_INFO(("setContentSize(source_id = %lu, content_size = %llu)\n",
                source_id, static_cast<unsigned long long int>(content_size)));
    TRACE_ONLY_PARAM(source_id);
    TRACE_ONLY_PARAM(content_size);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getPosition(double &position)
{
    FrostMutexLock lock(m_state_mutex);
    return getPositionLocked(position);
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getPositionLocked(double &position)
{
    // Note: Expects m_state_mutex to be locked.
    
    bool const position_known = (m_position != unknown_position);
    if (position_known)
    {
        // Calculate the position based on the play speed and the time the
        // position was received from the CICAM.
        std::uint32_t const now_ms = FROST_Time();
        std::uint32_t const elapsed_ms = FROST_TimeSubtract(now_ms, m_position_timestamp_ms);
        double const position_delta_ms = (elapsed_ms * m_speed) / nominal_play_speed;
        position = m_position + (position_delta_ms / milliseconds_per_second);

        // Clamp the position to a known duration.
        double duration;
        if (getDurationLocked(duration) == UVA_OK)
        {
            position = std::min(position, duration);
        }
    }
    else
    {
        position = 0.0;
    }
    TRACE_VERBOSE(("getPosition(): %g\n", position));
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setPosition(double position)
{
    TRACE_INFO(("setPosition(position = %g)\n", position));
    TRACE_ONLY_PARAM(position);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setVisibility(bool visible)
{
    TRACE_INFO(("setVisibility(visible = %d)\n", visible));
    TRACE_ONLY_PARAM(visible);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::canPlayType(const ContentDescription& desc)
{
    TRACE_INFO(("canPlayType(...)\n"));
    CABOT_UNUSED_PARAM(desc);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setFullScreen(bool fullscreen)
{
    TRACE_INFO(("setFullScreen(fullscreen = %d)\n", fullscreen));
    TRACE_ONLY_PARAM(fullscreen);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getSupportedMimetypes(char**& mime_types)
{
    TRACE_INFO(("getSupportedMimetypes(...)\n"));
    CABOT_UNUSED_PARAM(mime_types);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setLoop(int value)
{
    TRACE_INFO(("setLoop(value = %d)\n", value));
    TRACE_ONLY_PARAM(value);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::selectComponent(AVComponent* component)
{
    TRACE_INFO(("selectComponent(...)\n"));
    CABOT_UNUSED_PARAM(component);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::unselectComponent(AVComponent* component)
{
    TRACE_INFO(("unselectComponent(...)\n"));
    CABOT_UNUSED_PARAM(component);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::getPlaybackSpeedRanges(
    std::vector<SpeedRange>& speed_ranges)
{
    TRACE_INFO(("getPlaybackSpeedRanges()\n"));
    CABOT_UNUSED_PARAM(speed_ranges);
    // Not yet implemented.
    return UVA_OK;
}

UVA_STATUS
OperaMuminCicamPlayerBackend::setWindowId(uintptr_t window_id)
{
    TRACE_INFO(("setWindowId(window_id = %lu)\n", window_id));
    TRACE_ONLY_PARAM(window_id);
    // Not yet implemented.
    return UVA_OK;
}

void
OperaMuminCicamPlayerBackend::eventCompleted(const UVAAsyncEvent* event,
                                             bool success)
{
    TRACE_INFO(("eventCompleted(%s, %d)\n",
                toString(event->GetType()), success));
    switch (event->GetType())
    {
    case UVAEvent::BackendResourceRequestAsync:
        if (success)
        {
            TRACE_INFO(("Acquired scarce resources\n"));
        }
        else
        {
            TRACE_INFO(("Failed to acquire scarce resources\n"));
            // Generate an error as per 'Acquisition of scarce resources' in
            // the UVA API documentation.
            sendEvent(new DecodeErrorEvent(
                        DecodeErrorEvent::INSUFFICIENT_RESOURCES));
        }
        break;
    }
}

bool
OperaMuminCicamPlayerBackend::sendEvent(UVAEvent* ev)
{
    bool result = false;
    UVAEvent::UVAEventType type; 
    cabot::String ev_name;
    if (m_client && ev)
    {
        type = ev->GetType();
        result = m_client->handleEvent(ev) == UVA_OK;

        if (!result)
        {
            TRACE_ERROR(("Failed to send event (%s) to UVA client\n",
                         toString(type)));
        }
        else
        {
            TRACE_INFO(("UVA event (%s) sent for CICAM player backend\n",
                        toString(type)));
        }
    }
    return result;
}

bool
OperaMuminCicamPlayerBackend::startBufferingLocked()
{
    // Note: Expects m_state_mutex to be locked.
    
    bool success = false;
    if (!m_buffering && m_source)
    {
        TRACE_INFO(("startBuffering()\n"));
        
        // This corresponds roughly to steps 1-4 of 'Starting buffering and
        // playback' in the UVA API documentation.
        success = sendEvent(new BufferingStreamUnderrunEvent(m_source->source_id));
        success = sendEvent(new BackendResourceRequestAsyncEvent()) && success;

        // Issue a CICAM_player_play_req APDU to the CICAM so it can start
        // the data transfer.
        m_cicam_player.play(serviceLocationtoXml(m_source.get()));

        m_buffering = true;
    }
    return success;
}

bool
OperaMuminCicamPlayerBackend::sendNewMetadataEventLocked()
{
    // Note: Expects m_state_mutex to be locked.
    
    // TODO: Temporary event to simulate playback - only send once video
    //       attributes are known.
    
    // This event should only be sent once video attributes are known.
    // If the duration is not known, an infinite duration can be given or
    // sending of the event postponed.
    double const duration = (m_duration != unknown_duration) ?
                                    m_duration :
                                    std::numeric_limits<double>::infinity();
    return sendEvent(new NewMetadataEvent(duration,
                                          m_video_width,
                                          m_video_height,
                                          m_video_pixel_aspect));
}

void
OperaMuminCicamPlayerBackend::requestInfo()
{
    TRACE_INFO(("requestInfo()\n"));
    
    m_thread.enqueueCommand(new InfoRequestCommand(*this));
}

void
OperaMuminCicamPlayerBackend::processInfoRequest()
{
    TRACE_INFO(("processInfoRequest()\n"));
    
    // Request player information from the CICAM.
    m_cicam_player.info();
}

void
OperaMuminCicamPlayerBackend::setInfoPollTimerLocked()
{
    // Note: Expects m_state_mutex to be locked.
    
    cancelInfoPollTimerLocked();
    CABOT_ASSERT(m_info_poll_timer == nullptr);
    m_info_poll_timer = cabot::setTimer(*this, info_request_interval_ms,
                                               info_request_timer_id);
}

void
OperaMuminCicamPlayerBackend::cancelInfoPollTimerLocked()
{
    // Note: Expects m_state_mutex to be locked.
    
    cabot::cancelTimer(m_info_poll_timer);
    m_info_poll_timer = nullptr;
}

void
OperaMuminCicamPlayerBackend::playing(bool network_active)
{
    TRACE_INFO(("playing(network_active = %d)\n", network_active));
    
    bool success = true;
    if (!network_active)
    {
        // It is expected that network_active will not be set when the
        // input_max_bitrate is not zero and no data has been received.
        // Sending this event ensures networkState is set to NETWORK_IDLE.
        // See ETSI TS 102 796 section K.4 and 'Obtaining data for playback'
        // in the UVA API documentation.
        success = sendEvent(new BufferingIdleEvent()) && success;
    }
    else
    {
        // It is expected that network_active will be set when the
        // input_max_bitrate is zero, or non-zero and data has been received.
        // Sending this event ensures networkState is set to NETWORK_LOADING.
        // See ETSI TS 102 796 section K.4 and 'Obtaining data for playback'
        // in the UVA API documentation.
        success = sendEvent(new BufferingProgressEvent()) && success;

        // The following corresponds roughly to steps 5-7 of 'Starting buffering
        // and playback' in the UVA documentation.  Assume playback can be
        // started at this time.
        success = sendEvent(new BufferingStreamBufferedEvent()) && success;

        // Request player information from the CICAM.
        requestInfo();
        
        // The following corresponds roughly to step 8 of 'Starting buffering
        // and playback' in the UVA documentation.
        // TODO: Will need to send a CICAM_player_control_req APDU with the
        //       speed set to the correct playback rate if play() (rather than
        //       pause()) was called previously.
    }
    if (!success)
    {
        TRACE_ERROR(("playing(): failed to notify UVA client\n"));
    }
}

void
OperaMuminCicamPlayerBackend::info(std::uint32_t duration,
                                   std::uint32_t position,
                                   std::int16_t speed)
{
    TRACE_INFO(("info(duration = %u, position = %u, speed = %d/%d)\n",
                duration, position, speed, nominal_play_speed));
    
    FrostMutexLock lock(m_state_mutex);
    
    bool const position_known = (position != unknown_position);
    if (position_known)
    {
        m_position = position;
        m_position_timestamp_ms = FROST_Time();
    }
    else
    {
        // What should happen if the position was known, but now is not?
        if (m_position != unknown_position)
        {
            TRACE_WARN(("info(): position reported as unknown "
                        "when previously known (%u)", m_position));
        }
    }
    
    bool const duration_known = (duration != unknown_duration);
    if (duration_known)
    {
        if (m_duration == unknown_duration || m_duration != duration)
        {
            m_duration = duration;
            sendNewMetadataEventLocked();
        }
    }
    else
    {
        // What should happen if the duration was known, but now is not?
        if (m_duration != unknown_duration)
        {
            TRACE_WARN(("info(): duration reported as unknown "
                        "when previously known (%u)", m_duration));
        }
    }

    // It is not possible to influence the playbackRate attribute via the UVA
    // API.  Store the actual speed so it can be used for position calculations.
    // See ETSI TS 102 796 section K.4, K.8.
    
    m_speed = speed;
    
    // Set up a timer to poll for an update.
    setInfoPollTimerLocked();
}

void
OperaMuminCicamPlayerBackend::stopped()
{
    TRACE_INFO(("stopped()\n"));

    FrostMutexLock lock(m_state_mutex);
    if (m_buffering)
    {
        // Force playback to be stopped.
        // Note: It is not clear from specifications or UVA documentation what
        //       should be done here, but this is a reasonable guess.
        //       An alternative is to send DecodeErrorEvent::CONTENT_CORRUPTED
        //       (which also causes this backend to be destroyed and the 'error'
        //       attribute to be set).
        sendEvent(new BackendResourceRevokedEvent());

        // Reset the backend state.
        cancelInfoPollTimerLocked();
        m_buffering = false;
    }
}

void
OperaMuminCicamPlayerBackend::supported(bool is_supported)
{
    TRACE_INFO(("supported(%s)\n", is_supported ? "yes" : "no"));
    m_can_play_status = is_supported;
    if (m_can_play_semaphore)
    {
        // Signal the semaphore to allow the blocking request thread to continue
        m_can_play_semaphore->signal();
    }
}

void
OperaMuminCicamPlayerBackend::error(nebula::CicamPlayerStatus status)
{
    TRACE_INFO(("error(%d: %s)\n", static_cast<int>(status), toString(status)));
    
    // Mapping as defined by ETSI TS 102 796 section K.7.
    // TODO: The error attribute is only updated when an error occurs after
    //       playback starts (on UVA client receipt of BufferingStreamBufferedEvent).
    //       May need to find a solution, but none found so far.
    switch (status)
    {
    case nebula::CicamPlayerStatus::content_play_is_not_possible:
    case nebula::CicamPlayerStatus::unrecoverable_error:
        // Should generate a MEDIA_ERR_SRC_NOT_SUPPORTED error.
        // TODO: This is as advised by Vewd, but it actually generates a
        //       MEDIA_ERR_NETWORK error.  Need to find a solution, but none
        //       found so far.
        sendEvent(new BufferingNetworkErrorEvent(
                BufferingNetworkErrorEvent::CONNECTION_LOST_OR_IMPOSSIBLE));
        break;
    case nebula::CicamPlayerStatus::content_blocked:
        // Should generate a MEDIA_ERR_DECODE error.
        sendEvent(new DecodeErrorEvent(
                    DecodeErrorEvent::FORMAT_NOT_SUPPORTED));
        break;
    default:
        TRACE_ERROR(("error(): unsupported error\n"));
        SHOULD_NOT_BE_HERE();
        break;
    }
}

void
OperaMuminCicamPlayerBackend::timeout(int /*id*/)
{
    TRACE_INFO(("timeout()\n"));

    requestInfo();
}
