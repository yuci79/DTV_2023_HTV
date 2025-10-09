#include "rpc/server.h"
#include "AVControlObjectIpcServer.hpp"
#include "AVControlObjectIpcTypes.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "utilities_public/AnyCommand.hpp"     // AnyCommandThread typedef
#include "nebula/core/browser_client/AVControlObject.hpp"   // provides AVControlObject class
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp" // for IPC_NAME / server reference (adjust path)
#include <map>
#include <atomic>
// forward declarations already in headers:
// AnyCommandThread, MediaDataSink, AnyAVControlObjectEventGenerator

// Only pointers — do NOT instantiate abstract classes.
static AnyCommandThread* s_command_thread = nullptr;
static MediaDataSink* s_media_data_sink = nullptr;
static AnyAVControlObjectEventGenerator* s_event_generator = nullptr;

// single map (ensure only one declaration)
static std::map<std::int64_t, AVControlObject*> s_instances;
static std::atomic<std::int64_t> s_next_handle(1);
static std::mutex s_instances_mutex; // protect the map in multi-threaded context

// setters (definitions)
void AVControlObjectIpcServer_setCommandThread(AnyCommandThread* thread) {
    s_command_thread = thread;
}

void AVControlObjectIpcServer_setMediaDataSink(MediaDataSink* sink) {
    s_media_data_sink = sink;
}

void AVControlObjectIpcServer_setEventGenerator(AnyAVControlObjectEventGenerator* eg) {
    s_event_generator = eg;
}

static std::int64_t handle_create(int streaming_type, const std::string& origin) {
    // Validate we have necessary backend pieces
    if (!s_command_thread || !s_media_data_sink || !s_event_generator) {
        // Not ready -> return 0 to indicate failure. Client must check.
        return 0;
    }

    // Map int streaming_type to AVControlObject::StreamingType (the type lives in AVControlObject class)
    AVControlObject::StreamingType st = AVControlObject::progressive;
    // match integers used by client; adjust as needed
    if (streaming_type == 1) st = AVControlObject::mse;
    else if (streaming_type == 2) st = AVControlObject::rtsp;
    else if (streaming_type == 3) st = AVControlObject::udp;

    // Create MediaDataSource. If client needs data inside it, you'll need to serialize fields from client.
    MediaDataSource src; // minimal for MVP
    AVControlObject* obj = new AVControlObject(*s_command_thread, st, src, *s_media_data_sink, *s_event_generator, const_cast<char*>(origin.c_str()));
    if (!obj) return 0;

    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    std::int64_t h = s_next_handle.fetch_add(1);
    s_instances[h] = obj;
    return h;
}

static bool handle_setSource(std::int64_t handle, const std::string& url) {
    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->setSource(url.c_str());
}

static bool handle_play(std::int64_t handle) {
    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->play();
}

static bool handle_stop(std::int64_t handle) {
    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    it->second->stop();
    return true;
}

static bool handle_seek(std::int64_t handle, std::int64_t position_msecs) {
    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->seek(static_cast<double>(position_msecs));
}

static void handle_destroy(std::int64_t handle) {
    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return;
    delete it->second;
    s_instances.erase(it);
}

void AVControlObjectIpcServer::bindToServer(rpc::server& server) {
    server.bind(IPC_NAME(AVControlObject_Create),    handle_create);
    server.bind(IPC_NAME(AVControlObject_SetSource), handle_setSource);
    server.bind(IPC_NAME(AVControlObject_Play),      handle_play);
    server.bind(IPC_NAME(AVControlObject_Stop),      handle_stop);
    server.bind(IPC_NAME(AVControlObject_Seek),      handle_seek);
    server.bind(IPC_NAME(AVControlObject_Destroy),   handle_destroy);
}
