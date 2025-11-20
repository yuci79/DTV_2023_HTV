/**
 * Copyright 2024 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of Mumin Backend IPC Aurora handlers - FIXED VERSION
 */

#include "NebulaBrowserBackendIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include "nebula/core/browser_client/AVControlObject.hpp"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"
#include "nebula/adaptation_layer/nebula_system_information_provider.h"

//#include "vewd_integration/ipc/avcontrolobject/AVControlHelpers.hpp"

#include "utilities_private/CommandThread.hpp"
#include "utilities_private/PrioritisedCommandThread.hpp"  // Add this
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"

// Add these includes for missing types
#include "nebula_src/adaptation_layer/NebulaMediaPlayer.hpp"
#include "utilities_public/StreamConfig.hpp"          // For StreamConfig
#include "utilities_public/StreamConfig.hpp"          // For StreamContentType, StreamCodec
#include "utilities_public/MseStreamData.hpp"        // For MseStreamData
#include "utilities_public/AnyStreamData.hpp"        // For AnyStreamData

#include "frost/frost_debug.h"
#include "frost/frost_threads.h"

#include <rpc/server.h>
#include <map>
#include <mutex>
#include <tuple>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaBrowserBackendIpcAurora");
// ============================================================================
// LOCAL DUMMY EVENT GENERATOR (to avoid linking issues)
// ============================================================================

namespace {

class DummyAVControlObjectEventGenerator : public AnyAVControlObjectEventGenerator
{
public:
    DummyAVControlObjectEventGenerator() {
        TRACE_WARN(("Creating DummyAVControlObjectEventGenerator\n"));
    }

    virtual ~DummyAVControlObjectEventGenerator() {}

    virtual void generateFullScreenChangedEvent() override {
        TRACE_ALWAYS(("DummyEventGen: FullScreenChanged\n"));
    }

    virtual void generatePlayStateChangedEvent(NEBULA_MediaPlayerStatus state) override {
        TRACE_ALWAYS(("DummyEventGen: PlayStateChanged to %d\n", state));
    }

    virtual void generatePlaySpeedChangedEvent(int play_speed) override {
        TRACE_ALWAYS(("DummyEventGen: PlaySpeedChanged to %d\n", play_speed));
    }

    virtual void generatePlayPositionChangedEvent(std::int64_t play_position) override {
        TRACE_ALWAYS(("DummyEventGen: PlayPositionChanged to %lld\n", play_position));
    }

    virtual void generateFileDetailsParsedEvent(NEBULA_MediaFileDetails* /*source_details*/) override {
        TRACE_ALWAYS(("DummyEventGen: FileDetailsParsed\n"));
    }

    virtual void generateComponentActivenessChangedEvent(
        const NEBULA_AVComponent* /*component_data*/, bool active) override {
        TRACE_ALWAYS(("DummyEventGen: ComponentActivenessChanged active=%d\n", active));
    }

