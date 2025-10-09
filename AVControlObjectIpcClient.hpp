#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED

#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "utilities_private/CommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "AVControlObjectIpcTypes.hpp"

// Forward declaration
class AnyAVControlObjectEventGenerator;

/**
 * @brief IPC Client for AVControlObject that runs in the browser process
 */
class AVControlObjectIpcClient : public AnyAVControlObject
{
public:
    /**
     * @brief Constructor - creates remote AVControlObject via RPC
     */
    AVControlObjectIpcClient(AnyCommandThread& media_queue,
                           int streaming_type,  // Use int instead of enum to avoid dependencies
                           MediaDataSource const& media_data_source,
                           MediaDataSink& media_data_sink,
                           AnyAVControlObjectEventGenerator& event_generator,
                           char* origin);

    /**
     * @brief Destructor - destroys remote AVControlObject via RPC
     */
    virtual ~AVControlObjectIpcClient() override;

    // AnyAVControlObject interface implementation - only implement methods that exist in base class
    virtual ExplodingBoolean isInitialised() const override;
    virtual bool setSource(const char* url) override;
    virtual bool play() override;
    virtual void stop() override;
    virtual bool setSpeed(int speed) override;
    virtual bool setFullScreen(bool fullscreen) override;
    virtual bool setVisibility(bool visible) override;
    virtual bool getCurrentPosition(std::int64_t& position_msecs) override;
    virtual NEBULA_MediaPlayerStatus getPlaybackStatus() override;
    virtual bool suspend(bool suspend) override;
    virtual bool setCookies(char const* cookies) override;

    // Remove methods that don't exist in the base class or implement as needed

private:
    std::intptr_t m_remote_handle = 0;
    bool m_initialised = false;

    // Prevent copying
    AVControlObjectIpcClient(const AVControlObjectIpcClient&) = delete;
    AVControlObjectIpcClient& operator=(const AVControlObjectIpcClient&) = delete;
};

#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED
