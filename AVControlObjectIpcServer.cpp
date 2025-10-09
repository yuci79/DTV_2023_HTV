#include "AVControlObjectIpcServer.hpp"
#include "3rd_party_wrapper/rpclib/include/CommonIpcServer.hpp"
#include "utilities_debug/trace.h"

// Include the rpc header for the implementation
#include <rpc/server.h>

TRACE_IMPLEMENT("avcontrolobject/ipcserver");

AVControlObjectIpcServer&
AVControlObjectIpcServer::instance()
{
    static AVControlObjectIpcServer the_instance;
    return the_instance;
}

// In the create method:
bool
AVControlObjectIpcServer::createAVControlObject(AnyCommandThread& media_queue,
                                              int streaming_type,  // Use int
                                              MediaDataSource const& media_data_source,
                                              MediaDataSink& media_data_sink,
                                              AnyAVControlObjectEventGenerator& event_generator,
                                              char* origin,
                                              std::intptr_t& handle_out)
{
    TRACE_INFO(("createAVControlObject: streaming_type=%d\n", streaming_type));

    // Convert integer to AVControlObject::StreamingType
    AVControlObject::StreamingType av_streaming_type;
    switch(streaming_type) {
        case 0: av_streaming_type = AVControlObject::progressive; break;
        case 1: av_streaming_type = AVControlObject::mse; break;
        case 2: av_streaming_type = AVControlObject::rtsp; break;
        case 3: av_streaming_type = AVControlObject::udp; break;
        default: av_streaming_type = AVControlObject::progressive; break;
    }

    // Rest of the method remains the same...
    auto* av_object = new AVControlObject(media_queue, av_streaming_type,
                                        media_data_source, media_data_sink,
                                        event_generator, origin);

    if (av_object && av_object->isInitialised()) {
        handle_out = reinterpret_cast<std::intptr_t>(av_object);
        m_objects[handle_out] = av_object;
        TRACE_INFO(("Created AVControlObject with handle: %p\n", (void*)handle_out));
        return true;
    }

    TRACE_ERROR(("Failed to create AVControlObject\n"));
    delete av_object;
    return false;
}

bool
AVControlObjectIpcServer::destroyAVControlObject(std::intptr_t handle)
{
    TRACE_INFO(("destroyAVControlObject: handle=%p\n", (void*)handle));

    auto it = m_objects.find(handle);
    if (it != m_objects.end()) {
        delete it->second;
        m_objects.erase(it);
        TRACE_INFO(("Destroyed AVControlObject with handle: %p\n", (void*)handle));
        return true;
    }

    TRACE_WARN(("AVControlObject handle not found: %p\n", (void*)handle));
    return false;
}

bool
AVControlObjectIpcServer::setSource(std::intptr_t handle, const std::string& url)
{
    auto it = m_objects.find(handle);
    if (it != m_objects.end()) {
        TRACE_INFO(("setSource: handle=%p, url=%s\n", (void*)handle, url.c_str()));
        return it->second->setSource(url.c_str());
    }
    return false;
}

bool
AVControlObjectIpcServer::play(std::intptr_t handle)
{
    auto it = m_objects.find(handle);
    if (it != m_objects.end()) {
        TRACE_INFO(("play: handle=%p\n", (void*)handle));
        return it->second->play();
    }
    return false;
}

bool
AVControlObjectIpcServer::stop(std::intptr_t handle)
{
    auto it = m_objects.find(handle);
    if (it != m_objects.end()) {
        TRACE_INFO(("stop: handle=%p\n", (void*)handle));
        it->second->stop();
        return true;
    }
    return false;
}

bool
AVControlObjectIpcServer::setVideoOutputWindow(std::intptr_t handle, const NEBULA_DisplayWindow& window, bool apply)
{
    auto it = m_objects.find(handle);
    if (it != m_objects.end()) {
        TRACE_INFO(("setVideoOutputWindow: handle=%p\n", (void*)handle));
        return it->second->setVideoOutputWindow(const_cast<NEBULA_DisplayWindow*>(&window), apply);
    }
    return false;
}

// Implement other methods similarly...

void bindAVControlObjectServer(rpc::server& server)
{
    TRACE_ALWAYS(("Binding AVControlObject IPC server methods\n"));

    auto& instance = AVControlObjectIpcServer::instance();

    server.bind(IPC_NAME(AVControlObject_Create), [&instance](
        int streaming_type, const std::string& origin) {

        TRACE_INFO(("RPC: AVControlObject_Create called\n"));

        // For now, create with simplified parameters
        MediaDataSource data_source;
        MediaDataSink data_sink;

        std::intptr_t handle = 0;
        bool success = instance.createAVControlObject(
            *static_cast<AnyCommandThread*>(nullptr), // Need actual queue
            static_cast<nebula::StreamingType>(streaming_type),
            data_source, data_sink,
            *static_cast<AnyAVControlObjectEventGenerator*>(nullptr), // Need actual event generator
            const_cast<char*>(origin.c_str()), handle);

        TRACE_INFO(("RPC: AVControlObject_Create result: success=%d, handle=%p\n",
                   success, (void*)handle));
        return std::make_tuple(success, handle);
    });

    // Bind other methods...
}