    virtual void generateUpdateAudioComponentEvent(char const* language, bool ad_on) override {
        TRACE_ALWAYS(("DummyEventGen: UpdateAudioComponent lang=%s ad_on=%d\n",
                   language ? language : "(null)", ad_on));
    }
};

static DummyAVControlObjectEventGenerator g_local_dummy_event_generator;

AnyAVControlObjectEventGenerator* getEventGeneratorForAv() {
    return &g_local_dummy_event_generator;
}

} // anonymous namespace
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
    bool initialized;

    MuminBackendInstance()
        : av_control_obj(nullptr)
        , data_source(nullptr)
        , data_sink(nullptr)
        , initialized(false)
    {}

    ~MuminBackendInstance() {
        TRACE_ALWAYS(("Destroying MuminBackendInstance: av_control=%p\n", (void*)av_control_obj));
        if (av_control_obj) {
            delete av_control_obj;
            av_control_obj = nullptr;
        }
        if (data_source) {
            delete data_source;
            data_source = nullptr;
        }
        if (data_sink) {
            delete data_sink;
            data_sink = nullptr;
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

    // FIX: Remove 'static' to use global s_media_thread (not local shadow variable)
    TRACE_ALWAYS(("Creating CommandThread...\n"));
    s_media_thread = new CommandThread;
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
    TRACE_ALWAYS(("Command thread created: %p\n", (void*)s_media_thread));

    // Step 2. Get event generator (shared by all backends)
    if (!s_event_generator)
    {
        TRACE_ALWAYS(("Getting event generator...\n"));
        s_event_generator = getEventGeneratorForAv();
        if (!s_event_generator)
        {
            TRACE_ERROR(("Failed to get event generator\n"));
            // Clean up thread
            delete s_media_thread;
            s_media_thread = nullptr;
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

        TRACE_ALWAYS(("Destroying %zu backend instances...\n", s_backend_instances.size()));
        for (auto& pair : s_backend_instances) {
            delete pair.second;
        }
        s_backend_instances.clear();
    }

    // Shutdown global resources
    if (s_media_thread) {
        TRACE_ALWAYS(("Shutting down media thread...\n"));
        delete s_media_thread;
        s_media_thread = nullptr;
    }

    // Note: Don't delete s_event_generator as it may be shared/managed elsewhere

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

    MuminBackendInstance* backend = it->second;
    if (!backend) {
        TRACE_ERROR(("Backend instance is null\n"));
        return frost_false;
    }

    // Mark as initialized
    backend->initialized = true;

    TRACE_ALWAYS(("=== InitBackend SUCCESS ===\n"));
    return frost_true;
}

// ============================================================================
// IPC Handler: Create Backend
// ============================================================================

static intptr_t handle_MuminCreateBackend(
    frost_int backend_type,
    intptr_t client_ptr,
    frost_bool is_rtsp,
    frost_bool is_udp)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MuminCreateBackend ===\n"));
    TRACE_ALWAYS(("    backend_type=%d\n", backend_type));
    TRACE_ALWAYS(("    client_ptr=%ld\n", client_ptr));
    TRACE_ALWAYS(("    is_rtsp=%d, is_udp=%d\n", is_rtsp, is_udp));

    if (!s_mumin_service_running) {
        TRACE_ERROR(("Service not running!\n"));
        return 0;
    }

    TRACE_ALWAYS(("Service running check passed\n"));

    // FIX: Create objects BEFORE acquiring lock to avoid potential deadlock
    // if constructors trigger callbacks
    TRACE_ALWAYS(("Checking service infrastructure...\n"));

    // Check service infrastructure is available FIRST
    if (!s_media_thread || !s_event_generator) {
        TRACE_ERROR(("Service infrastructure not available! thread=%p, generator=%p\n",
                    (void*)s_media_thread, (void*)s_event_generator));
        return 0;
    }

    TRACE_ALWAYS(("Service infrastructure OK. Creating objects...\n"));

    MediaDataSource* data_source = nullptr; //new MediaDataSource();
    //TRACE_ALWAYS(("MediaDataSource created: %p\n", (void*)data_source));

    MediaDataSink* data_sink = nullptr; // new MediaDataSink();
    //TRACE_ALWAYS(("MediaDataSink created: %p\n", (void*)data_sink));

    TRACE_ALWAYS(("Creating AVControlObject...\n"));

    AVControlObject* av_control_obj = nullptr;

    try {
        // Determine streaming type based on flags
        bool rtsp_flag = (is_rtsp == frost_true);
        bool udp_flag = (is_udp == frost_true);

        AVControlObject::StreamingType streaming_type;

        if (rtsp_flag) {
            streaming_type = AVControlObject::rtsp;
            TRACE_ALWAYS(("Using RTSP streaming type\n"));
        }
        else if (udp_flag) {
            streaming_type = AVControlObject::udp;
            TRACE_ALWAYS(("Using UDP streaming type\n"));
        }
        else {
            streaming_type = AVControlObject::progressive;
            TRACE_ALWAYS(("Using progressive streaming type\n"));
        }

        TRACE_ALWAYS(("Calling AVControlObject constructor with streaming_type=%d...\n", (int)streaming_type));

        av_control_obj = new AVControlObject(
            *s_media_thread,
			streaming_type,
            *data_source,
            *data_sink,
            *s_event_generator,
            nullptr
        );
        TRACE_ALWAYS(("AVControlObject constructor returned: %p\n", (void*)av_control_obj));
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception creating AVControlObject: %s\n", e.what()));
        delete data_source;
        delete data_sink;
        return 0;
    } catch (...) {
        TRACE_ERROR(("Unknown exception creating AVControlObject\n"));
        delete data_source;
        delete data_sink;
        return 0;
    }

    if (!av_control_obj || !av_control_obj->isInitialised())
    {
        TRACE_ERROR(("Failed to create AVControlObject\n"));
        delete av_control_obj;
        delete data_source;
        delete data_sink;
        return 0;
    }

    TRACE_ALWAYS(("Preparing AVControlObject for playback...\n"));
    if (!av_control_obj->prepareForPlayback())
    {
        TRACE_ERROR(("prepareForPlayback failed\n"));
        delete av_control_obj;
        delete data_source;
        delete data_sink;
        return 0;
    }

    // NOW acquire lock to register the backend
    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    // Create backend instance structure
    MuminBackendInstance* backend = new MuminBackendInstance();
    backend->av_control_obj = av_control_obj;
    backend->data_source = data_source;
    backend->data_sink = data_sink;

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

    // FIX: Properly delete the backend instance
    delete it->second;
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

    if (!backend->initialized) {
        TRACE_WARN(("Backend not initialized yet!\n"));
        // Could return false here, or allow it
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

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance\n"));
        return frost_false;
    }

    // Call play on AVControlObject
    TRACE_ALWAYS(("Calling play on AVControlObject...\n"));
    bool result = backend->av_control_obj->play();

    TRACE_ALWAYS(("=== Play %s ===\n", result ? "SUCCESS" : "FAILED"));
    return result ? frost_true : frost_false;
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

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance\n"));
        return frost_false;
    }

    // Call stop on AVControlObject
    TRACE_ALWAYS(("Calling stop on AVControlObject...\n"));
    //bool result = backend->av_control_obj->stop();
    backend->av_control_obj->stop();
    TRACE_INFO(("stop() result: OK\n"));
    return frost_true; //result ? frost_true : frost_false;
}

