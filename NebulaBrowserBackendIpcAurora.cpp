/**
 * Copyright 2024 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of Mumin Backend IPC Aurora handlers
 */

#include "NebulaBrowserBackendIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include "nebula/core/browser_client/AVControlObject.hpp"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"

// Use the helpers that provide implementations
#include "vewd_integration/ipc/avcontrolobject/AVControlHelpers.hpp"

#include "utilities_private/CommandThread.hpp"  // Use concrete CommandThread instead of AnyCommandThread
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"

#include "frost/frost_debug.h"
#include "frost/frost_threads.h"

#include <rpc/server.h>
#include <map>
#include <mutex>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaBrowserBackendIpcAurora");

// ============================================================================
// Global State Management
// ============================================================================

namespace {

// Global service infrastructure
static CommandThread* s_media_thread = nullptr;
static AnyAVControlObjectEventGenerator* s_event_generator = nullptr;

// Service state
static bool s_mumin_service_running = false;
static std::mutex s_service_mutex;

// Backend instance structure
struct MuminBackendInstance {
    AVControlObject* av_control_obj;
    MediaDataSource* data_source;
    MediaDataSink* data_sink;

    MuminBackendInstance()
        : av_control_obj(nullptr)
        , data_source(nullptr)
        , data_sink(nullptr)
    {}

    ~MuminBackendInstance() {
        if (av_control_obj) {
            delete av_control_obj;
        }
        if (data_source) {
            delete data_source;
        }
        if (data_sink) {
            delete data_sink;
        }
    }
};

// Backend instance management
static std::map<intptr_t, MuminBackendInstance*> s_backend_instances;
static std::atomic<intptr_t> s_next_backend_handle(1000);
static std::mutex s_backend_mutex;

inline intptr_t to_intptr(void* p) {
    return reinterpret_cast<intptr_t>(p);
}

inline void* from_intptr(intptr_t i) {
    return reinterpret_cast<void*>(i);
}

} // anonymous namespace

// ============================================================================
// Module Initialization
// ============================================================================

// This will be called when the module loads
static struct ModuleInitializer {
    ModuleInitializer() {
        TRACE_ALWAYS(("=== NebulaBrowserBackendIpcAurora Module Loaded ===\n"));
        TRACE_ALWAYS(("Initial state: s_mumin_service_running=%d\n", s_mumin_service_running));
    }
} s_module_init;

// ============================================================================
// IPC Handler: Start Mumin Service
// ============================================================================

static frost_bool handle_MuminStartService(frost_int reserved)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminStartService ===\n"));
    TRACE_ALWAYS(("reserved=%d\n", reserved));

    TRACE_ALWAYS(("Attempting to acquire service mutex...\n"));
    const std::lock_guard<std::mutex> lock(s_service_mutex);
    TRACE_ALWAYS(("Service mutex acquired successfully\n"));

    TRACE_ALWAYS(("Current service state: s_mumin_service_running=%d\n", s_mumin_service_running));

    if (s_mumin_service_running)
    {
        TRACE_ALWAYS(("Mumin service already running - returning success\n"));
        return frost_true;
    }

    TRACE_ALWAYS(("Starting Mumin service infrastructure...\n"));

    // Step 1. Create the command thread for A/V control
    static CommandThread* s_media_thread = new CommandThread;
    const int stacksize_hint = 64*1024;
    if (!s_media_thread || !s_media_thread->initialise("MediaCmdThread",
                                           FROST_highest_priority,
                                           stacksize_hint))
    {
        TRACE_ERROR(("Command thread initialisation failed!!!\n"));
        delete s_media_thread;
        s_media_thread = nullptr;
        return frost_false;
    }


    // Step 2. Get event generator (shared by all backends)
    if (!s_event_generator)
    {
        TRACE_ALWAYS(("Getting event generator...\n"));
        s_event_generator = getEventGeneratorForAv();
        if (!s_event_generator)
        {
            TRACE_ERROR(("Failed to get event generator\n"));
            return frost_false;
        }
        TRACE_ALWAYS(("Event generator obtained: %p\n", (void*)s_event_generator));
    }

    s_mumin_service_running = true;
    TRACE_ALWAYS(("=== Mumin Service infrastructure started successfully ===\n"));
    TRACE_ALWAYS(("    CommandThread: %p\n", (void*)s_media_thread));
    TRACE_ALWAYS(("    EventGenerator: %p\n", (void*)s_event_generator));
    TRACE_ALWAYS(("=== Ready to create backend instances ===\n"));

    return frost_true;
}


