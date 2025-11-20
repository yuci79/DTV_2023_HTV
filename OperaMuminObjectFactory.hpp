/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMuminObjectFactory
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINOBJECTFACTORY_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINOBJECTFACTORY_HPP_INCLUDED

#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "nebula/core/browser_client/AnyVideoBroadcastObject.hpp"
#include "nebula/core/browser_client/VideoBroadcastMediator.hpp"
#include "OperaMuminMediaBackend.hpp"
#include "utilities_private/PrioritisedCommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "utilities_public/FrostMutex.hpp"
#include <vector>

#include "uva_backend.h"

class AnyCommandThread;
class AVControlObject;
class CommandThread;
class MediaFileControlObject;
class VideoBroadcastObject;
class AnyVbObjectEventGenerator;
class AnyAVControlObjectEventGenerator;
class OperaMuminVideoBroadcastBackend;
class WebAudio;

class OperaMuminObjectFactory
{
public:
    OperaMuminObjectFactory();
    ~OperaMuminObjectFactory();

    bool initialise();

    /** 
     * @brief Create a VideoBroadcastObject and register the event generator associated
     * with an OIPF video/broadcast object.
     * 
     * @param out_vb_object If successful, a pointer to the newly assigned VideoBroadcastObject.
     * @param event_generator The event generator to register.
     * @param is_opapp The flag to indicate that the video/broadcast object belongs to which
     *                 type of application.
     *                 It belongs to an HbbTV Application if is_opapp is false,
     *                 It belongs to an Operator Application if is_opapp is true.
     * 
     * @return True if the VideoBroadcastObject is created, assigned and the event generator
     * is registered, false otherwise.
     */
    bool createOipfVideoBroadcastObject(AnyVideoBroadcastObject** out_vb_object,
                                        AnyVbObjectEventGenerator* event_generator,
                                        bool is_opapp=false);

    /**
     * @brief Discard a VideoBroadcastObject and unregister the event generator associated
     * with an OIPF video/broadcast object.
     * 
     * @param vb_object The VideoBroadcastObject to discard.
     * @param event_generator The event generator to unregister.
     */
    void discardOipfVideoBroadcastObject(AnyVideoBroadcastObject* vb_object,
                                         AnyVbObjectEventGenerator* event_generator);

    /**
     * @brief Create a VideoBroadcastObject and register the event generator associated
     * with an HbbTV OpApps BroadcastSupervisor.
     * 
     * @param out_vb_object If successful, a pointer to the newly assigned VideoBroadcastObject.
     * @param event_generator The event generator to register.
     * 
     * @return True if the VideoBroadcastObject is created, assigned and the event generator
     * is registered, false otherwise.
     */
    bool createBroadcastSupervisor(AnyVideoBroadcastObject** out_vb_object,
                                   AnyVbObjectEventGenerator* event_generator);

    /**
     * @brief Discard a VideoBroadcastObject and unregister the event generator associated
     * with an HbbTV OpApps BroadcastSupervisor.
     * 
     * @param vb_object The VideoBroadcastObject to discard.
     * @param event_generator The event generator to unregister.
     */
    void discardBroadcastSupervisor(AnyVideoBroadcastObject* vb_object,
                                    AnyVbObjectEventGenerator* event_generator);
/*
    bool createOipfVideoOnDemandObject(
                                AnyAVControlObject** out_av_object,
                                AnyAVControlObjectEventGenerator* event_generator,
                                AnyCommandThread& media_queue,
                                AVControlObjectType type,
                                OperaMuminMediaBackend::StreamingType streaming_type,
                                MediaDataSource const& media_data_source,
                                MediaDataSink& media_data_sink,
                                bool is_rtsp,
                                bool is_udp,
                                char* origin);
*/
    void discardOipfVideoOnDemandObject(
                               AnyAVControlObject* object);

    /**
     * @brief Allows testing for support without creating unnecessary objects.
     * 
     * @param type The type of UVABackend being querried.
     * @return true If the backed type is supported, otherwise false
     */
    static bool supportedType(UVABackend::UVABackendType type);

    UVABackend* createMuminBackend(
                           UVABackend::UVABackendType type,
                           UVAClient* client);
    void discardMuminBackend(UVABackend* backend);

private:
    class BackendContext
    {
    public:
        BackendContext(WebAudio* web_audio,
                       CommandThread* queue);
        ~BackendContext();

        void stopContext();
    private:
        // copy operations disallowed
        BackendContext(BackendContext const&); // not to be implemented
        BackendContext& operator=(BackendContext const&); // not to be implemented

        WebAudio* m_web_audio;
        CommandThread* m_queue;
    };

    BackendContext* retrieveBackendContext(UVABackend* backend);

private:
    typedef std::pair<UVABackend*, BackendContext*> BackendContextPair;
    typedef std::vector<BackendContextPair> VectorOfBackendContext;

    VectorOfBackendContext m_backend_contexts;
    FrostMutex m_av_object_mutex;
    VideoBroadcastMediator m_video_broadcast_mediator;
};


#endif /* CABOT_OPERA_BROWSER_MUMIN_OPERAMUMINOBJECTFACTORY_HPP_INCLUDED */