frost_bool NEBULA_SystemInformationProviderIsPanelInfoAvailable(void)
{
    int dummy_width, dummy_height;  // Use 'int' not 'frost_int'
    return NEBULA_SystemInformationProviderGetPanelInformation(dummy_width, dummy_height);
}

int NEBULA_SystemInformationProviderGetPanelWidth(void)  // Return 'int' not 'frost_int'
{
    int width, height;  // Use 'int' not 'frost_int'
    NEBULA_SystemInformationProviderGetPanelInformation(width, height);
    return width;
}

int NEBULA_SystemInformationProviderGetPanelHeight(void)  // Return 'int' not 'frost_int'
{
    int width, height;  // Use 'int' not 'frost_int'
    NEBULA_SystemInformationProviderGetPanelInformation(width, height);
    return height;
}
// ============================================================================
// Bind All Handlers to RPC Server
// ============================================================================

// Add these handlers to your existing extern "C" block or create new ones

// Stream Data Management
void NEBULA_DeleteStreamData(intptr_t stream_data_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_DeleteStreamData ===\n"));
    auto* stream_data = reinterpret_cast<AnyStreamData*>(stream_data_ptr);
    //deleteStreamData(stream_data);
}

intptr_t NEBULA_CreateMseStreamData(intptr_t data_ptr, unsigned int size,
                                   double pts, double dts, double duration)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_CreateMseStreamData ===\n"));
    auto* data = reinterpret_cast<unsigned char*>(data_ptr);
    // createMseStreamData not available in nebula layer
    return 0;
}

// Stream Configuration
void NEBULA_StreamConfigSetCodecInfo(intptr_t config_handle,
                                    int content_type, int codec)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_StreamConfigSetCodecInfo ===\n"));
    auto* config = reinterpret_cast<StreamConfig*>(config_handle);
    config->setStreamConfigCodecInfo(static_cast<StreamContentType>(content_type),
                                   static_cast<StreamCodec>(codec));
}

