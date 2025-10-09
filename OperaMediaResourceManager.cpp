/*
 * OperaMediaResourceManager.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: oguzhanc
 */
#include "config/vewd_integration/enable_broadcast_resource_management.h"
#include "config/novatek_platform.h"

#include "OperaMediaResourceManager.hpp"
#include "OperaWindow.hpp"
#include "OperaWindowManager.hpp"
#include "nebula/adaptation_layer/AnyAvComponentController.hpp"
#include "nebula_src/adaptation_layer/AnyAvObjectMediaPlayer.hpp"
#include "nebula_src/adaptation_layer/AnyAvObjectMediaPlayerResourceHandler.hpp"
#include "nebula_src/adaptation_layer/NebulaMediaPlayerFactory.hpp"
#include "nebula_src/adaptation_layer/NebulaMsePlayer.hpp"

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("opera/resource_man");

OperaMediaResourceManager::OperaMediaResourceManager(OperaWindowManager* window_man,
        AnyAvComponentController* component_controller)
:m_window_man(window_man),
 m_component_controller(component_controller),
 m_media_resource_handler(nullptr),
 m_media_player_count(0),
 m_broadcast_stopped_by_browser(false),
 m_linear_ip_service(false),
 m_hbbtv_window_active(false),
 m_media_player_in_use(false),
 m_window_id(-1),
 m_reason(omi::evt::MediaObjectChanged::REMOVED)
{
    m_media_resource_status[0].mp_count = 0;
    m_media_resource_status[0].resources_allocated = false;
    m_media_resource_status[1].mp_count = 0;
    m_media_resource_status[1].resources_allocated = false;

    m_media_sync_enabled[0] = false;
    m_media_sync_enabled[1] = false;

    m_media_resource_handler = nebula::createMediaPlayerResourceHandler(m_component_controller, false);
}

OperaMediaResourceManager::~OperaMediaResourceManager()
{
    if (m_media_resource_handler)
    {
        nebula::destroyMediaPlayerResourceHandler(m_media_resource_handler);
    }
}

UString OperaMediaResourceManager::getCurrentPageUrl()
{
    cabot::String current_url = "";

    if(m_window_man)
    {
        OperaWindow* active_win = m_window_man->getActiveWindow();
        if (active_win)
        {
            if(active_win->type() == OperaWindow::WindowType::window_hbbtv)
            {
                current_url = active_win->getCurrentPageUrl();
            }
        }
    }
    return UString(current_url.charArray());
}

bool OperaMediaResourceManager::handleResourceRequest(bool request, int resource_type)
{
    TRACE_ALWAYS(("handleResourceRequest() %d %d \n", request, resource_type));

    if ((resource_type == video_output) || (resource_type == audio_output))
    {
        // For our case output is allocated together with decoders. So, there is no need to allocate output separately.
        // See https://bugs.vewd.com/browse/SIERRA-137 for details.
        return true;
    }

    handleInitialResourceRequest(request);

    bool msync_enabled = mediaSyncEnabled();

    TRACE_ALWAYS(("handleResourceRequest() msync_enabled: %d\n", msync_enabled));

    if (request)
    {
        TRACE_ALWAYS(("handleResourceRequest() request mp_count: %d\n", m_media_player_count));
        if (m_media_player_count == 0)
        {

            TRACE_ALWAYS(("handleResourceRequest() Allocating media player resources\n"));
            m_media_resource_handler->allocateMediaPlayerResources(m_linear_ip_service, msync_enabled, getCurrentPageUrl());
        }
        m_media_player_count += 1;

        if (msync_enabled)
        {
            if (resource_type == video_decoder)
            {
                TRACE_ALWAYS(("handleResourceRequest() REQUESTING VIDEO mp_count: %d\n", m_media_player_count));
                m_media_resource_handler->requestResource(AnyAvObjectMediaPlayerResourceHandler::video_decoder);
            }
            else if (resource_type == audio_decoder)
            {
                TRACE_ALWAYS(("handleResourceRequest() REQUESTING AUDIO mp_count: %d\n", m_media_player_count));
                m_media_resource_handler->requestResource(AnyAvObjectMediaPlayerResourceHandler::audio_decoder);
            }
        }
    }
    else
    {
        TRACE_ALWAYS(("handleResourceRequest() release mp_count: %d\n", m_media_player_count));

        if (msync_enabled)
        {
            if (resource_type == video_decoder)
            {
                TRACE_ALWAYS(("handleResourceRequest() RELEASING VIDEO mp_count: %d\n", m_media_player_count));
                m_media_resource_handler->releaseResource(AnyAvObjectMediaPlayerResourceHandler::video_decoder);
            }
            else if (resource_type == audio_decoder)
            {
                TRACE_ALWAYS(("handleResourceRequest() RELEASING AUDIO mp_count: %d\n", m_media_player_count));
                m_media_resource_handler->releaseResource(AnyAvObjectMediaPlayerResourceHandler::audio_decoder);
            }
        }
        m_media_player_count -= 1;
        if (m_media_player_count <= 0)
        {
            TRACE_ALWAYS(("handleResourceRequest() Releasing media player resources\n"));
            m_media_resource_handler->releaseMediaPlayerResources();
            NEBULA_SetVolumeLevel(100, frost_true);
            m_media_player_count = 0;
            m_hbbtv_window_active = false;
        }
    }

    TRACE_VERBOSE(("handleResourceRequest(): m_media_player_in_use=%d, m_media_player_count=%d\n",
                  m_media_player_in_use, m_media_player_count));

    return true;
}

