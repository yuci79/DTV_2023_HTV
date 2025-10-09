/**
 * Copyright 2018 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the class OperaMuminCicamPlayerBackend.
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCICAMPLAYERBACKEND_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCICAMPLAYERBACKEND_HPP_INCLUDED

#include "nebula/adaptation_layer/AnyCicamPlayerListener.hpp"
#include "utilities_public/AnyTimeoutHandler.hpp"
#include "utilities_public/FrostMutex.hpp"
#include "uva_backend.h"

#include <memory>

class AnyCommandThread;
class OperaMuminObjectFactory;

namespace nebula
{
class AnyCicamPlayer;
}

namespace cabot
{
struct TimerHandle;
}

/**
 * @brief UVA Media backend for CICAM Player support.
 * @note Assumes SDK 4.11 support.
 */
class OperaMuminCicamPlayerBackend : public UVAMediaBackendBase,
                                     public nebula::AnyCicamPlayerListener,
                                     public AnyTimeoutHandler
{
public:
    OperaMuminCicamPlayerBackend(UVABackendType type,
                                 UVAClient* _client,
                                 nebula::AnyCicamPlayer & cicam_player,
                                 AnyCommandThread & thread);
    ~OperaMuminCicamPlayerBackend() override;

public: // Implementing functions from UVAMediaBackend.
    UVA_STATUS initBackend(const char* origin,
                           UVABackend::SourceDescription** sources,
                           bool automatic_key_system_selection) override;
    UVA_STATUS setVideoWindowAndClip(const Rect& window_rect,
                                    const Rect& clip_rect) override;
    UVA_STATUS getSeekableTimeRanges(ByteRange** input_byte_ranges,
                                     TimeRange**& output_time_ranges) override;
    UVA_STATUS getBufferedTimeRanges(
                        uintptr_t source_id,
                        const std::vector<ByteRange>& input_byte_ranges,
                        std::vector<TimeRange>& output_time_ranges) override;
    UVA_STATUS canPlaySource(const SourceDescription& source) override;
    UVA_STATUS load() override;
    UVA_STATUS play() override;
    UVA_STATUS suspend() override;
    UVA_STATUS pause() override;
    UVA_STATUS setPlaybackRate(double rate) override;
    UVA_STATUS setVolume(double volume) override;
    UVA_STATUS getDuration(double &duration) override;
    UVA_STATUS setContentSize(uintptr_t source_id,
                              uva_uint64 content_size) override;
    UVA_STATUS getPosition(double &position) override;
    UVA_STATUS setPosition(double position) override;
    UVA_STATUS setVisibility(bool visible) override;
    UVA_STATUS canPlayType(const ContentDescription& desc) override;
    UVA_STATUS setFullScreen(bool fullscreen) override;
    UVA_STATUS getSupportedMimetypes(char**& mime_types) override;
    UVA_STATUS setLoop(int value) override;
    UVA_STATUS selectComponent(AVComponent* component) override;
    UVA_STATUS unselectComponent(AVComponent* component) override;
    UVA_STATUS getPlaybackSpeedRanges(
                        std::vector<SpeedRange>& speed_ranges) override;

public: // Implementing functions from UVABackend.
    UVA_STATUS setWindowId(uintptr_t window_id) override;
    void eventCompleted(const UVAAsyncEvent* event, bool success) override;

public: // Implementing AnyCicamPlayerListener.
    void playing(bool network_active) override;
    void info(std::uint32_t duration,
              std::uint32_t position,
              std::int16_t speed) override;
    void stopped() override;
    void supported(bool is_supported) override;
    void error(nebula::CicamPlayerStatus status) override;

public: // Implementing AnyTimeoutHandler.
    void timeout(int id) override;

private: // Copy constructor & assignment operations disallowed.
    OperaMuminCicamPlayerBackend(OperaMuminCicamPlayerBackend const &) = delete;
    OperaMuminCicamPlayerBackend& operator=(OperaMuminCicamPlayerBackend const &) = delete;

private:
    class InfoRequestCommand;
    void processInfoRequest();

    bool sendEvent(UVAEvent* ev);
    UVA_STATUS getDurationLocked(double &duration);
    UVA_STATUS getPositionLocked(double &position);
    bool startBufferingLocked();
    bool sendNewMetadataEventLocked();
    void requestInfo();
    void setInfoPollTimerLocked();
    void cancelInfoPollTimerLocked();

private:
    UVAClient* m_client;
    nebula::AnyCicamPlayer & m_cicam_player;
    AnyCommandThread & m_thread;
    bool m_init_ok;
    
    // Protect access to the following block of state.
    FrostMutex m_state_mutex;
    bool m_buffering;
    std::uint32_t m_duration;
    std::uint32_t m_position;
    std::uint32_t m_position_timestamp_ms;
    std::int16_t m_speed;
    cabot::TimerHandle * m_info_poll_timer;
    int m_video_width;
    int m_video_height;
    double m_video_pixel_aspect;
    std::unique_ptr<UVABackend::SourceDescription> m_source;
    
    std::unique_ptr<UVA::SignalMutex> m_can_play_semaphore;
    bool m_can_play_status;
};

#endif /* CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCICAMPLAYERBACKEND_HPP_INCLUDED */