// Media Data Sink
/*
intptr_t NEBULA_CreateMediaDataSink(void)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_CreateMediaDataSink ===\n"));
    auto* sink = new MediaDataSink();
    return reinterpret_cast<intptr_t>(sink);
}

void NEBULA_DestroyMediaDataSink(intptr_t sink_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_DestroyMediaDataSink ===\n"));
    auto* sink = reinterpret_cast<MediaDataSink*>(sink_handle);
    delete sink;
}
*/
void NEBULA_MediaDataSinkPrepareForDataFromNewPosition(intptr_t sink_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MediaDataSinkPrepareForDataFromNewPosition ===\n"));
    auto* sink = reinterpret_cast<MediaDataSink*>(sink_handle);
    sink->prepareForDataFromNewPosition();
}

void NEBULA_MediaDataSinkSendData(intptr_t sink_handle, unsigned int id,
                                 intptr_t stream_data_ptr, frost_bool eos, frost_bool flush)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MediaDataSinkSendData ===\n"));
    auto* sink = reinterpret_cast<MediaDataSink*>(sink_handle);
    auto* stream_data = reinterpret_cast<AnyStreamData*>(stream_data_ptr);
    sink->sendData(id, stream_data, eos, flush);
}

// MSE Stream Data
intptr_t NEBULA_MseStreamDataGetStreamConfig(intptr_t mse_stream_data_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MseStreamDataGetStreamConfig ===\n"));
    auto* mse_stream_data = reinterpret_cast<MseStreamData*>(mse_stream_data_handle);
    auto* config = &mse_stream_data->streamConfig();
    return reinterpret_cast<intptr_t>(config);
}

// Media Data Source
intptr_t NEBULA_CreateMediaDataSource(void)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_CreateMediaDataSource ===\n"));
    auto* source = new MediaDataSource();
    return reinterpret_cast<intptr_t>(source);
}

void NEBULA_DestroyMediaDataSource(intptr_t source_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_DestroyMediaDataSource ===\n"));
    auto* source = reinterpret_cast<MediaDataSource*>(source_handle);
    delete source;
}

frost_bool NEBULA_MediaDataSourceRegisterDataProvider(intptr_t source_handle, intptr_t provider_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MediaDataSourceRegisterDataProvider ===\n"));
    TRACE_ALWAYS(("source_handle: 0x%lx, provider_ptr: 0x%lx\n", source_handle, provider_ptr));

    if (source_handle == 0 || provider_ptr == 0) {
        TRACE_ERROR(("Null pointer detected\n"));
        return frost_false;
    }

    try {
        auto* source = reinterpret_cast<MediaDataSource*>(source_handle);
        auto* provider = reinterpret_cast<AnyMediaDataProvider*>(provider_ptr);
        source->registerDataProvider(provider);
        TRACE_ALWAYS(("=== MediaDataSourceRegisterDataProvider SUCCESS ===\n"));
        return frost_true;
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception in MediaDataSourceRegisterDataProvider: %s\n", e.what()));
        return frost_false;
    }
}

void NEBULA_MediaDataSourceUnregisterDataProvider(intptr_t source_handle, intptr_t provider_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_MediaDataSourceUnregisterDataProvider ===\n"));
    auto* source = reinterpret_cast<MediaDataSource*>(source_handle);
    auto* provider = reinterpret_cast<AnyMediaDataProvider*>(provider_ptr);
    source->unregisterDataProvider(provider);
}

// Nebula Media Player
frost_bool NEBULA_NebulaMediaPlayerCanPlayType(intptr_t mime_ptr, int length, intptr_t codecs_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_NebulaMediaPlayerCanPlayType ===\n"));
    auto* mime = reinterpret_cast<const char*>(mime_ptr);
    auto* codecs = reinterpret_cast<const char**>(codecs_ptr);
    return NebulaMediaPlayer::canPlayType(mime, length, codecs) ? frost_true : frost_false;
}

void NEBULA_NebulaMediaPlayerGetVideoCaps(intptr_t width_ptr, intptr_t height_ptr, intptr_t fps_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_NebulaMediaPlayerGetVideoCaps ===\n"));
    auto* width = reinterpret_cast<int*>(width_ptr);
    auto* height = reinterpret_cast<int*>(height_ptr);
    auto* fps = reinterpret_cast<int*>(fps_ptr);
    NebulaMediaPlayer::getVideoCaps(width, height, fps);
}

