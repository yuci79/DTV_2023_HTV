#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED

#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "utilities_private/CommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "AVControlObjectIpcTypes.hpp"

#include <map>
#include <memory>

// Forward declare rpc::server to avoid including the full header
namespace rpc {
    class server;
}

/**
 * @brief IPC Server for AVControlObject that runs in the Aurora process
 */
class AVControlObjectIpcServer
{
public:
    static AVControlObjectIpcServer& instance();

    bool createAVControlObject(AnyCommandThread& media_queue,
                              int streaming_type,
                              MediaDataSource const& media_data_source,
                              MediaDataSink& media_data_sink,
                              AnyAVControlObjectEventGenerator& event_generator,
                              char* origin,
                              std::intptr_t& handle_out);

    bool destroyAVControlObject(std::intptr_t handle);
    bool setSource(std::intptr_t handle, const std::string& url);
    bool play(std::intptr_t handle);
    bool stop(std::intptr_t handle);
    bool setVideoOutputWindow(std::intptr_t handle, const NEBULA_DisplayWindow& window, bool apply);
    NEBULA_MediaPlayerStatus getPlaybackStatus(std::intptr_t handle);
    bool seek(std::intptr_t handle, std::int64_t position_msecs);
    bool setSpeed(std::intptr_t handle, int speed);
    bool setFullScreen(std::intptr_t handle, bool fullscreen);
    bool setVisibility(std::intptr_t handle, bool visible);
    bool getCurrentPosition(std::intptr_t handle, std::int64_t& position_msecs);
    bool isInitialised(std::intptr_t handle) const;
    // Declare the binding function
    static void bindToServer(rpc::server& server);
private:
    AVControlObjectIpcServer() = default;
    ~AVControlObjectIpcServer() = default;

    AVControlObjectIpcServer(const AVControlObjectIpcServer&) = delete;
    AVControlObjectIpcServer& operator=(const AVControlObjectIpcServer&) = delete;

    std::map<std::intptr_t, AVControlObject*> m_objects;
};



#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED
