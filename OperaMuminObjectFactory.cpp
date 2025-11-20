/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMuminObjectFactory
 */
#include "config/using_sdk420.h"
#include "config/using_sdk422.h"
#include "config/using_sdk423.h"
#include "config/include_opapp_support.h"

#include "OperaMuminObjectFactory.hpp"
#include "OperaMuminCicamPlayerBackend.hpp"
#include "OperaMuminVideoBroadcastBackend.hpp"
#include "OperaWebAudioBackend.hpp"
#if defined INCLUDE_OPAPP_SUPPORT && defined USING_SDK420
#include "vewd_integration/opapp/OpAppProprietaryFunctionProviderBackend.hpp"
#endif
#include "config/include_ip_tuner.h" //[d]:added to support IP tuner
#include "config/enable_vod_over_ip_tuner.h"
//#include "vewd_integration/ipc/avcontrolobject/AVControlObjectIpcClient.hpp" // adjust include path as needed
//#include "OperaMuminMediaBackendIpc.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp"

#include "nebula/adaptation_layer/CicamPlayerProvider.hpp"
#include "nebula/core/browser_client/AnyVideoBroadcastObject.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "nebula/core/browser_client/WebAudio.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp"

#include "utilities_private/CommandThread.hpp"
#include "utilities_public/AnyCommand.hpp"
#include "eclipse_src/core/Time.hpp"

#include <memory>
#include <utility>

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("mumin/object_factory");

void init_opera_mumin_object_factory()
{
    // Defined to stop unreferenced classes/functions from being removed
    // at link time, that are required at runtime by Vewd shared libraries.
}

OperaMuminObjectFactory::BackendContext::BackendContext(
        WebAudio* web_audio,
        CommandThread* queue)
    : m_web_audio(web_audio)
    , m_queue(queue)
{}

OperaMuminObjectFactory::BackendContext::~BackendContext()
{
    delete m_web_audio;
    delete m_queue;
}

void
OperaMuminObjectFactory::BackendContext::stopContext()
{
    if (m_queue)
    {
        m_queue->terminateAndWaitForExit();
    }
}

OperaMuminObjectFactory::OperaMuminObjectFactory()
{
    TRACE_ALWAYS(("OperaMuminObjectFactory::OperaMuminObjectFactory\n"));

}

bool OperaMuminObjectFactory::initialise()
{
    if (!m_av_object_mutex.initialise())
    {
        TRACE_ERROR(("mutex creation failed!"));
        return false;
    }

    return m_video_broadcast_mediator.isInitialised();
}

OperaMuminObjectFactory::~OperaMuminObjectFactory()
{
    CABOT_ASSERT(m_backend_contexts.size() == 0);
}

/*#ifdef INCLUDE_IP_TUNER
bool OperaMuminObjectFactory::createOipfVideoOnDemandObject(
    AnyAVControlObject** out_av_object,
    AnyAVControlObjectEventGenerator* event_generator,
    AnyCommandThread& media_queue,
    AVControlObjectType type,
    OperaMuminMediaBackend::StreamingType streaming_type,
    MediaDataSource const& media_data_source,
    MediaDataSink& media_data_sink,
    bool is_rtsp,
    bool is_udp,
    char* origin)
{
    TRACE_ALWAYS(("OperaMuminObjectFactory::createOipfVideoOnDemandObject (IP_TUNER/RPC Mode)\n"));

    // Set dummy object so browser-side checks pass
    // Real object exists on Aurora side via RPC
    *out_av_object = reinterpret_cast<AnyAVControlObject*>(0x12345678);

    TRACE_INFO(("Created dummy AV object for RPC: %p\n", *out_av_object));
    return true;  // Always succeed for RPC mode
}
#else
// Keep original implementation unchanged for non-IP_TUNER builds
bool OperaMuminObjectFactory::createOipfVideoOnDemandObject(
    AnyAVControlObject** out_av_object,
    AnyAVControlObjectEventGenerator* event_generator,
    AnyCommandThread& media_queue,
    AVControlObjectType type,
    OperaMuminMediaBackend::StreamingType streaming_type,
    MediaDataSource const& media_data_source,
    MediaDataSink& media_data_sink,
    bool is_rtsp,
    bool is_udp,
    char* origin)
{
    TRACE_ALWAYS(("OperaMuminObjectFactory::createOipfVideoOnDemandObject (Original Mode)\n"));

    FrostMutexLock lock(m_av_object_mutex);
    AnyAVControlObject* av_object = NULL;

    {
        AVControlObject::StreamingType streaming_type_enum;

        if (is_rtsp)
        {
            streaming_type_enum = AVControlObject::rtsp;
        }
        else if (is_udp)
        {
            streaming_type_enum = AVControlObject::udp;
        }
        else
        {
            streaming_type_enum = (streaming_type == OperaMuminMediaBackend::mse) ?
                        AVControlObject::mse : AVControlObject::progressive;
        }

        av_object = new AVControlObject(media_queue, streaming_type_enum,
                media_data_source, media_data_sink, *event_generator, origin);
    }

    if (av_object && av_object->isInitialised())
    {
        *out_av_object = av_object;
        TRACE_INFO(("Created real AV object : %p\n", av_object));
        return true;
    }

    TRACE_ERROR(("Failed to create AV object\n"));
    delete av_object;
    return false;
}
#endif
*/
void
OperaMuminObjectFactory::discardOipfVideoOnDemandObject(AnyAVControlObject* av_object)
{
    delete av_object;
}