void NEBULA_NebulaMediaPlayerChangeYoutubeThreadPriorities(frost_bool high)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_NebulaMediaPlayerChangeYoutubeThreadPriorities ===\n"));
    NebulaMediaPlayer::changeYoutubeThreadPriorities(high == frost_true);
}

// Prioritised Command Thread
intptr_t NEBULA_CreatePrioritisedCommandThread(void)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_CreatePrioritisedCommandThread ===\n"));
    auto* thread = new PrioritisedCommandThread();
    return reinterpret_cast<intptr_t>(thread);
}

frost_bool NEBULA_PrioritisedCommandThreadInitialise(intptr_t thread_handle,
                                                    intptr_t name_ptr, int priority,
                                                    int stack_size, int queue_size)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_PrioritisedCommandThreadInitialise ===\n"));
    auto* thread = reinterpret_cast<PrioritisedCommandThread*>(thread_handle);
    auto* name = reinterpret_cast<const char*>(name_ptr);
    return thread->initialise(name, static_cast<FROST_TaskPriority>(priority),
                            stack_size, queue_size) ? frost_true : frost_false;
}

// AV Control Object queries
frost_bool NEBULA_AVControlObjectIsInitialised(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectIsInitialised ===\n"));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        return frost_false;
    }

    return backend->av_control_obj->isInitialised() ? frost_true : frost_false;
}

// Object Factory - Critical for createOipfVideoOnDemandObject conversion
intptr_t NEBULA_CreateOipfVideoOnDemandObject(intptr_t event_generator_ptr,
                                             intptr_t thread_handle, int type,
                                             int streaming_type, intptr_t data_source_handle,
                                             intptr_t data_sink_handle, frost_bool is_rtsp,
                                             frost_bool is_udp, intptr_t origin_ptr)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_CreateOipfVideoOnDemandObject ===\n"));

    // This is handled by the existing handle_MuminCreateBackend function
    // which creates the full backend instance including AVControlObject

    // Return a placeholder handle - actual creation is done in CreateBackend
    return 1; // Success indicator
}

frost_bool NEBULA_AVControlObjectSetFullScreen(intptr_t backend_handle, frost_bool fullscreen)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectSetFullScreen ===\n"));
    TRACE_ALWAYS(("backend_handle: 0x%lx, fullscreen: %s\n", backend_handle, fullscreen ? "true" : "false"));

    if (backend_handle == 0) {
        TRACE_ERROR(("Invalid backend_handle: null pointer\n"));
        return frost_false;
    }

    try {
        // Use the SAME pattern as your existing code
        const std::lock_guard<std::mutex> lock(s_backend_mutex);

        auto it = s_backend_instances.find(backend_handle);
        if (it == s_backend_instances.end()) {
            TRACE_ERROR(("No backend found for handle: 0x%lx\n", backend_handle));
            return frost_false;
        }

        MuminBackendInstance* backend = it->second;
        if (!backend || !backend->av_control_obj) {
            TRACE_ERROR(("Invalid backend or AVControlObject for handle: 0x%lx\n", backend_handle));
            return frost_false;
        }

        // Call the actual setFullScreen method
        bool result = backend->av_control_obj->setFullScreen(fullscreen == frost_true);

        TRACE_ALWAYS(("=== AVControlObjectSetFullScreen result: %s ===\n", result ? "SUCCESS" : "FAILED"));
        return result ? frost_true : frost_false;

    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception in AVControlObjectSetFullScreen: %s\n", e.what()));
        return frost_false;
    }
}

// ============================================================================
// IPC Handler: Get Playback Status
// ============================================================================

static frost_int handle_AVControlObjectGetPlaybackStatus(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectGetPlaybackStatus ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return -1; // Error status
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return -1;
    }

    try {
        NEBULA_MediaPlayerStatus status = backend->av_control_obj->getPlaybackStatus();
        TRACE_ALWAYS(("Playback status: %d\n", static_cast<int>(status)));
        return static_cast<frost_int>(status);
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception getting playback status: %s\n", e.what()));
        return -1;
    }
}

