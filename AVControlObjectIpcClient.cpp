// AVControlObjectIpcClient.cpp (snippet)
#include "AVControlObjectIpcClient.hpp"
#include "AVControlObjectIpcTypes.hpp"

AVControlObjectIpcClient::AVControlObjectIpcClient(AnyCommandThread& /*media_queue*/,
                                                   int streaming_type,
                                                   const MediaDataSource& /*media_data_source*/,
                                                   MediaDataSink& /*media_data_sink*/,
                                                   AnyAVControlObjectEventGenerator& /*event_generator*/,
                                                   char* origin)
: m_handle(0)
{
    // Use templated nebulaRpcCall to get back a handle (int64)
    // Adjust return type if your RPC uses a different type name (e.g. frost_int)
    std::int64_t handle = 0;
    bool ok = nebulaRpcCall<std::int64_t>(IPC_NAME(AVControlObject_Create), &handle, streaming_type, std::string(origin ? origin : ""));
    if (ok) m_handle = handle;
}

AVControlObjectIpcClient::~AVControlObjectIpcClient() {
    if (m_handle) {
        // best-effort destroy
        nebulaRpcCall<frost_bool>(IPC_NAME(AVControlObject_Destroy), m_handle);
        m_handle = 0;
    }
}

ExplodingBoolean AVControlObjectIpcClient::isInitialised() const {
    return ExplodingBoolean(m_handle != 0);
}

bool AVControlObjectIpcClient::setSource(const char* url) {
    if (!m_handle) return false;
    return nebulaRpcCall<frost_bool>(IPC_NAME(AVControlObject_SetSource), m_handle, std::string(url ? url : ""));
}

bool AVControlObjectIpcClient::play() {
    if (!m_handle) return false;
    return nebulaRpcCall<frost_bool>(IPC_NAME(AVControlObject_Play), m_handle);
}

void AVControlObjectIpcClient::stop() {
    if (!m_handle) return;
    nebulaRpcCall<frost_bool>(IPC_NAME(AVControlObject_Stop), m_handle);
}

bool AVControlObjectIpcClient::seek(double position_in_msecs) {
    if (!m_handle) return false;
    return nebulaRpcCall<frost_bool>(IPC_NAME(AVControlObject_Seek), m_handle, static_cast<long long>(position_in_msecs));
}