bool
OperaMuminObjectFactory::createOipfVideoBroadcastObject(
                               AnyVideoBroadcastObject** out_vb_object,
                               AnyVbObjectEventGenerator* event_generator,
                               bool is_opapp)
{
    return m_video_broadcast_mediator.createVideoBroadcastObject(out_vb_object,
                                                                 event_generator,
                                                                 is_opapp);
}

void
OperaMuminObjectFactory::discardOipfVideoBroadcastObject(
                                AnyVideoBroadcastObject* vb_object,
                                AnyVbObjectEventGenerator* event_generator)
{
    m_video_broadcast_mediator.discardVideoBroadcastObject(vb_object, event_generator);
}

bool
OperaMuminObjectFactory::createBroadcastSupervisor(AnyVideoBroadcastObject** out_vb_object,
                               AnyVbObjectEventGenerator* event_generator)
{
    return m_video_broadcast_mediator.createBroadcastSupervisor(out_vb_object, event_generator);
}

void 
OperaMuminObjectFactory::discardBroadcastSupervisor(AnyVideoBroadcastObject* vb_object,
                                        AnyVbObjectEventGenerator* event_generator)
{
    m_video_broadcast_mediator.discardBroadcastSupervisor(vb_object, event_generator);
}

bool
OperaMuminObjectFactory::supportedType(UVABackend::UVABackendType type)
{
    switch (type)
    {
        case UVABackend::UVAVideoBroadcastBackendType:
        case UVABackend::UVAVideoBackendType:
        case UVABackend::UVAAudioBackendType:
#if defined INCLUDE_OPAPP_SUPPORT && defined USING_SDK420
        case UVABackend::UVAProprietaryFunctionBackendType:
#endif
            return true;

        default:
            TRACE_WARN(("supportedType(): type: %u NOT SUPPORTED\n", type));
            break;
    }

    return false;
}