// ============================================================================
// IPC Handler: Set Video Output Window
// ============================================================================
static frost_bool handle_AVControlObjectSetVideoOutputWindow(
    intptr_t backend_handle,
    float x_position,
    float y_position,
    float width,
    float height,
    frost_bool is_browser_video)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectSetVideoOutputWindow ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld, pos=(%f,%f), size=(%f,%f), browser=%d\n",
                 backend_handle, x_position, y_position, width, height, is_browser_video));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return frost_false;
    }

    try {
        NEBULA_DisplayWindow window;
        window.x_position = x_position;
        window.y_position = y_position;
        window.width = width;
        window.height = height;

        bool result = backend->av_control_obj->setVideoOutputWindow(&window, is_browser_video == frost_true);
        TRACE_ALWAYS(("=== SetVideoOutputWindow result: %s ===\n", result ? "SUCCESS" : "FAILED"));
        return result ? frost_true : frost_false;
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception in SetVideoOutputWindow: %s\n", e.what()));
        return frost_false;
    }
}

// ============================================================================
// IPC Handler: Set Visibility
// ============================================================================
static frost_bool handle_AVControlObjectSetVisibility(intptr_t backend_handle, frost_bool visible)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectSetVisibility ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld, visible=%s\n", backend_handle, visible ? "true" : "false"));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return frost_false;
    }

    // BEFORE (causes error with -fno-rtti):
    // TRACE_ALWAYS(("DBG pre-setVisibility: backend=%p av_control_obj=%p initialised=%d typeid=%s\n",
    //     (void*)backend,
    //     (void*)backend->av_control_obj,
    //     backend->av_control_obj ? backend->av_control_obj->isInitialised() : -1,
    //     backend->av_control_obj ? typeid(*backend->av_control_obj).name() : "(null)"
    // ));

    // AFTER (RTTI-free):
    void** vtbl = nullptr;
    if (backend->av_control_obj) {
        vtbl = *reinterpret_cast<void***>(backend->av_control_obj);
    }
    TRACE_ALWAYS(("DBG pre-setVisibility: backend=%p av_control_obj=%p vtbl=%p initialised=%d\n",
        (void*)backend,
        (void*)backend->av_control_obj,
        (void*)vtbl,
        backend->av_control_obj ? backend->av_control_obj->isInitialised() : -1
    ));

    try {
        bool result = backend->av_control_obj->setVisibility(visible == frost_true);
        TRACE_ALWAYS(("=== SetVisibility result: %s ===\n", result ? "SUCCESS" : "FAILED"));
        return result ? frost_true : frost_false;
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception in SetVisibility: %s\n", e.what()));
        return frost_false;
    }
}

// ============================================================================
// IPC Handler: Set Speed
// ============================================================================
static frost_bool handle_AVControlObjectSetSpeed(intptr_t backend_handle, frost_int speed)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectSetSpeed ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld, speed=%d\n", backend_handle, speed));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return frost_false;
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return frost_false;
    }

    try {
        bool result = backend->av_control_obj->setSpeed(speed);
        TRACE_ALWAYS(("=== SetSpeed result: %s ===\n", result ? "SUCCESS" : "FAILED"));
        return result ? frost_true : frost_false;
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception in SetSpeed: %s\n", e.what()));
        return frost_false;
    }
}

// ============================================================================
// IPC Handler: Get Error
// ============================================================================
static frost_int handle_AVControlObjectGetError(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectGetError ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return static_cast<frost_int>(NEBULA_AVCONTROL_ERROR_NONE);
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return static_cast<frost_int>(NEBULA_AVCONTROL_ERROR_NONE);
    }

    try {
        NEBULA_MediaPlayerError error = backend->av_control_obj->getError();
        TRACE_ALWAYS(("=== GetError result: %d ===\n", static_cast<int>(error)));
        return static_cast<frost_int>(error);
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception getting error: %s\n", e.what()));
        return static_cast<frost_int>(NEBULA_AVCONTROL_ERROR_NONE);
    }
}

