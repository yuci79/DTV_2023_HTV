// ============================================================================
// FILE: AVControlObjectIpcClient.hpp
// DESCRIPTION: IPC Client header - runs in Browser process
// ============================================================================

#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED

#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "AVControlObjectIpcTypes.hpp"

#include <cstdint>
#include <string>

// Forward declarations
class AnyAVControlObjectEventGenerator;

/**
 * @brief IPC Client for AVControlObject that runs in the browser process.
 *
 * This is a proxy/stub that forwards all method calls via RPC to the real
 * AVControlObject instance running in the Aurora process.
 *
 * Usage:
 * @code
 * AVControlObjectIpcClient* client = new AVControlObjectIpcClient(
 *     media_queue, streaming_type, data_source, data_sink, event_gen, origin);
 *
 * if (client->isInitialised()) {
 *     client->setSource("udp://239.1.1.7:1234");
 *     client->play();
 * }
 * @endcode
 */
class AVControlObjectIpcClient : public AnyAVControlObject
{
public:
    /**
     * @brief Constructor - creates remote AVControlObject via RPC.
     *
     * @param media_queue Command thread for media operations (not used in IPC client)
     * @param streaming_type Type of streaming (0=progressive, 1=mse, 2=rtsp, 3=udp)
     * @param media_data_source Source configuration (not used in IPC client)
     * @param media_data_sink Sink configuration (not used in IPC client)
     * @param event_generator Event generator (not used in IPC client)
     * @param origin Origin URL for the media stream
     */
    AVControlObjectIpcClient(AnyCommandThread& media_queue,
                             int streaming_type,
                             const MediaDataSource& media_data_source,
                             MediaDataSink& media_data_sink,
                             AnyAVControlObjectEventGenerator& event_generator,
                             char* origin);

    /**
     * @brief Destructor - destroys remote AVControlObject via RPC.
     */
    virtual ~AVControlObjectIpcClient() override;

    // ========================================================================
    // AnyAVControlObject interface implementation
    // ========================================================================

    /**
     * @brief Check if the remote object was successfully created.
     * @return ExplodingBoolean(true) if handle is valid, false otherwise
     */
    ExplodingBoolean isInitialised() const override;

    /**
     * @brief Set the media source URL.
     * @param url URL of the media source
     * @return true if successful, false otherwise
     */
    bool setSource(const char* url) override;

    /**
     * @brief Set cookies for HTTP(S) streams.
     * @param cookies Cookie string
     * @return true if successful, false otherwise
     */
    bool setCookies(char const* cookies) override;

    /**
     * @brief Start playback.
     * @return true if successful, false otherwise
     */
    bool play() override;

    /**
     * @brief Stop playback.
     */
    void stop() override;

    /**
     * @brief Suspend or resume playback.
     * @param is_suspend true to suspend, false to resume
     * @return true if successful, false otherwise
     */
    bool suspend(bool is_suspend) override;

    /**
     * @brief Set video visibility.
     * @param visible true to show video, false to hide
     * @return true if successful, false otherwise
     */
    bool setVisibility(bool visible) override;

    /**
     * @brief Get current playback status.
     * @return NEBULA_MediaPlayerStatus enum value
     */
    NEBULA_MediaPlayerStatus getPlaybackStatus() override;

    /**
     * @brief Get the last error that occurred.
     * @return NEBULA_MediaPlayerError enum value
     */
    NEBULA_MediaPlayerError getError() override;

    /**
     * @brief Seek to a specific position.
     * @param position_in_msecs Position in milliseconds
     * @return true if successful, false otherwise
     */
    bool seek(double position_in_msecs) override;

    /**
     * @brief Get current playback position as return value.
     * @return Current position in milliseconds
     */
    std::int64_t getCurrentPosition() override;

    /**
     * @brief Get current playback position via output parameter.
     * @param position Output parameter for position in milliseconds
     * @return true if successful, false otherwise
     */
    bool getCurrentPosition(std::int64_t& position) override;

    /**
     * @brief Set playback speed.
     * @param speed Speed multiplier (e.g., 1000 = 1x, 2000 = 2x)
     * @return true if successful, false otherwise
     */
    bool setSpeed(int speed) override;

    /**
     * @brief Get current playback speed.
     * @return Current speed multiplier
     */
    int getSpeed() override;

    /**
     * @brief Get total playback time.
     * @return Total duration in milliseconds
     */
    int getTotalPlaybackTime() override;

