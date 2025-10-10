// ============================================================================
// FILE: AVControlObjectIpcServer.hpp
// DESCRIPTION: IPC Server header - runs in Aurora process
// ============================================================================

#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED

#include "nebula/core/browser_client/AnyAVControlObject.hpp"
#include "nebula/core/browser_client/AVControlObject.hpp"
#include "utilities_private/CommandThread.hpp"
#include "utilities_public/MediaDataSource.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "AVControlObjectIpcTypes.hpp"

// Forward declare rpc::server
namespace rpc {
    class server;
}

// Forward declarations
class AnyAVControlObjectEventGenerator;

/**
 * @brief IPC Server for AVControlObject that runs in the Aurora process.
 *
 * This class provides static methods to bind RPC handlers and manage
 * AVControlObject instances created via IPC from the browser process.
 */
class AVControlObjectIpcServer
{
public:
    /**
     * @brief Bind all AVControlObject RPC methods to the server.
     *
     * This MUST be called during Aurora process initialization, typically
     * from BrowserCallbackServer's binding function.
     *
     * @param server The rpc::server instance to bind methods to
     */
    static void bindToServer(rpc::server& server);

private:
    // This class only provides static methods
    AVControlObjectIpcServer() = delete;
    ~AVControlObjectIpcServer() = delete;
    AVControlObjectIpcServer(const AVControlObjectIpcServer&) = delete;
    AVControlObjectIpcServer& operator=(const AVControlObjectIpcServer&) = delete;
};

/**
 * @brief Set the command thread to use for AVControlObject instances.
 *
 * These functions allow the Aurora process to provide concrete implementations
 * of abstract dependencies. If not called, fallback implementations will be used.
 *
 * @param thread Pointer to the command thread instance
 */
void AVControlObjectIpcServer_setCommandThread(AnyCommandThread* thread);

/**
 * @brief Set the media data sink to use for AVControlObject instances.
 * @param sink Pointer to the media data sink instance
 */
void AVControlObjectIpcServer_setMediaDataSink(MediaDataSink* sink);

/**
 * @brief Set the event generator to use for AVControlObject instances.
 * @param eg Pointer to the event generator instance
 */
void AVControlObjectIpcServer_setEventGenerator(AnyAVControlObjectEventGenerator* eg);

#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCSERVER_HPP_INCLUDED
