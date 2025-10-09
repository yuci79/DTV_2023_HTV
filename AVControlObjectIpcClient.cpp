/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of AVControlObjectIpcClient
 */

#include "AVControlObjectIpcClient.hpp"
#include "3rd_party_wrapper/rpclib/include/RpcClient.hpp"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("avcontrolobject/ipcclient");

AVControlObjectIpcClient::AVControlObjectIpcClient(AnyCommandThread& media_queue,
                           int streaming_type,  // Use int instead of nebula::StreamingType
                           MediaDataSource const& media_data_source,
                           MediaDataSink& media_data_sink,
                           AnyAVControlObjectEventGenerator& event_generator,
                           char* origin) {

    TRACE_INFO(("Creating AVControlObjectIpcClient\n"));

    // Create RpcClient instance and call remote constructor via RPC
    RpcClient client;
    auto result = client.call<std::tuple<bool, std::intptr_t>>(
        IPC_NAME(AVControlObject_Create),
        streaming_type,
        std::string(origin ? origin : ""));

    bool success = std::get<0>(result);
    std::intptr_t handle = std::get<1>(result);

    if (success) {
        m_remote_handle = handle;
        m_initialised = true;
        TRACE_INFO(("AVControlObjectIpcClient created successfully, remote_handle=%p\n",
                   (void*)m_remote_handle));
    } else {
        TRACE_ERROR(("Failed to create AVControlObject via RPC\n"));
    }
}

AVControlObjectIpcClient::~AVControlObjectIpcClient() {
    TRACE_INFO(("Destroying AVControlObjectIpcClient, remote_handle=%p\n",
               (void*)m_remote_handle));

    if (m_remote_handle) {
        RpcClient client;
        client.call(IPC_NAME(AVControlObject_Destroy), m_remote_handle);
    }
}

ExplodingBoolean AVControlObjectIpcClient::isInitialised() const {
    return m_initialised ? ExplodingBoolean(true) : ExplodingBoolean(false);
}

bool AVControlObjectIpcClient::setSource(const char* url) {
    if (!m_remote_handle) return false;

    TRACE_INFO(("AVControlObjectIpcClient::setSource: url=%s\n", url));
    RpcClient client;
    return client.call(IPC_NAME(AVControlObject_SetSource),
                      m_remote_handle, std::string(url));
}

bool AVControlObjectIpcClient::play() {
    if (!m_remote_handle) return false;

    TRACE_INFO(("AVControlObjectIpcClient::play\n"));
    RpcClient client;
    return client.call(IPC_NAME(AVControlObject_Play), m_remote_handle);
}

void AVControlObjectIpcClient::stop() {
    if (!m_remote_handle) return;

    TRACE_INFO(("AVControlObjectIpcClient::stop\n"));
    RpcClient client;
    client.call(IPC_NAME(AVControlObject_Stop), m_remote_handle);
}

bool AVControlObjectIpcClient::setSpeed(int speed) {
    TRACE_WARN(("AVControlObjectIpcClient::setSpeed not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::seek(std::int64_t position_msecs) {
    TRACE_WARN(("AVControlObjectIpcClient::seek not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::setVideoOutputWindow(NEBULA_DisplayWindow* window, bool apply) {
    if (!m_remote_handle || !window) return false;

    TRACE_INFO(("AVControlObjectIpcClient::setVideoOutputWindow\n"));
    RpcClient client;
    return client.call(IPC_NAME(AVControlObject_SetVideoOutputWindow),
                      m_remote_handle, *window, apply);
}

bool AVControlObjectIpcClient::setFullScreen(bool fullscreen) {
    TRACE_WARN(("AVControlObjectIpcClient::setFullScreen not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::setVisibility(bool visible) {
    TRACE_WARN(("AVControlObjectIpcClient::setVisibility not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::getCurrentPosition(std::int64_t& position_msecs) {
    TRACE_WARN(("AVControlObjectIpcClient::getCurrentPosition not implemented yet\n"));
    return false;
}

NEBULA_MediaPlayerStatus AVControlObjectIpcClient::getPlaybackStatus() {
    TRACE_WARN(("AVControlObjectIpcClient::getPlaybackStatus not implemented yet\n"));
    return NEBULA_Error;
}

bool AVControlObjectIpcClient::suspend(bool suspend) {
    TRACE_WARN(("AVControlObjectIpcClient::suspend not implemented yet\n"));
    return false;
}

AVControlObjectType AVControlObjectIpcClient::getObjectType() const {
    TRACE_WARN(("AVControlObjectIpcClient::getObjectType not implemented yet\n"));
    return av_control_object_type_net_stream;
}

float AVControlObjectIpcClient::getVideoDecodeWidth() const {
    TRACE_WARN(("AVControlObjectIpcClient::getVideoDecodeWidth not implemented yet\n"));
    return 0.0f;
}

float AVControlObjectIpcClient::getVideoDecodeHeight() const {
    TRACE_WARN(("AVControlObjectIpcClient::getVideoDecodeHeight not implemented yet\n"));
    return 0.0f;
}

bool AVControlObjectIpcClient::getVideoExactAspectRatio(float& aspect_ratio) {
    TRACE_WARN(("AVControlObjectIpcClient::getVideoExactAspectRatio not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::attachSource(uintptr_t source_id) {
    TRACE_WARN(("AVControlObjectIpcClient::attachSource not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::detachSource(uintptr_t source_id) {
    TRACE_WARN(("AVControlObjectIpcClient::detachSource not implemented yet\n"));
    return false;
}

bool AVControlObjectIpcClient::validURL(const char* url) {
    TRACE_WARN(("AVControlObjectIpcClient::validURL not implemented yet\n"));
    return true;
}

bool AVControlObjectIpcClient::setCookies(char const* cookies) {
    TRACE_WARN(("AVControlObjectIpcClient::setCookies not implemented yet\n"));
    return false;
}