UVABackend*
OperaMuminObjectFactory::createMuminBackend(
                       UVABackend::UVABackendType type,
                       UVAClient* client)
{
    TRACE_ALWAYS(("createMuminBackend: type: %d, client: %p\n", type, client));

    switch (type)
    {
#if defined INCLUDE_OPAPP_SUPPORT && defined USING_SDK420
        case UVABackend::UVAProprietaryFunctionBackendType:
        {
            TRACE_INFO(("requestBackend: Creating OpApp PF Provider backend\n"));
            BackendContext* context = new BackendContext(nullptr, nullptr);

            OpAppProprietaryFunctionProviderBackend* pfp_backend =
                    new OpAppProprietaryFunctionProviderBackend(client);
            if (!pfp_backend)
            {
                delete context;
                return nullptr;
            }

            m_backend_contexts.push_back(std::make_pair(pfp_backend, context));

            TRACE_INFO(("createMuminBackend: UVAProprietaryFunctionBackend: %p\n", pfp_backend));
            return pfp_backend;
        }
#endif
        case UVABackend::UVAVideoBroadcastBackendType:
        {
            TRACE_INFO(("requestBackend: Creating Video Broadcast backend \n"));
            OperaMuminVideoBroadcastBackend* vb_backend = new OperaMuminVideoBroadcastBackend
                    (client, *this, m_video_broadcast_mediator);
            return vb_backend;
        }
        case UVABackend::UVAVideoBackendType:
        case UVABackend::UVAAudioBackendType:
        {
//#ifndef INCLUDE_IP_TUNER
           // UVAMediaBackend* media_backend = nullptr;
//#endif
#if 1//!defined USING_SDK422 && !defined USING_SDK423
#ifndef INCLUDE_IP_TUNER
            
            auto cicam_player = nebula::cicamPlayer();
            if (cicam_player)
            {
                TRACE_INFO(("requestBackend: Creating CICAM player Media backend\n"));

                std::unique_ptr<CommandThread> queue(new CommandThread());
                if (queue->initialise("CICAM player backend thread",
                                               FROST_high_priority, 4 * 1024))
                {
                    media_backend = new OperaMuminCicamPlayerBackend
                                            (type, client, *cicam_player, *queue);
                    std::unique_ptr<BackendContext> context(
                                        new BackendContext(nullptr, queue.get()));
                    queue.release();
                    m_backend_contexts.push_back(
                                        std::make_pair(media_backend, context.get()));
                    context.release();
                }
            }
#else
            // PrioritisedCommandThread* queue = new PrioritisedCommandThread();

            auto queue_handle = nebulaRpcCall<intptr_t>(
                IPC_NAME(NEBULA_CreatePrioritisedCommandThread)
            );
            PrioritisedCommandThread* queue = reinterpret_cast<PrioritisedCommandThread*>(queue_handle);

            if (!queue)
            {
                return NULLPTR;
            }


            // if (!queue->initialise("Media playback queue", FROST_high_priority, 64 * 1024))

            auto init_result = nebulaRpcCall<frost_bool>(
                IPC_NAME(NEBULA_PrioritisedCommandThreadInitialise),
                queue_handle,
                reinterpret_cast<intptr_t>("Media playback queue"),
                static_cast<int>(FROST_high_priority),
                64 * 1024,
                0  // queue_size parameter
            );

            if (!init_result)
            {
                delete queue;
                return NULLPTR;
            }

            BackendContext* context = new BackendContext(NULLPTR, queue);
            if (!context)
            {
                delete queue;
                return NULLPTR;
            }

            UVAMediaBackend* media_backend = new OperaMuminMediaBackend(type, client, *this, *queue);
            m_backend_contexts.push_back(std::make_pair(media_backend, context));
            TRACE_INFO(("requestBackend: Created Media backend %p\n", media_backend));
#endif
#endif

//#ifndef INCLUDE_IP_TUNER
            return media_backend;
//#endif
/*            TRACE_INFO(("requestBackend: Creating IPC Media backend\n"));

            // Create IPC wrapper instead of OperaMuminMediaBackend
            UVAMediaBackend* media_backend = new OperaMuminMediaBackendIpc(type, client);

            TRACE_INFO(("requestBackend: Created IPC Media backend %p\n", media_backend));
            return media_backend;*/
        }
        default:
            TRACE_WARN(("createMuminBackend: NOT SUPPORTED type: %d, client: %p\n", type, client));
            return NULL;
    }
}

void
OperaMuminObjectFactory::discardMuminBackend(UVABackend* backend)
{
    TRACE_INFO(("discardMuminBackend: backend: %p\n", backend));
    BackendContext* context = retrieveBackendContext(backend);
    if (context)
    {
        context->stopContext();
    }
    delete backend;
    delete context;
}

OperaMuminObjectFactory::BackendContext*
OperaMuminObjectFactory::retrieveBackendContext(UVABackend* backend)
{
    BackendContext* context = nullptr;

    VectorOfBackendContext::iterator it = m_backend_contexts.begin();
    for (; it != m_backend_contexts.end(); ++it)
    {
        if (it->first == backend)
        {
            context = it->second;
            m_backend_contexts.erase(it);
            break;
        }
    }
    return context;
}