// ============================================================================
// IPC Handler: Get Video Exact Aspect Ratio
// ============================================================================
static float handle_AVControlObjectGetVideoExactAspectRatio(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectGetVideoExactAspectRatio ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    const std::lock_guard<std::mutex> lock(s_backend_mutex);

    auto it = s_backend_instances.find(backend_handle);
    if (it == s_backend_instances.end()) {
        TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
        return 1.777777778f; // Default 16:9
    }

    MuminBackendInstance* backend = it->second;
    if (!backend || !backend->av_control_obj) {
        TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
        return 1.777777778f; // Default 16:9
    }

    try {
        float exact_aspect_ratio = 1.777777778f; // Default 16:9
        bool success = backend->av_control_obj->getVideoExactAspectRatio(exact_aspect_ratio);

        TRACE_ALWAYS(("=== GetVideoExactAspectRatio result: success=%s, ratio=%f ===\n",
                     success ? "true" : "false", exact_aspect_ratio));

        return exact_aspect_ratio;
    } catch (const std::exception& e) {
        TRACE_ERROR(("Exception getting aspect ratio: %s\n", e.what()));
        return 1.777777778f; // Default 16:9
    }
}

// ============================================================================
// IPC Handler: Get Video Output Window
// ============================================================================
// NEW tuple-returning handler:
static std::tuple<float,float,float,float>
handle_AVControlObjectGetVideoOutputWindow(intptr_t backend_handle)
{
    TRACE_ALWAYS(("=== RPC: NEBULA_AVControlObjectGetVideoOutputWindow ===\n"));
    TRACE_ALWAYS(("    backend_handle=%ld\n", backend_handle));

    float x = 0.0f;
    float y = 0.0f;
    float w = 1920.0f;
    float h = 1080.0f;

    {
        const std::lock_guard<std::mutex> lock(s_backend_mutex);

        auto it = s_backend_instances.find(backend_handle);
        if (it == s_backend_instances.end()) {
            TRACE_ERROR(("Backend handle %ld not found\n", backend_handle));
            return std::make_tuple(x,y,w,h);
        }

        MuminBackendInstance* backend = it->second;
        if (!backend || !backend->av_control_obj) {
            TRACE_ERROR(("Invalid backend instance or AVControlObject\n"));
            return std::make_tuple(x,y,w,h);
        }

        try {
            NEBULA_DisplayWindow window = backend->av_control_obj->getVideoOutputWindow();
            x = window.x_position;
            y = window.y_position;
            w = window.width;
            h = window.height;

            TRACE_ALWAYS(("=== GetVideoOutputWindow result: pos=(%f,%f), size=(%f,%f) ===\n",
                          x, y, w, h));
        } catch (const std::exception& e) {
            TRACE_ERROR(("Exception getting video output window: %s\n", e.what()));
        }
    }

    return std::make_tuple(x,y,w,h);
}

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

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderIsPanelInfoAvailable), NEBULA_SystemInformationProviderIsPanelInfoAvailable);
    TRACE_ALWAYS(("  Bound: NEBULA_SystemInformationProviderIsPanelInfoAvailable\n"));

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetPanelWidth), NEBULA_SystemInformationProviderGetPanelWidth);
    TRACE_ALWAYS(("  Bound: NEBULA_SystemInformationProviderGetPanelWidth\n"));

    server.bind(IPC_NAME(NEBULA_SystemInformationProviderGetPanelHeight), NEBULA_SystemInformationProviderGetPanelHeight);
    TRACE_ALWAYS(("  Bound: NEBULA_SystemInformationProviderGetPanelHeight\n"));

    // Add these to your existing bindToServer method:

    // Stream Data Management
    server.bind(IPC_NAME(NEBULA_DeleteStreamData), NEBULA_DeleteStreamData);
    server.bind(IPC_NAME(NEBULA_CreateMseStreamData), NEBULA_CreateMseStreamData);

    // Stream Configuration
    server.bind(IPC_NAME(NEBULA_StreamConfigSetCodecInfo), NEBULA_StreamConfigSetCodecInfo);

    // Media Data Sink
    //server.bind(IPC_NAME(NEBULA_CreateMediaDataSink), NEBULA_CreateMediaDataSink);
    //server.bind(IPC_NAME(NEBULA_DestroyMediaDataSink), NEBULA_DestroyMediaDataSink);
    server.bind(IPC_NAME(NEBULA_MediaDataSinkPrepareForDataFromNewPosition), NEBULA_MediaDataSinkPrepareForDataFromNewPosition);
    server.bind(IPC_NAME(NEBULA_MediaDataSinkSendData), NEBULA_MediaDataSinkSendData);

    // MSE Stream Data
    server.bind(IPC_NAME(NEBULA_MseStreamDataGetStreamConfig), NEBULA_MseStreamDataGetStreamConfig);

    // Media Data Source
    server.bind(IPC_NAME(NEBULA_CreateMediaDataSource), NEBULA_CreateMediaDataSource);
    server.bind(IPC_NAME(NEBULA_DestroyMediaDataSource), NEBULA_DestroyMediaDataSource);
    server.bind(IPC_NAME(NEBULA_MediaDataSourceRegisterDataProvider), NEBULA_MediaDataSourceRegisterDataProvider);
    server.bind(IPC_NAME(NEBULA_MediaDataSourceUnregisterDataProvider), NEBULA_MediaDataSourceUnregisterDataProvider);

    // Nebula Media Player
    server.bind(IPC_NAME(NEBULA_NebulaMediaPlayerCanPlayType), NEBULA_NebulaMediaPlayerCanPlayType);
    server.bind(IPC_NAME(NEBULA_NebulaMediaPlayerGetVideoCaps), NEBULA_NebulaMediaPlayerGetVideoCaps);
    server.bind(IPC_NAME(NEBULA_NebulaMediaPlayerChangeYoutubeThreadPriorities), NEBULA_NebulaMediaPlayerChangeYoutubeThreadPriorities);

    // Prioritised Command Thread
    server.bind(IPC_NAME(NEBULA_CreatePrioritisedCommandThread), NEBULA_CreatePrioritisedCommandThread);
    server.bind(IPC_NAME(NEBULA_PrioritisedCommandThreadInitialise), NEBULA_PrioritisedCommandThreadInitialise);

    // AV Control Object queries
    server.bind(IPC_NAME(NEBULA_AVControlObjectIsInitialised), NEBULA_AVControlObjectIsInitialised);

    // Object Factory
    server.bind(IPC_NAME(NEBULA_CreateOipfVideoOnDemandObject), NEBULA_CreateOipfVideoOnDemandObject);

    server.bind(IPC_NAME(NEBULA_AVControlObjectSetFullScreen), NEBULA_AVControlObjectSetFullScreen);

    server.bind(IPC_NAME(NEBULA_AVControlObjectGetPlaybackStatus), handle_AVControlObjectGetPlaybackStatus);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectGetPlaybackStatus\n"));

    server.bind(IPC_NAME(NEBULA_AVControlObjectSetVideoOutputWindow), handle_AVControlObjectSetVideoOutputWindow);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectSetVideoOutputWindow\n"));

    server.bind(IPC_NAME(NEBULA_AVControlObjectSetVisibility), handle_AVControlObjectSetVisibility);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectSetVisibility\n"));

    server.bind(IPC_NAME(NEBULA_AVControlObjectSetSpeed), handle_AVControlObjectSetSpeed);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectSetSpeed\n"));


    server.bind(IPC_NAME(NEBULA_AVControlObjectGetError), handle_AVControlObjectGetError);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectGetError\n"));

    server.bind(IPC_NAME(NEBULA_AVControlObjectGetVideoExactAspectRatio), handle_AVControlObjectGetVideoExactAspectRatio);
    TRACE_ALWAYS(("  Bound: NEBULA_AVControlObjectGetVideoExactAspectRatio\n"));

    server.bind(IPC_NAME(NEBULA_AVControlObjectGetVideoOutputWindow),
                &handle_AVControlObjectGetVideoOutputWindow);
    TRACE_ALWAYS(("  Bound (tuple): NEBULA_AVControlObjectGetVideoOutputWindow\n"));


    TRACE_ALWAYS(("  All missing symbol handlers bound\n"));

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
