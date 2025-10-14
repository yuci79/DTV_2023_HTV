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

#include "core/browser_client/AVControlObject.hpp"

#include "utilities_debug/trace.h"
#include "AnyCommandThread.hpp"
#include "MediaDataSource.hpp"
#include "MediaDataSink.hpp"
#include "AnyAVControlObjectEventGenerator.hpp"
#include "nebula/core/browser_client/AVControlObjectEventGenerator.hpp"

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

// Service state
static bool s_mumin_service_running = false;
static std::mutex s_service_mutex;

// Backend instance management
static std::map<intptr_t, void*> s_backend_instances;
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
// IPC Handler: Start Mumin Service
// ============================================================================

static frost_bool handle_MuminStartService(frost_int reserved)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminStartService ===\n"));
    TRACE_ALWAYS(("reserved=%d\n", reserved));

    const std::lock_guard<std::mutex> lock(s_service_mutex);
    if (s_mumin_service_running)
    {
        TRACE_INFO(("Mumin service already running\n"));
        return frost_true;
    }

    // Step 1. Create the command thread for A/V control
    static AnyCommandThread* s_media_thread = nullptr;
    if (!s_media_thread)
    {
        s_media_thread = new AnyCommandThread();
        if (!s_media_thread->initialise("MediaCmdThread", FROST_high_priority, 64 * 1024))
        {
            TRACE_ERROR(("Failed to initialize media thread\n"));
            return frost_false;
        }
    }

    // Step 2. Prepare source/sink placeholders
    static MediaDataSource media_source;
    static MediaDataSink media_sink;
    static AVControlObjectEventGenerator event_gen;

    // Step 3. Create AVControlObject
    TRACE_ALWAYS(("Creating AVControlObject...\n"));
    static AVControlObject* s_av_obj = nullptr;
    s_av_obj = new AVControlObject(
        *s_media_thread,
        AVControlObject::StreamingType::progressive,
        media_source,
        media_sink,
        event_gen,
        nullptr
    );

    if (!s_av_obj || !s_av_obj->isInitialised())
    {
        TRACE_ERROR(("AVControlObject initialisation failed\n"));
        return frost_false;
    }

    // Step 4. Set test source and play
    TRACE_ALWAYS(("Setting test URL...\n"));
    bool st = s_av_obj->setSource("http://10.135.6.54/test.mp4");
    if (st != true)
    {
        TRACE_ERROR(("setSource failed: %d\n", st));
    }

    TRACE_ALWAYS(("Calling play()...\n"));
    s_av_obj->play();

    s_mumin_service_running = true;
    TRACE_ALWAYS(("=== AVControlObject playback started successfully ===\n"));
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

    // Create the actual backend object
    // TODO: Implement actual backend creation
    // For now, create a placeholder

    void* backend_obj = nullptr;

    // Example of what this should do:
    // void* client = from_intptr(client_ptr);
    // auto* factory = OperaMuminObjectFactory::instance();
    // backend_obj = factory->createMuminBackend(backend_type, static_cast<UVAClient*>(client));

    if (!backend_obj) {
        TRACE_ERROR(("Failed to create backend\n"));
        return 0;
    }

    // Assign a handle
    intptr_t handle = s_next_backend_handle.fetch_add(1);
    s_backend_instances[handle] = backend_obj;

    TRACE_ALWAYS(("=== Backend Created: handle=%ld ===\n", handle));
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

    // TODO: Call setSource on the backend
    // auto* backend = static_cast<OperaMuminMediaBackend*>(it->second);
    // return backend->setSource(url.c_str()) == UVA_OK ? frost_true : frost_false;

    TRACE_ALWAYS(("=== SetSource SUCCESS ===\n"));
    return frost_true;
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
    TRACE_INFO(("NebulaBrowserBackendIpcAurora::reset()\n"));

    // Clean up any registered callbacks or state
    handle_MuminStopService();
}
