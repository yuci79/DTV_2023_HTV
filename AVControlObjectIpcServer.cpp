#include "AVControlObjectIpcServer.hpp"
#include "AVControlObjectIpcTypes.hpp"

#include "nebula/core/browser_client/AVControlObject.hpp"   // provides AVControlObject class
#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp" // for IPC_NAME / server reference (adjust path)
#include <map>
#include <atomic>

// storage for created backend objects
static std::map<std::int64_t, AVControlObject*> s_instances;
static std::atomic<std::int64_t> s_next_handle(1);

// named handler functions (avoid complicated lambda signatures)
static std::int64_t handle_create(int streaming_type, const std::string& origin) {
    // TODO: wire real AnyCommandThread, MediaDataSource, MediaDataSink and event generator
    AnyCommandThread& cmdThread = getGlobalMediaCommandThread(); // implement or pass through registration
    MediaDataSource src;
    MediaDataSink* sink = getMediaDataSink(); // implement or retrieve
    AnyAVControlObjectEventGenerator* eventGen = getEventGeneratorForAv(); // implement or retrieve

    AVControlObject* obj = new AVControlObject(cmdThread, (AVControlObject::StreamingType)streaming_type,
                                               src, *sink, *eventGen, const_cast<char*>(origin.c_str()));
    std::int64_t h = s_next_handle.fetch_add(1);
    s_instances[h] = obj;
    return h;
}

static bool handle_setSource(std::int64_t handle, const std::string& url) {
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->setSource(url.c_str());
}

static bool handle_play(std::int64_t handle) {
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->play();
}

static bool handle_stop(std::int64_t handle) {
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    it->second->stop();
    return true;
}

static bool handle_seek(std::int64_t handle, long long ms) {
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return false;
    return it->second->seek(static_cast<double>(ms));
}

static void handle_destroy(std::int64_t handle) {
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return;
    delete it->second;
    s_instances.erase(it);
}

// ---- Binding ----
void AVControlObjectIpcServer::bindToServer(rpc::server& server) {
    server.bind(IPC_NAME(AVControlObject_Create), handle_create);
    server.bind(IPC_NAME(AVControlObject_SetSource), handle_setSource);
    server.bind(IPC_NAME(AVControlObject_Play), handle_play);
    server.bind(IPC_NAME(AVControlObject_Stop), handle_stop);
    server.bind(IPC_NAME(AVControlObject_Seek), handle_seek);
    server.bind(IPC_NAME(AVControlObject_Destroy), handle_destroy);
}