bool OperaMediaResourceManager::handleResourceRequest(bool request, int resource_type, int resource_idx)
{
     TRACE_ALWAYS(("handleResourceRequest() %d res_type: %d index:%d \n", request, resource_type, resource_idx));
     if (resource_idx == -1)
     {
         return false;
     }

     handleInitialResourceRequest(request);

     if (request && (resource_type == audio_decoder || resource_type == video_decoder))
     {
         if (resource_idx == 0)
         {
             m_media_sync_enabled[0] = mediaSyncEnabled();
         }
         else if (resource_idx == 1)
         {
             m_media_sync_enabled[1] = false;
         }
     }

     TRACE_ALWAYS(("handleResourceRequest() msync_enabled:%d \n", m_media_sync_enabled[resource_idx]));

     bool result = false;

     if (m_media_sync_enabled[resource_idx])
     {
         result = handleResourceRequestForMediaSync(request, resource_type, resource_idx);
     }
     else
     {
#if defined(NOVATEK_PLATFORM)
         result = handleResourceRequestForRegularPlaybackNVT(request, resource_type, resource_idx);
#else
         result = handleResourceRequestForRegularPlayback(request, resource_type, resource_idx);
#endif         
     }
     TRACE_ALWAYS(("handleResourceRequest() %d res_type: %d index:%d - DONE\n", request, resource_type, resource_idx));
     return result;
}

#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
void
OperaMediaResourceManager::handleBroadcastResourceReleaseRequested(bool const linear_ip_service)
{
    TRACE_INFO(("handleBroadcastResourceReleaseRequested() Stopping broadcast\n"));
    m_broadcast_stopped_by_browser = true;
    m_linear_ip_service = linear_ip_service;
    m_media_resource_handler->stopBroadcast(linear_ip_service);
}

void
OperaMediaResourceManager::handleBroadcastResourceGranted(int resource_id)
{
    TRACE_INFO(("handleBroadcastResourceGranted()\n"));
    m_broadcast_stopped_by_browser = false;
    m_linear_ip_service = false;
}

bool
OperaMediaResourceManager::isBroadcastStoppedByBrowser() const
{
    return m_broadcast_stopped_by_browser;
}

bool
OperaMediaResourceManager::internalIsMediaPlayerInUse() const
{
    return m_media_player_in_use || (m_media_player_count > 0);
}

bool
OperaMediaResourceManager::isMediaPlayerInUse(omi::Handle window_id) const
{
    const bool in_use = (m_reason != omi::evt::MediaObjectChanged::REMOVED) &&
                        (m_window_id == window_id) &&
                        internalIsMediaPlayerInUse();

    TRACE_VERBOSE(("isMediaPlayerInUse(): window_id=%s, in_use=%d\n",
                  window_id.str().c_str(), in_use));

    return in_use;
}

void
OperaMediaResourceManager::onMediaObjectChanged(
                                    const omi::Handle& window_id,
                                    const omi::evt::MediaObjectChanged::Reason& reason)
{
    TRACE_VERBOSE(("onMediaObjectChanged(): window_id=%s, reason=%d\n",
                  window_id.str().c_str(), reason));

    m_window_id = window_id;
    m_reason = reason;
}

void
OperaMediaResourceManager::onMediaObjectChanged(const omi::evt::MediaObjectChanged& media_object_changed)
{
    onMediaObjectChanged(media_object_changed.window_id, media_object_changed.reason);
}
#endif

