/**
 * Copyright 2024 Vestel.  All rights reserved.
 *
 * @file OperaMuminMediaBackendIpc.cpp
 * @brief Implementation of IPC wrapper for media backend
 */

#include "OperaMuminMediaBackendIpc.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
#include "utilities_debug/trace.h"
#include "vewd_common/ipc/browser/VewdAuroraIpcHelper.hpp"

TRACE_IMPLEMENT("mumin/media_backend_ipc");

// Use the correct port from NebulaIpcConfiguration
//using MuminRpcClient = AuroraRpcClient::AuroraRpcClient<NEBULA_IPC_BACKEND_PORT, false>;

// ============================================================================
// Constructor / Destructor
// ============================================================================

OperaMuminMediaBackendIpc::OperaMuminMediaBackendIpc(
    UVABackendType backend_type,
    UVAClient* client)
    : UVAMediaBackendBase(backend_type)
    , m_client(client)
    , m_backend_handle(0)
    , m_initialized(false)
{
    TRACE_ALWAYS(("Creating IPC Media Backend: type=%d, client=%p\n", backend_type, client));
}

OperaMuminMediaBackendIpc::~OperaMuminMediaBackendIpc()
{
    TRACE_ALWAYS(("Destroying IPC Media Backend: handle=%d\n", (int)m_backend_handle));

    if (m_backend_handle != 0)
    {
        try {
            vewdAuroraRpcCall<frost_bool>(
                IPC_NAME(NEBULA_MuminDiscardBackend),
                m_backend_handle
            );
            TRACE_ALWAYS(("Backend %d discarded successfully\n", (int)m_backend_handle));
        } catch (const std::exception& e) {
            TRACE_ERROR(("Failed to discard backend: %s\n", e.what()));
        }
    }
}

// ============================================================================
// Backend Lifecycle
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::initBackend(
    const char* origin,
    UVABackend::SourceDescription** sources,
    bool automatic_key_system_selection)
{
    TRACE_ALWAYS(("IPC initBackend: origin=%s\n", origin ? origin : "null"));

    (void)sources;
    (void)automatic_key_system_selection;

    if (m_initialized) {
        TRACE_WARN(("Backend already initialized\n"));
        return UVA_OK;
    }

    try {
        // Step 1: Create backend instance on server side
        TRACE_ALWAYS(("Creating backend on server...\n"));

        m_backend_handle = vewdAuroraRpcCall<intptr_t>(
        		IPC_NAME(NEBULA_MuminCreateBackend),
            static_cast<frost_int>(getType()),
            reinterpret_cast<intptr_t>(m_client)
        );

        if (m_backend_handle == 0) {
            TRACE_ERROR(("Failed to create backend on server\n"));
            return UVA_ERROR;
        }

        TRACE_ALWAYS(("Backend created with handle: %d\n", (int)m_backend_handle));

        // Step 2: Initialize backend
        TRACE_ALWAYS(("Initializing backend...\n"));

        frost_bool init_ok = vewdAuroraRpcCall<frost_bool>(
            IPC_NAME(NEBULA_MuminInitBackend),
            m_backend_handle,
            std::string(origin ? origin : "")
        );

        if (init_ok != frost_true) {
            TRACE_ERROR(("Backend initialization failed\n"));
            return UVA_ERROR;
        }

        m_initialized = true;
        TRACE_ALWAYS(("Backend initialized successfully\n"));
        return UVA_OK;

    } catch (const std::exception& e) {
        TRACE_ERROR(("IPC initBackend failed: %s\n", e.what()));
        return UVA_ERROR;
    }
}

