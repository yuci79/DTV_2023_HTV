/*
 * OperaMediaResourceManager.hpp
 *
 * @file
 * @brief This class defines the functionality that is required to handle browser's requests about
 *        necessary resources during a broadband playback.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAMEDIARESOURCEMANAGER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAMEDIARESOURCEMANAGER_HPP_INCLUDED

#include "config/vewd_integration/enable_broadcast_resource_management.h"

#include <omi/events/MediaObjectChanged.h>

#include "utilities_public/UString.hpp"

class OperaWindowManager;
class AnyAvComponentController;
class AnyAvObjectMediaPlayerResourceHandler;

class OperaMediaResourceManager
{
public:
    /**
     * @brief Constructor.
     *
     * @param window_man An instance of window manager.
     * @param component_controller An instance of AnyAvComponentController.
     */
    OperaMediaResourceManager(
                      OperaWindowManager* window_man,
                      AnyAvComponentController* component_controller);

    /**
     * @brief Destructor
     */
    ~OperaMediaResourceManager();

    /**
     * @brief Returns current page URL
     *
     * @return current page URL
     */
    UString getCurrentPageUrl();

    /**
     * @brief Handle a resource request / release from browser side.
     *
     * @param request If true, it means browser is requesting the specified resource,
     *                if false, it means browser is releasing the specified resource.
     * @param resource_type Type of the resource that will be requested / released.
     *                See the MediaResourceType enum for possible resource types.
     *
     * @return true if operations is successful, false otherwise.
     */
    bool handleResourceRequest(bool request, int resource_type);

    /**
     * @brief Handle a resource request / release from browser side.
     *
     * @param request If true, it means browser is requesting the specified resource,
     *                if false, it means browser is releasing the specified resource.
     * @param resource_type Type of the resource that will be requested / released.
     *                See the MediaResourceType enum for possible resource types.
     * @param resource_idx Index of the resource to be allocated / released if there are
     *                more than one.
     *
     * @return true if operations is successful, false otherwise.
     *
     */
    bool handleResourceRequest(bool request, int resource_type, int resource_idx);

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    /**
     * @brief Release resources from broadcast once requested by browser. As part of Vewd's
     *        Browser Resource Management feature, browser can request to stop broadcast
     *        when the resources (a/v decoders and outputs) are needed by broadband playback.
     * @param linear_ip_service if it is an linear_ip_service certain management will 
     *        happen within NovoLinearIPServiceLifecycle
     */
    void handleBroadcastResourceReleaseRequested(bool const linear_ip_service);

    /**
     * @brief Integration can use resources for broadcast again after receiving
     *        this call from browser. Note that this function will be called if
     *        broadcast resources are previously released by a call to
     *        handleBroadcastResourceReleaseRequested() function.
     *
     * @brief resource_id Resource id that can be used for broadcast.
     */
    void handleBroadcastResourceGranted(int resource_id);

    bool isBroadcastStoppedByBrowser() const;

    /**
     * @brief Query if the media player is in use by a specific window.
     * 
     * @param window_id is the window being queried about.
     * 
     * @return true if the media player is in use by the specific window.
     */
    bool isMediaPlayerInUse(omi::Handle window_id) const;

    /**
     * @brief An event handler for Opera engine media object changed events.
     * 
     * @param media_object_changed The data for this event.
     */
    void onMediaObjectChanged(const omi::evt::MediaObjectChanged& media_object_changed);

    /**
     * @brief An event handler for Opera engine media object changed events.
     * 
     * @param window_id The window id for this event.
     * @param reason The reason of this event.
     */
    void onMediaObjectChanged(const omi::Handle& window_id,
                              const omi::evt::MediaObjectChanged::Reason& reason);
#endif

    void playWebAudio(unsigned int sample_rate,
                    unsigned char channel_number,
                    unsigned char* buffer,
                    unsigned int size,
                    bool bof,
                    bool eof);

    OperaMediaResourceManager(const OperaMediaResourceManager&) = delete;
    OperaMediaResourceManager& operator=(const OperaMediaResourceManager&) = delete;

public:
    enum MediaResourceType
    {
        video_decoder = (1 << 0), ///< video decoder type = 1
        audio_decoder = (1 << 1), ///< audio decoder type = 2
        video_output  = (1 << 2), ///< video output type  = 4
        audio_output  = (1 << 3), ///< audio output type  = 8
    };
    
private:
    void handleInitialResourceRequest(bool request);
    bool handleResourceRequestForMediaSync(bool request, int resource_type, int resource_idx);
    bool handleResourceRequestForRegularPlayback(bool request, int resource_type, int resource_idx);
    bool handleResourceRequestForRegularPlaybackNVT(bool request, int resource_type, int resource_idx);
    bool mediaSyncEnabled() const;

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
    virtual bool internalIsMediaPlayerInUse() const;
#endif

private:
    OperaWindowManager* m_window_man;
    AnyAvComponentController* m_component_controller;
    AnyAvObjectMediaPlayerResourceHandler* m_media_resource_handler;
    int m_media_player_count;
    bool m_broadcast_stopped_by_browser;
    bool m_linear_ip_service;
    bool m_hbbtv_window_active;
    struct MediaResourceStatus
    {
        int mp_count;
        int resources_allocated;
    };
    MediaResourceStatus m_media_resource_status[2];
    bool m_media_sync_enabled[2];
    bool m_media_player_in_use;

    omi::Handle m_window_id;
    omi::evt::MediaObjectChanged::Reason m_reason;
};



#endif /* VEWD_INTEGRATION_CORE_OPERAMEDIARESOURCEMANAGER_HPP_ */