bool
OperaMediaResourceManager::handleResourceRequestForMediaSync(bool request, int resource_type, int resource_idx)
{
    if ((resource_type == audio_output) || (resource_type == video_output))
    {
        TRACE_ALWAYS(("handleResourceRequestForMediaSync() nothing to be done for audio & video outputs for msync\n"));
        return true;
    }

    if (request)
    {
        if (m_media_player_count == 0)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() Allocating media player resources\n"));
            m_media_resource_handler->allocateMediaPlayerResources(m_linear_ip_service, true, getCurrentPageUrl());
        }
        m_media_player_count += 1;

        if (resource_type == video_decoder)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() REQUESTING VIDEO mp_count: %d\n", m_media_player_count));
            m_media_resource_handler->requestResource(AnyAvObjectMediaPlayerResourceHandler::video_decoder);
        }
        else if (resource_type == audio_decoder)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() REQUESTING AUDIO mp_count: %d\n", m_media_player_count));
            m_media_resource_handler->requestResource(AnyAvObjectMediaPlayerResourceHandler::audio_decoder);
        }
    }
    else
    {
        if (resource_type == video_decoder)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() RELEASING VIDEO mp_count: %d\n", m_media_player_count));
            m_media_resource_handler->releaseResource(AnyAvObjectMediaPlayerResourceHandler::video_decoder);
        }
        else if (resource_type == audio_decoder)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() RELEASING AUDIO mp_count: %d\n", m_media_player_count));
            m_media_resource_handler->releaseResource(AnyAvObjectMediaPlayerResourceHandler::audio_decoder);
        }

        m_media_player_count -= 1;
        if (m_media_player_count <= 0)
        {
            TRACE_ALWAYS(("handleResourceRequestForMediaSync() Releasing media player resources\n"));
            m_media_resource_handler->releaseMediaPlayerResources();
            NEBULA_SetVolumeLevel(100, frost_true);
            m_media_player_count = 0;
            m_hbbtv_window_active = false;
            m_media_sync_enabled[resource_idx] = false;
        }
    }
    return true;
}

bool
OperaMediaResourceManager::handleResourceRequestForRegularPlayback(bool request, int resource_type, int resource_idx)
{
    if (request)
    {
        if ((m_media_resource_status[0].mp_count == 0) && (m_media_resource_status[1].mp_count == 0))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() - notifyEnteringMediaPlayback\n"));
            m_media_resource_handler->notifyEnteringMediaPlayback();
            m_media_player_in_use = true;
        }

        if ((resource_type == video_decoder) || (resource_type == audio_decoder))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() REQUEST resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count += 1;
        }
        else if ((resource_type == video_output) || (resource_type == audio_output))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() REQUEST resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));

            if (!m_media_resource_status[resource_idx].resources_allocated)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() REQUEST Allocating media player resources\n"));
                m_media_resource_handler->allocateMediaPlayerResources(m_linear_ip_service, false, getCurrentPageUrl());
                m_media_resource_status[resource_idx].resources_allocated = true;

                m_media_player_count = 1;
            }
            m_media_resource_status[resource_idx].mp_count += 1;
        }
    }
    else
    {
        if ((resource_type == video_decoder) || (resource_type == audio_decoder))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() RELEASE resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count -= 1;
        }
        else if ((resource_type == video_output) || (resource_type == audio_output))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() RELEASE resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count -= 1;
        }

        if (m_media_resource_status[resource_idx].mp_count <= 0)
        {
            if (m_media_resource_status[1-resource_idx].mp_count <= 0)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() - notifyExitingMediaPlayback\n"));
                m_media_resource_handler->notifyExitingMediaPlayback();
            }

            if (m_media_resource_status[resource_idx].resources_allocated)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlayback() RELEASE Releasing media player resources\n"));
                m_media_resource_handler->releaseMediaPlayerResources();
                m_media_resource_status[resource_idx].resources_allocated = false;

                m_media_player_count = 0;
            }
            NEBULA_SetVolumeLevel(100, frost_true);
            m_media_resource_status[resource_idx].mp_count = 0;
            m_hbbtv_window_active = false;
            m_media_sync_enabled[resource_idx] = false;
        }

        if ((m_media_resource_status[0].mp_count == 0) && (m_media_resource_status[1].mp_count == 0))
        {
            m_media_player_in_use = false;
        }
    }

    TRACE_VERBOSE(("handleResourceRequestForRegularPlayback(): m_media_player_in_use=%d, m_media_player_count=%d\n",
                  m_media_player_in_use, m_media_player_count));

    return true;
}


bool
OperaMediaResourceManager::handleResourceRequestForRegularPlaybackNVT(bool request, int resource_type, int resource_idx)
{
    if (request)
    {
        if ((m_media_resource_status[0].mp_count == 0) && (m_media_resource_status[1].mp_count == 0))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() - notifyEnteringMediaPlayback\n"));
            m_media_resource_handler->notifyEnteringMediaPlayback();
            m_media_player_in_use = true;
        }

        if ((resource_type == video_output) || (resource_type == audio_output))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() REQUEST resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count += 1;
        }
        else if ((resource_type == video_decoder) || (resource_type == audio_decoder))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() REQUEST resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));

            if (!m_media_resource_status[resource_idx].resources_allocated)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() REQUEST Allocating media player resources\n"));
                m_media_resource_handler->allocateMediaPlayerResources(m_linear_ip_service, false, getCurrentPageUrl());
                m_media_resource_status[resource_idx].resources_allocated = true;

                m_media_player_count = 1;
            }
            m_media_resource_status[resource_idx].mp_count += 1;
        }
    }
    else
    {
        if ((resource_type == video_output) || (resource_type == audio_output))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() RELEASE resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count -= 1;
        }
        else if ((resource_type == video_decoder) || (resource_type == audio_decoder))
        {
            TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() RELEASE resource_type: %d, index: %d, mp_count: %d\n",
                    resource_type, resource_idx, m_media_resource_status[resource_idx].mp_count));
            m_media_resource_status[resource_idx].mp_count -= 1;
        }

        if (m_media_resource_status[resource_idx].mp_count <= 0)
        {
            if (m_media_resource_status[1-resource_idx].mp_count <= 0)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() - notifyExitingMediaPlayback\n"));
                m_media_resource_handler->notifyExitingMediaPlayback();
            }

            if (m_media_resource_status[resource_idx].resources_allocated)
            {
                TRACE_ALWAYS(("handleResourceRequestForRegularPlaybackNVT() RELEASE Releasing media player resources\n"));
                m_media_resource_handler->releaseMediaPlayerResources();
                m_media_resource_status[resource_idx].resources_allocated = false;

                m_media_player_count = 0;
            }
            NEBULA_SetVolumeLevel(100, frost_true);
            m_media_resource_status[resource_idx].mp_count = 0;
            m_hbbtv_window_active = false;
            m_media_sync_enabled[resource_idx] = false;
        }

        if ((m_media_resource_status[0].mp_count == 0) && (m_media_resource_status[1].mp_count == 0))
        {
            m_media_player_in_use = false;
        }
    }

    TRACE_VERBOSE(("handleResourceRequestForRegularPlaybackNVT(): m_media_player_in_use=%d, m_media_player_count=%d\n",
                  m_media_player_in_use, m_media_player_count));

    return true;
}

bool
OperaMediaResourceManager::mediaSyncEnabled() const
{
    bool msync_enabled = false;
    /* Media Synchronization feature is applicable to HbbTV windows only since it's
     * a part of HbbTV Spec since version 2.0.
     * So we must always return false if any other window (such as csp, browser_ui)
     * is active. */
    if (m_hbbtv_window_active)
    {
#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
        msync_enabled = !m_broadcast_stopped_by_browser;
#else
        msync_enabled = true;
#endif
    }
    TRACE_VERBOSE(("mediaSyncEnabled() m_hbbtv_window_active: %d, msync_enabled: %d\n",
                    m_hbbtv_window_active, msync_enabled));
    return msync_enabled;
}

void 
OperaMediaResourceManager::playWebAudio(unsigned int sample_rate,
                unsigned char channel_number,
                unsigned char* buffer,
                unsigned int size,
                bool bof,
                bool eof)
{
    if (m_media_resource_handler)
    {
        m_media_resource_handler->playWebAudio(sample_rate,channel_number,buffer,size,bof,eof);
    }
}

void
OperaMediaResourceManager::handleInitialResourceRequest(bool request)
{
    if (request
#if defined(ENABLE_BROADCAST_RESOURCE_MANAGEMENT)
                && !internalIsMediaPlayerInUse()
#endif
    )
    {
        if (m_window_man)
        {
            OperaWindow* active_win = m_window_man->getActiveWindow();
            if (active_win)
            {
                m_hbbtv_window_active = (active_win->type() == OperaWindow::WindowType::window_hbbtv);
            }
        }
    }
}