// ============================================================================
// Playback Control
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::load()
{
    TRACE_ALWAYS(("IPC load: handle=%d\n", (int)m_backend_handle));
    if (!m_initialized) return UVA_NOT_INITIALIZED;
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::play()
{
    TRACE_ALWAYS(("IPC play: handle=%d\n", (int)m_backend_handle));

    if (!m_initialized) return UVA_NOT_INITIALIZED;

    try {
        frost_bool play_ok = vewdAuroraRpcCall<frost_bool>(
            IPC_NAME(NEBULA_MuminPlay),
            m_backend_handle
        );

        if (play_ok == frost_true) {
            TRACE_ALWAYS(("Play succeeded\n"));
            return UVA_OK;
        } else {
            TRACE_ERROR(("Play failed\n"));
            return UVA_ERROR;
        }

    } catch (const std::exception& e) {
        TRACE_ERROR(("IPC play failed: %s\n", e.what()));
        return UVA_ERROR;
    }
}

UVA_STATUS OperaMuminMediaBackendIpc::stop()
{
    TRACE_ALWAYS(("IPC stop: handle=%d\n", (int)m_backend_handle));

    if (!m_initialized) return UVA_NOT_INITIALIZED;

    try {
        frost_bool stop_ok = vewdAuroraRpcCall<frost_bool>(
            IPC_NAME(NEBULA_MuminStop),
            m_backend_handle
        );

        return (stop_ok == frost_true) ? UVA_OK : UVA_ERROR;

    } catch (const std::exception& e) {
        TRACE_ERROR(("IPC stop failed: %s\n", e.what()));
        return UVA_ERROR;
    }
}

UVA_STATUS OperaMuminMediaBackendIpc::pause()
{
    TRACE_ALWAYS(("IPC pause: handle=%d\n", (int)m_backend_handle));
    if (!m_initialized) return UVA_NOT_INITIALIZED;
    TRACE_INFO(("Pause not yet implemented via IPC\n"));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::suspend()
{
    TRACE_ALWAYS(("IPC suspend: handle=%d\n", (int)m_backend_handle));
    return stop();
}

// ============================================================================
// Source Management
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::attachSource(UVABackend::SourceDescription* source)
{
    TRACE_ALWAYS(("IPC attachSource: source=%p\n", source));

    if (!m_initialized) return UVA_NOT_INITIALIZED;
    if (!source) return UVA_ERROR;

    if (source->type == UVABackend::SourceDescription::BACKEND_STREAMING)
    {
        const char* url = source->description.backend_streaming.url;
        TRACE_ALWAYS(("Setting source URL: %s\n", url ? url : "null"));
        if (!url) return UVA_ERROR;

        try {
            frost_bool source_ok = vewdAuroraRpcCall<frost_bool>(
                IPC_NAME(NEBULA_MuminSetSource),
                m_backend_handle,
                std::string(url)
            );

            if (source_ok == frost_true) {
                TRACE_ALWAYS(("SetSource succeeded\n"));
                return UVA_OK;
            } else {
                TRACE_ERROR(("SetSource failed\n"));
                return UVA_ERROR;
            }

        } catch (const std::exception& e) {
            TRACE_ERROR(("IPC setSource failed: %s\n", e.what()));
            return UVA_ERROR;
        }
    }

    TRACE_ERROR(("Unsupported source type: %d\n", source->type));
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminMediaBackendIpc::removeSource(uintptr_t source_id)
{
    TRACE_INFO(("IPC removeSource: source_id=%zu\n", source_id));
    return UVA_OK;
}

// ============================================================================
// Video Window / Display
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::setVideoWindowAndClip(
    const Rect& window_rect,
    const Rect& clip_rect)
{
    TRACE_INFO(("IPC setVideoWindowAndClip: (%d,%d,%d,%d)\n",
               window_rect.x, window_rect.y, window_rect.w, window_rect.h));
    
    (void)clip_rect; // Unused
    // TODO: Implement video window via IPC if needed
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setFullScreen(bool fullscreen)
{
    TRACE_INFO(("IPC setFullScreen: %d\n", fullscreen));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setVisibility(bool visible)
{
    TRACE_INFO(("IPC setVisibility: %d\n", visible));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setWindowId(uintptr_t window_id)
{
    TRACE_INFO(("IPC setWindowId: %zu\n", window_id));
    return UVA_OK;
}

// ============================================================================
// Playback Properties
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::setPlaybackRate(double rate)
{
    TRACE_INFO(("IPC setPlaybackRate: %.2f\n", rate));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setVolume(double volume)
{
    TRACE_INFO(("IPC setVolume: %.2f\n", volume));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::getDuration(double& duration)
{
    TRACE_VERBOSE(("IPC getDuration\n"));
    duration = 0.0;
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::getPosition(double& position)
{
    TRACE_VERBOSE(("IPC getPosition\n"));
    position = 0.0;
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setPosition(double position)
{
    TRACE_INFO(("IPC setPosition: %.2f\n", position));
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setLoop(int value)
{
    TRACE_INFO(("IPC setLoop: %d\n", value));
    return UVA_OK;
}

// ============================================================================
// Media Content / Capabilities
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::canPlayType(const ContentDescription& desc)
{
    TRACE_INFO(("IPC canPlayType: type=%s\n", desc.type.c_str()));
    // Assume all content can be played - backend will reject if not
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::getSupportedMimetypes(char**& mime_types)
{
    TRACE_INFO(("IPC getSupportedMimetypes\n"));
    (void)mime_types; // Unused
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminMediaBackendIpc::writeData(
    UVAEvent* need_data_event,
    size_t bytes_written,
    WriteDataStatus status)
{
    TRACE_VERBOSE(("IPC writeData: bytes=%zu\n", bytes_written));
    (void)need_data_event; // Unused
    (void)status; // Unused
    // For MSE streaming - not implemented yet
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::setContentSize(
    uintptr_t source_id,
    uva_uint64 content_size)
{
    TRACE_INFO(("IPC setContentSize: source=%zu, size=%llu\n", 
               source_id, (unsigned long long)content_size));
    return UVA_OK;
}

// ============================================================================
// Components / Tracks
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::selectComponent(AVComponent* component)
{
    TRACE_INFO(("IPC selectComponent: %p\n", component));
    return UVA_NOT_SUPPORTED;
}

UVA_STATUS OperaMuminMediaBackendIpc::unselectComponent(AVComponent* component)
{
    TRACE_INFO(("IPC unselectComponent: %p\n", component));
    return UVA_NOT_SUPPORTED;
}

// ============================================================================
// DRM
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::setKeySystem(const char* key_system_id)
{
    TRACE_INFO(("IPC setKeySystem: %s\n", key_system_id ? key_system_id : "null"));
    return UVA_OK;
}

// ============================================================================
// Buffering / Time Ranges
// ============================================================================

UVA_STATUS OperaMuminMediaBackendIpc::getBufferedTimeRanges(
    uintptr_t source_id,
    const std::vector<ByteRange>& input_byte_ranges,
    std::vector<TimeRange>& output_time_ranges)
{
    TRACE_VERBOSE(("IPC getBufferedTimeRanges\n"));
    (void)source_id; // Unused
    (void)input_byte_ranges; // Unused
    (void)output_time_ranges; // Unused
    return UVA_OK;
}

UVA_STATUS OperaMuminMediaBackendIpc::getPlaybackSpeedRanges(
    std::vector<SpeedRange>& speed_ranges)
{
    TRACE_INFO(("IPC getPlaybackSpeedRanges\n"));
    (void)speed_ranges; // Unused
    return UVA_OK;
}
