#include "IpcMediaEventPublisher.hpp"
#include <iostream>

IpcMediaEventPublisher::IpcMediaEventPublisher(const std::string& host, uint16_t port)
: m_host(host), m_port(port)
{
    try {
        m_client = std::make_unique<rpc::client>(m_host, m_port);
        m_client->set_timeout(std::chrono::milliseconds(250));
    } catch (const std::exception& e) {
        std::cerr << "IpcMediaEventPublisher ctor: " << e.what() << std::endl;
    }
}

IpcMediaEventPublisher::~IpcMediaEventPublisher() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_client.reset();
}

void IpcMediaEventPublisher::safeInvoke(const std::function<void(rpc::client&)>& fn) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_client) {
        try {
            m_client = std::make_unique<rpc::client>(m_host, m_port);
            m_client->set_timeout(std::chrono::milliseconds(250));
        } catch (...) {
            return;
        }
    }
    try {
        fn(*m_client);
    } catch (const std::exception& e) {
        // Log; optionally mark for retry.
        std::cerr << "IpcMediaEventPublisher invoke error: " << e.what() << std::endl;
    }
}

void IpcMediaEventPublisher::publishPlayState(NEBULA_MediaPlayerStatus state) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_play_state_changed", static_cast<int>(state));
    });
}

void IpcMediaEventPublisher::publishPlayPosition(int64_t position) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_play_position_changed", position);
    });
}

void IpcMediaEventPublisher::publishPlaySpeed(int speed) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_play_speed_changed", speed);
    });
}

void IpcMediaEventPublisher::publishFullscreenChanged(bool fullscreen) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_fullscreen_changed", fullscreen);
    });
}

void IpcMediaEventPublisher::publishFileDetails(const std::string& serialized) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_file_details_parsed", serialized);
    });
}

void IpcMediaEventPublisher::publishComponentsChanged() {
    safeInvoke([&](rpc::client& c) {
        c.call("media_components_changed");
    });
}

void IpcMediaEventPublisher::publishComponentActiveness(const std::string& serializedComponent, bool active) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_component_activeness_changed", serializedComponent, active);
    });
}

void IpcMediaEventPublisher::publishUpdateAudioComponent(const std::string& language, bool ad_on) {
    safeInvoke([&](rpc::client& c) {
        c.call("media_update_audio_component", language, ad_on);
    });
}