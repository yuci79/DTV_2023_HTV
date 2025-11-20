#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <mutex>
#include "rpc/client.h"
#include "nebula/media/NebulaMediaTypes.hpp"

class IpcMediaEventPublisher {
public:
    IpcMediaEventPublisher(const std::string& host, uint16_t port);
    ~IpcMediaEventPublisher();

    void publishPlayState(NEBULA_MediaPlayerStatus state);
    void publishPlayPosition(int64_t position);
    void publishPlaySpeed(int speed);
    void publishFullscreenChanged(bool fullscreen);
    void publishFileDetails(const std::string& serialized); // serialized payload
    void publishComponentsChanged();
    void publishComponentActiveness(const std::string& serializedComponent, bool active);
    void publishUpdateAudioComponent(const std::string& language, bool ad_on);

private:
    void safeInvoke(const std::function<void(rpc::client&)>& fn);

    std::unique_ptr<rpc::client> m_client;
    std::mutex m_mutex;
    std::string m_host;
    uint16_t m_port;
};