    /**
     * @brief Set fullscreen mode.
     * @param fullscreen true for fullscreen, false for windowed
     * @return true if successful, false otherwise
     */
    bool setFullScreen(bool fullscreen) override;

    /**
     * @brief Get file details for the current media.
     * @param file_info Output parameter for file details
     * @return true if successful, false otherwise
     */
    bool getFileDetails(NEBULA_MediaFileDetails* file_info) override;

    /**
     * @brief Add an item to the playback list.
     * @param url URL of the media item to add
     * @return true if successful, false otherwise
     */
    bool addItem(char const* url) override;

    /**
     * @brief Queue a media item for playback.
     * @param url URL of the media item to queue
     * @param added true if item should be added to queue
     * @return true if successful, false otherwise
     */
    bool queue(char const* url, bool added) override;

    /**
     * @brief Set the playback status.
     * @param status New playback status
     * @param synchronous_event Whether to generate synchronous events
     */
    void setPlaybackStatus(NEBULA_MediaPlayerStatus status, bool synchronous_event = false) override;

    /**
     * @brief Prepare for playback.
     * @return true if successful, false otherwise
     */
    bool prepareForPlayback() override;

    /**
     * @brief Validate a URL.
     * @param url URL to validate
     * @return true if valid, false otherwise
     */
    bool validURL(const UString url) override;

    /**
     * @brief Attach a data source.
     * @param source_id Source identifier
     * @return true if successful, false otherwise
     */
    bool attachSource(frost_uintptr source_id) override;

    /**
     * @brief Detach a data source.
     * @param source_id Source identifier
     * @return true if successful, false otherwise
     */
    bool detachSource(frost_uintptr source_id) override;

    /**
     * @brief Get buffered time range.
     * @param start Output parameter for start time
     * @param end Output parameter for end time
     * @return true if successful, false otherwise
     */
    bool getBufferedTimeRange(double* start, double* end) override;

    /**
     * @brief Get supported trick modes.
     * @param supported_speed_list Output array for supported speeds
     * @param max_count Maximum number of speeds to return
     * @return true if successful, false otherwise
     */
    bool getSupportedTrickModes(int* supported_speed_list, int max_count) override;

    // ========================================================================
    // AnyAvController interface implementation
    // ========================================================================

    /**
     * @brief Check if video is in fullscreen mode.
     * @return true if fullscreen, false otherwise
     */
    bool isFullScreen() override;

    /**
     * @brief Set video output window position and size.
     * @param output_window Pointer to display window structure
     * @param is_browser_video Whether this is browser video
     * @return true if successful, false otherwise
     */
    bool setVideoOutputWindow(const NEBULA_DisplayWindow* output_window, bool is_browser_video = false) override;

    /**
     * @brief Get current video output window.
     * @return Current display window structure
     */
    NEBULA_DisplayWindow getVideoOutputWindow() override;

    /**
     * @brief Get video aspect ratio.
     * @return NEBULA_VideoAspectRatio enum value
     */
    NEBULA_VideoAspectRatio getVideoAspectRatio() override;

    /**
     * @brief Get exact video aspect ratio.
     * @param aspect_ratio Output parameter for aspect ratio
     * @return true if available, false otherwise
     */
    bool getVideoExactAspectRatio(float &aspect_ratio) override;

    /**
     * @brief Get video decode width.
     * @return Width in pixels
     */
    unsigned int getVideoDecodeWidth() override;

    /**
     * @brief Get video decode height.
     * @return Height in pixels
     */
    unsigned int getVideoDecodeHeight() override;

    // ========================================================================
    // AnyAvComponentController interface implementation
    // ========================================================================

    /**
     * @brief Get available AV components.
     * @param type Type of components to get
     * @param component_list Output list for components
     * @return true if successful, false otherwise
     */
    bool getComponents(OIPF_ComponentType type, NEBULA_AVComponentList* component_list) override;

    /**
     * @brief Select an AV component.
     * @param component Component to select
     * @return true if successful, false otherwise
     */
    bool selectComponent(const NEBULA_AVComponent* component) override;

    /**
     * @brief Deselect an AV component.
     * @param component Component to deselect
     * @return true if successful, false otherwise
     */
    bool deselectComponent(const NEBULA_AVComponent* component) override;

private:
    std::int64_t m_handle;  ///< Handle to remote AVControlObject instance

    // Prevent copying
    AVControlObjectIpcClient(const AVControlObjectIpcClient&) = delete;
    AVControlObjectIpcClient& operator=(const AVControlObjectIpcClient&) = delete;
};

#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCCLIENT_HPP_INCLUDED