// ============================================================================
// IPC Handler: Stop Mumin Service
// ============================================================================

static frost_bool handle_MuminStopService()
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminStopService ===\n"));

    const std::lock_guard<std::mutex> lock(s_service_mutex);

    if (!s_mumin_service_running) {
        TRACE_INFO(("Service not running\n"));
        return frost_true;
    }

    // Clean up all backend instances
    {
        const std::lock_guard<std::mutex> backend_lock(s_backend_mutex);

        // TODO: Properly destroy all backend instances
        for (auto& pair : s_backend_instances) {
            // Delete the backend object
            // delete static_cast<SomeBackendType*>(pair.second);
        }
        s_backend_instances.clear();
    }

    // Shutdown global resources
    // TODO: Add actual cleanup here

    s_mumin_service_running = false;

    TRACE_ALWAYS(("=== Mumin Service Stopped ===\n"));
    return frost_true;
}

// ============================================================================
// IPC Handler: Get Service Status
// ============================================================================

static frost_int handle_MuminGetStatus()
{
    TRACE_VERBOSE(("=== RPC: NEBULA_MuminGetStatus ===\n"));

    const std::lock_guard<std::mutex> lock(s_service_mutex);

    // Return 1 if running, 0 if not
    frost_int status = s_mumin_service_running ? 1 : 0;

    TRACE_VERBOSE(("    status=%d\n", status));
    return status;
}

// ============================================================================
// IPC Handler: Initialize Backend
// ============================================================================

static frost_bool handle_MuminInitBackend(
    intptr_t backend_handle,
    const std::string& origin)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminInitBackend ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld, origin=%s\n",
                 backend_handle, origin.c_str()));

    if (!s_mumin_service_running) {
        TRACE_ERROR(("Service not running!\n"));
        return frost_false;
    }

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    // TODO: Call actual initBackend on the backend object
    // For now, just return success
    // Example:
    // auto* backend = static_cast<OperaMuminMediaBackend*>(it->second);
    // return backend->initBackend(origin.c_str(), ...) == UVA_OK ? frost_true : frost_false;

    TRACE_ALWAYS(("=== InitBackend SUCCESS ===\n"));
    return frost_true;
}

// ============================================================================
// IPC Handler: Create Backend
// ============================================================================

static intptr_t handle_MuminCreateBackend(
    frost_int backend_type,
    intptr_t client_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminCreateBackend ===\n"));
    TRACE_ALWAYS(("    backend_type=%d, client_ptr=%ld\n",
                 backend_type, client_ptr));

    if (!s_mumin_service_running) {
        TRACE_ERROR(("Service not running!\n"));
        return 0;
    }

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    // Create backend instance structure
    TRACE_ALWAYS(("Creating backend instance...\n"));
    MuminBackendInstance* backend = new MuminBackendInstance();

    // Create MediaDataSource and MediaDataSink for this backend
    backend->data_source = new MediaDataSource();
    backend->data_sink = new MediaDataSink();

    // Create AVControlObject for this backend
    TRACE_ALWAYS(("Creating AVControlObject for backend...\n"));
    backend->av_control_obj = new AVControlObject(
        *s_media_thread,
        AVControlObject::StreamingType::progressive,
        *backend->data_source,
        *backend->data_sink,
        *s_event_generator,
        nullptr
    );

    if (!backend->av_control_obj || !backend->av_control_obj->isInitialised())
    {
        TRACE_ERROR(("Failed to create AVControlObject\n"));
        delete backend;
        return 0;
    }

    TRACE_ALWAYS(("Preparing AVControlObject for playback...\n"));
    if (!backend->av_control_obj->prepareForPlayback())
    {
        TRACE_ERROR(("prepareForPlayback failed\n"));
        delete backend;
        return 0;
    }

    // Assign a handle
    intptr_t handle = s_next_backend_handle.fetch_add(1);
    s_backend_instances[handle] = backend;

    TRACE_ALWAYS(("=== Backend Created: handle=%ld, AVControlObject=%p ===\n",
                 handle, (void*)backend->av_control_obj));
    return handle;
}

// ============================================================================
// IPC Handler: Discard Backend
// ============================================================================

static frost_bool handle_MuminDiscardBackend(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminDiscardBackend ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_WARN(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    // TODO: Properly delete the backend object
    // delete static_cast<SomeBackendType*>(it->second);

    s_backend_instances.erase(it);

    TRACE_ALWAYS(("=== Backend Discarded ===\n"));
    return frost_true;
}

// ============================================================================
// IPC Handler: Set Source
// ============================================================================

static frost_bool handle_MuminSetSource(
    intptr_t backend_handle,
    const std::string& url)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminSetSource ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld, url=%s\n",
                 backend_handle, url.c_str()));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance\n"));
        return frost_false;
    }

    TRACE_ALWAYS(("Setting source on AVControlObject %p...\n", (void*)backend->av_control_obj));
    bool result = backend->av_control_obj->setSource(url.c_str());

    TRACE_ALWAYS(("=== SetSource %s ===\n", result ? "SUCCESS" : "FAILED"));
    return result ? frost_true : frost_false;
}

// ============================================================================
// IPC Handler: Play
// ============================================================================

static frost_bool handle_MuminPlay(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminPlay ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    // TODO: Call play on the backend
    // auto* backend = static_cast<OperaMuminMediaBackend*>(it->second);
    // return backend->play() == UVA_OK ? frost_true : frost_false;

    TRACE_ALWAYS(("=== Play SUCCESS ===\n"));
    return frost_true;
}

// ============================================================================
// IPC Handler: Stop
// ============================================================================

static frost_bool handle_MuminStop(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminStop ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    // TODO: Call stop on the backend
    // auto* backend = static_cast<OperaMuminMediaBackend*>(it->second);
    // return backend->stop() == UVA_OK ? frost_true : frost_false;

    TRACE_ALWAYS(("=== Stop SUCCESS ===\n"));
    return frost_true;
}

// ============================================================================
// Bind All Handlers to RPC Server
// ============================================================================

void NebulaBrowserBackendIpcAurora::bindToServer(rpc::server& server)
{
    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("NebulaBrowserBackendIpcAurora::bindToServer - START\n"));
    TRACE_ALWAYS(("=================================================\n"));

    // IMPORTANT: Reset state when binding to ensure clean start
    TRACE_ALWAYS(("Resetting state before binding...\n"));
    {
        const std::lock_guard<std::mutex> lock(s_service_mutex);
        s_mumin_service_running = false;
        s_media_thread = nullptr;
        TRACE_ALWAYS(("State reset: service=false, av_obj=null, thread=null\n"));
    }

    // Service management
    server.bind(IPC_NAME(NEBULA_MuminStartService), handle_MuminStartService);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminStartService\n"));

    server.bind(IPC_NAME(NEBULA_MuminStopService), handle_MuminStopService);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminStopService\n"));

    server.bind(IPC_NAME(NEBULA_MuminGetStatus), handle_MuminGetStatus);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminGetStatus\n"));

    // Backend lifecycle
    server.bind(IPC_NAME(NEBULA_MuminCreateBackend), handle_MuminCreateBackend);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminCreateBackend\n"));

    server.bind(IPC_NAME(NEBULA_MuminInitBackend), handle_MuminInitBackend);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminInitBackend\n"));

    server.bind(IPC_NAME(NEBULA_MuminDiscardBackend), handle_MuminDiscardBackend);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminDiscardBackend\n"));

    // Playback control
    server.bind(IPC_NAME(NEBULA_MuminSetSource), handle_MuminSetSource);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminSetSource\n"));

    server.bind(IPC_NAME(NEBULA_MuminPlay), handle_MuminPlay);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminPlay\n"));

    server.bind(IPC_NAME(NEBULA_MuminStop), handle_MuminStop);
    TRACE_ALWAYS(("  Bound: NEBULA_MuminStop\n"));

    TRACE_ALWAYS(("=================================================\n"));
    TRACE_ALWAYS(("NebulaBrowserBackendIpcAurora::bindToServer - COMPLETE\n"));
    TRACE_ALWAYS(("=================================================\n"));
}

void NebulaBrowserBackendIpcAurora::reset()
{
    TRACE_ALWAYS(("NebulaBrowserBackendIpcAurora::reset() - START\n"));

    // Force reset the service state
    {
        const std::lock_guard<std::mutex> lock(s_service_mutex);
        s_mumin_service_running = false;
        TRACE_ALWAYS(("Service state reset to false\n"));
    }

    // Clean up any registered callbacks or state
    handle_MuminStopService();

    TRACE_ALWAYS(("NebulaBrowserBackendIpcAurora::reset() - COMPLETE\n"));
}
