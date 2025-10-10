// ============================================================================
// FILE: AVControlObjectIpcServer.cpp (FIXED - DO NOT INCLUDE .cpp FILES!)
// ============================================================================

#include "AVControlObjectIpcServer.hpp"
#include "AVControlObjectIpcTypes.hpp"
#include "AVControlHelpers.hpp"  // Include header only!
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>
#include <map>
#include <atomic>
#include <mutex>

TRACE_IMPLEMENT("avcontrolobject/ipcserver");

// Global state
static std::map<std::int64_t, AVControlObject*> s_instances;
static std::atomic<std::int64_t> s_next_handle(1000);
static std::mutex s_instances_mutex;

// Backend dependencies
static AnyCommandThread* s_command_thread = nullptr;
static MediaDataSink* s_media_data_sink = nullptr;
static AnyAVControlObjectEventGenerator* s_event_generator = nullptr;

void AVControlObjectIpcServer_setCommandThread(AnyCommandThread* thread) {
    TRACE_ALWAYS(("AVControlObjectIpcServer_setCommandThread: %p\n", thread));
    s_command_thread = thread;
}

void AVControlObjectIpcServer_setMediaDataSink(MediaDataSink* sink) {
    TRACE_ALWAYS(("AVControlObjectIpcServer_setMediaDataSink: %p\n", sink));
    s_media_data_sink = sink;
}

void AVControlObjectIpcServer_setEventGenerator(AnyAVControlObjectEventGenerator* eg) {
    TRACE_ALWAYS(("AVControlObjectIpcServer_setEventGenerator: %p\n", eg));
    s_event_generator = eg;
}

static bool ensureDependenciesInitialized() {
    if (!s_command_thread || !s_media_data_sink || !s_event_generator) {
        TRACE_WARN(("Using fallback dependencies\n"));

        if (!s_command_thread) {
            s_command_thread = &getGlobalMediaCommandThread();
        }
        if (!s_media_data_sink) {
            s_media_data_sink = getMediaDataSink();
        }
        if (!s_event_generator) {
            s_event_generator = getEventGeneratorForAv();
        }
    }

    return (s_command_thread && s_media_data_sink && s_event_generator);
}

// RPC handler: Create
static std::int64_t handle_create(int streaming_type, const std::string& origin) {
    TRACE_ALWAYS(("=== RPC: AVControlObject_Create ===\n"));
    TRACE_ALWAYS(("    streaming_type=%d, origin=%s\n", streaming_type, origin.c_str()));

    if (!ensureDependenciesInitialized()) {
        TRACE_ERROR(("    FAILED: Dependencies not initialized\n"));
        return 0;
    }

    AVControlObject::StreamingType st = AVControlObject::progressive;
    switch(streaming_type) {
        case 0: st = AVControlObject::progressive; break;
        case 1: st = AVControlObject::mse; break;
        case 2: st = AVControlObject::rtsp; break;
        case 3: st = AVControlObject::udp; break;
        default:
            TRACE_ERROR(("    Invalid streaming type: %d\n", streaming_type));
            return 0;
    }

    try {
        // CRITICAL FIX: MediaDataSource doesn't have 'url' member
        // It's a simple struct - just pass empty one
        MediaDataSource src;

        TRACE_INFO(("    Creating AVControlObject...\n"));

        AVControlObject* obj = new AVControlObject(
            *s_command_thread,
            st,
            src,
            *s_media_data_sink,
            *s_event_generator,
            const_cast<char*>(origin.c_str()));

        if (!obj) {
            TRACE_ERROR(("    FAILED: Object is null\n"));
            return 0;
        }

        if (!obj->isInitialised()) {
            TRACE_ERROR(("    FAILED: Not initialized\n"));
            delete obj;
            return 0;
        }

        const std::lock_guard<std::mutex> lock(s_instances_mutex);
        std::int64_t handle = s_next_handle.fetch_add(1);
        s_instances[handle] = obj;

        TRACE_ALWAYS(("=== AVControlObject_Create SUCCESS: handle=%lld ===\n", handle));
        return handle;

    } catch (const std::exception& e) {
        TRACE_ERROR(("    EXCEPTION: %s\n", e.what()));
        return 0;
    }
}

static bool handle_setSource(std::int64_t handle, const std::string& url) {
    TRACE_ALWAYS(("=== RPC: AVControlObject_SetSource ===\n"));
    TRACE_ALWAYS(("    handle=%lld, url=%s\n", handle, url.c_str()));

    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) {
        TRACE_ERROR(("    Handle not found\n"));
        return false;
    }

    bool result = it->second->setSource(url.c_str());
    TRACE_ALWAYS(("=== SetSource result=%d ===\n", result));
    return result;
}

static bool handle_play(std::int64_t handle) {
    TRACE_ALWAYS(("=== RPC: AVControlObject_Play ===\n"));
    TRACE_ALWAYS(("    handle=%lld\n", handle));

    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) {
        TRACE_ERROR(("    Handle not found\n"));
        return false;
    }

    bool result = it->second->play();
    TRACE_ALWAYS(("=== Play result=%d ===\n", result));
    return result;
}

static bool handle_stop(std::int64_t handle) {
    TRACE_ALWAYS(("=== RPC: AVControlObject_Stop ===\n"));

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
    TRACE_ALWAYS(("=== RPC: AVControlObject_Destroy ===\n"));

    const std::lock_guard<std::mutex> lock(s_instances_mutex);
    auto it = s_instances.find(handle);
    if (it == s_instances.end()) return;

    delete it->second;
    s_instances.erase(it);
}

void AVControlObjectIpcServer::bindToServer(rpc::server& server) {
    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("AVControlObjectIpcServer::bindToServer - START\n"));
    TRACE_ALWAYS(("=================================================\n"));

    ensureDependenciesInitialized();

    server.bind(IPC_NAME(AVControlObject_Create), handle_create);
    TRACE_ALWAYS(("  Bound: AVControlObject_Create\n"));

    server.bind(IPC_NAME(AVControlObject_SetSource), handle_setSource);
    TRACE_ALWAYS(("  Bound: AVControlObject_SetSource\n"));

    server.bind(IPC_NAME(AVControlObject_Play), handle_play);
    TRACE_ALWAYS(("  Bound: AVControlObject_Play\n"));

    server.bind(IPC_NAME(AVControlObject_Stop), handle_stop);
    TRACE_ALWAYS(("  Bound: AVControlObject_Stop\n"));

    server.bind(IPC_NAME(AVControlObject_Seek), handle_seek);
    TRACE_ALWAYS(("  Bound: AVControlObject_Seek\n"));

    server.bind(IPC_NAME(AVControlObject_Destroy), handle_destroy);
    TRACE_ALWAYS(("  Bound: AVControlObject_Destroy\n"));

    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("AVControlObjectIpcServer::bindToServer - COMPLETE\n"));
    TRACE_ALWAYS(("=================================================\n"));
}
