/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMuminVideoBroadcastBackend
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINVIDEOBROADCASTBACKEND_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINVIDEOBROADCASTBACKEND_HPP_INCLUDED

#include "config/using_sdk422.h"
#include "config/using_sdk423.h"

#include <memory>
#include "uva_backend.h"
#include "uva_programme_collection.h"

#include "nebula/core/browser_client/AnyVideoBroadcastObject.hpp"
#include "nebula/core/browser_client/AnyVbObjectEventGenerator.hpp"
#include "nebula/browser_interface/AnyWebBrowserOpAppEventListener.hpp"
#include "utilities_public/String.hpp"
#include "utilities_public/FrostMutex.hpp"

class OperaMuminAVComponentHelper;
class OperaMuminObjectFactory;
class AnyVideoBroadcastObjectTypeConverter;

class OperaMuminVideoBroadcastBackend : public UVAVideoBroadcastBackend
                                      , public AnyVbObjectEventGenerator
                                      , public AnyWebBrowserOpAppEventListener
{
public:
    OperaMuminVideoBroadcastBackend(UVAClient* client,
                                    OperaMuminObjectFactory& object_factory,
                                    AnyVideoBroadcastObjectTypeConverter& vbo_type_converter);
    ~OperaMuminVideoBroadcastBackend() override;

public:
    // Implementing UVAVideoBroadcastBackend.
    UVA_STATUS setWindowId(uintptr_t window_id) override;
    UVA_STATUS initBackend(const char* origin) override;

    UVA_STATUS setBackendMode(BackendMode mode) override;

    UVA_STATUS suspend() override;
    UVA_STATUS setVideoWindowAndClip(const Rect& window_rect, const Rect& clip_rect) override;
    UVA_STATUS setFullScreen(bool fullscreen) override;
    UVA_STATUS setVisibility(bool visible) override;
    UVA_STATUS setVolume(int volume) override;
    UVA_STATUS nextChannel() override;
    UVA_STATUS previousChannel() override;
    UVA_STATUS stop() override;            // HBBTV specific

    UVA_STATUS selectComponent(UVABackend::AVComponent* components) override;
    UVA_STATUS unselectComponent(UVABackend::AVComponent* components) override;

    UVA_STATUS getPlayState(UVAVideoBroadcastBackend::PlayState &play_state) override;
    UVA_STATUS getProgrammes(std::shared_ptr<UVAProgrammeCollection>& programmes_value) override;
    UVA_STATUS getCurrentChannel(UVAValueStatus* value_status,
                        std::shared_ptr<UVAChannel>& current_channel_value) override;
#ifdef USING_SDK423
    UVA_STATUS getCurrentServiceInstance(
                        UVAValueStatus* current_service_instance_value_status,
                        std::shared_ptr<UVAChannel>& current_service_instance_value) override;
#endif
    UVA_STATUS createChannelObject(
                       int idType,
                       const std::string& dsd,
                       int sid, UVAValueStatus* return_value_status,
                       std::shared_ptr<UVAChannel>& return_value) override;
    UVA_STATUS createChannelObject(
                       int idType, UVAValueStatus onid_value_status,
                       int onid, UVAValueStatus tsid_value_status,
                       int tsid, UVAValueStatus sid_value_status,
                       int sid, UVAValueStatus sourceID_value_status,
                       int sourceID, UVAValueStatus ip_broadcast_id_value_status,
                       const std::string& ip_broadcast_id,
                       UVAValueStatus* return_value_status,
                       std::shared_ptr<UVAChannel>& return_value) override;
    UVA_STATUS setChannel(UVAValueStatus channel_value_status,
                       const std::shared_ptr<UVAChannel>& channel,
                       UVAValueStatus trickplay_value_status,
                       bool trickplay, UVAValueStatus content_access_descriptor_url_value_status,
                       const std::string& content_access_descriptor_url,
                       UVAValueStatus offset_value_status, int offset,
                       UVAValueStatus quiet_value_status, int quiet_value) override;
    UVA_STATUS getChannelConfig(UVAValueStatus* return_value_status,
                       std::shared_ptr<UVAChannelConfig>& return_value) override;
#if defined USING_SDK422 || defined USING_SDK423
    UVA_STATUS bindToCurrentChannel(UVAValueStatus* bound_channel_value_status,
                                    std::shared_ptr<UVAChannel>& bound_channel_value) override;
#else
    UVA_STATUS bindToCurrentChannel() override;
#endif
    UVA_STATUS getPlaybackOffset(double &offset) override;
    UVA_STATUS getMaxOffset(double &offset) override;
    UVA_STATUS getPlaybackSpeeds(double*& speeds, size_t& num_speeds) override;
    UVA_STATUS recordNow(double duration, char* &identifier) override;
    UVA_STATUS stopRecording() override;
    UVA_STATUS pause() override;
    UVA_STATUS resume() override;
    UVA_STATUS setPlaybackSpeed(double speed) override;
    UVA_STATUS setPosition(double offset, PositionReference reference) override;
#ifdef USING_SDK423
    UVA_STATUS getTimeShiftMode(int& mode) override;
    UVA_STATUS getCurrentTimeShiftMode(int& mode) override;
#endif
    UVA_STATUS enableTimeline(const std::string& timeline_selector) override;
    UVA_STATUS disableTimeline(const std::string& timeline_selector) override;
    UVATime getPosition(const std::string& timeline_selector) override;
    UVATimelineDetails getTimelineDetails(const std::string& timeline_selector) override; 
    UVA_STATUS getContentId(std::string& content_id, 
                            UVAContentIdStatus& content_id_status) override;
    UVA_STATUS getMrsUrl(std::string& mrs_url) override;
    UVA_STATUS stopTimeShift() override;
    UVA_STATUS getRecordingState(RecordingState &state) override;
    UVA_STATUS getPlaybackSpeed(double &speed) override;
    UVA_STATUS getPlayerCapabilities(std::shared_ptr<UVAStringCollection>& capabilities) override;
    UVA_STATUS createAVAudioComponent(UVABackend::AVComponent** component,
                               int component_tag,
                               const std::string &language,
                               bool audio_description,
                               int channels,
                               std::string const& encoding) override;
    UVA_STATUS createAVVideoComponent(UVABackend::AVComponent** component,
                               int component_tag,
                               double aspect_ratio,
                               std::string const& encoding) override;

    UVA_STATUS getPlayStateError(UVAValueStatus* play_state_error_status,
                                 ChannelError& play_state_error) override;

    UVA_STATUS blockAv() override;
    UVA_STATUS unblockAv() override;

public: //Implements AnyVbObjectEventGenerator
    bool generateChannelChangeError(UString& ccid, NEBULA_ChannelChangeResult error) override;
    bool generateChannelChangeSucceeded(UString& ccid) override;
    bool generatePlayStateChanged(nebula::PlayState state,
                                  NEBULA_ChannelChangeResult error) override;
    bool generateProgrammesChanged() override;
    bool generateStreamEvent(void* listener,
                            const cabot::String& url,
                            const cabot::String& name,
                            const cabot::String& data,
                            const UString& text,
                            StreamEventStatus status) override;
    bool generateFullScreenChanged() override;
    void generateComponentActivenessChangedEvent(const NEBULA_AVComponent *component_data, 
                                                 bool active) override;
    bool generateRecordingEvent(int rec_state, int rec_error, UString rec_id) override;
    bool generatePlaySpeedChangedEvent(int speed) override;
    bool generatePlayPositionChangedEvent(int position) override;
    void generateComponentsChangedEvent() override;
    bool generateDrmRightsErrorEvent(
            int error_state, const std::string& content_id,
            const std::string& drm_system_id, const std::string& rights_issuer_url) override;
    bool generateParentalRatingChangeEvent(
            const std::string& pr_name, const std::string& pr_scheme,
            int pr_value, const std::string& pr_region,
            const std::string& content_id, const std::string& drm_system_id,
            bool blocked) override;
    bool generateParentalRatingErrorEvent(
            const std::string& pr_name, const std::string& pr_scheme,
            int pr_value, const std::string& pr_region,
            const std::string& content_id, const std::string& drm_system_id) override;
    bool generateEvent(int handle, AnyVbObjectEventGenerator::Event event) override;

public: // Implementing AnyWebBrowserOpAppEventListener interface.
    void notifyOpAppStartResult(std::uint16_t application_id,
                                std::uint32_t organisation_id,
                                bool started) override;
    void notifyOpAppStopped() override;
    void notifyOpAppStateChanged(
                        nebula::OperatorApplicationState old_state,
                        nebula::OperatorApplicationState new_state) override;
    void notifyOpAppDiscovered(std::uint16_t application_id,
                               std::uint32_t organisation_id,
                               std::uint32_t application_version) override;
    void notifyOpAppNotDiscovered() override;
    void notifyOpAppUninstalled(std::uint16_t application_id,
                                std::uint32_t organisation_id) override;
    void notifyOpAppSuspended() override;
    void notifyOpAppResumed() override;
    void registerAsOpAppListener();
    void deregisterAsOpAppListener();

private:
    UVA_STATUS selectTimeline(const std::string& timeline_selector, bool enable);
    bool generateContentIdChanged() const;
    bool generateMrsUrlChanged() const;

    UVA_STATUS internalBindToCurrentChannel(UVAValueStatus* bound_channel_value_status,
                                            std::shared_ptr<UVAChannel>& bound_channel_value);
                                            
protected:
    AnyVideoBroadcastObject* m_vb_object;
    UVAClient* m_client;

private:
    // copy constructor & assignment operations disallowed
    OperaMuminVideoBroadcastBackend(OperaMuminVideoBroadcastBackend const &);
    OperaMuminVideoBroadcastBackend& operator=(OperaMuminVideoBroadcastBackend const &);

    UVA_STATUS applyVideoWindowSettings(const Rect& window_rect, const Rect& clip_rect);
    BackendMode m_backend_mode;

    UString m_origin;
    OperaMuminAVComponentHelper* m_component_helper;

    OperaMuminObjectFactory& m_object_factory;
    AnyVideoBroadcastObjectTypeConverter& m_vbo_type_converter;

    FrostMutex m_pending_channel_mutex;
    std::shared_ptr<UVAChannel> m_viewer_channel;
    UString m_pending_ccid;
    NEBULA_ChannelChangeQuietValue m_pending_quiet_value;

    bool m_opapp_running;
    std::uintptr_t m_window_id;
};

#endif // CABOT_OPERA_BROWSER_MUMIN_OPERAMUMINVIDEOBROADCASTBACKEND_HPP_INCLUDED
