// ============================================================================
// FILE: AVControlHelpers.hpp
// DESCRIPTION: Helper functions for fallback dependencies
// ============================================================================

#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLHELPERS_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLHELPERS_HPP_INCLUDED

#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/MediaDataSink.hpp"
#include "nebula/core/browser_client/AnyAVControlObjectEventGenerator.hpp"

// Forward declarations only - no includes needed
class AnyCommandThread;
class MediaDataSink;
class AnyAVControlObjectEventGenerator;

/**
 * @brief Get a global dummy command thread instance.
 *
 * This is used as a fallback when no real command thread is provided
 * to the IPC server. The dummy implementation may not be fully functional.
 *
 * @return Reference to global dummy command thread
 */
AnyCommandThread& getGlobalMediaCommandThread();

/**
 * @brief Get a global dummy media data sink instance.
 *
 * This is used as a fallback when no real media data sink is provided
 * to the IPC server.
 *
 * @return Pointer to global dummy media data sink
 */
MediaDataSink* getMediaDataSink();

/**
 * @brief Get a global dummy event generator instance.
 *
 * This is used as a fallback when no real event generator is provided
 * to the IPC server. Events will not be propagated back to the browser.
 *
 * @return Pointer to global dummy event generator
 */
AnyAVControlObjectEventGenerator* getEventGeneratorForAv();

#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLHELPERS_HPP_INCLUDED

// ============================================================================
// USAGE NOTES
// ============================================================================

/*
HEADER FILE LOCATIONS:

vewd_integration/ipc/avcontrolobject/
├── AVControlObjectIpcTypes.hpp      - IPC type definitions
├── AVControlObjectIpcServer.hpp     - Server interface (Aurora process)
├── AVControlObjectIpcClient.hpp     - Client interface (Browser process)
├── AVControlHelpers.hpp             - Fallback dependency helpers
├── AVControlObjectIpcTypes.cpp      - IPC type implementations
├── AVControlObjectIpcServer.cpp     - Server implementations
├── AVControlObjectIpcClient.cpp     - Client implementations
└── AVControlHelpers.cpp             - Fallback implementations

INCLUDE ORDER IN YOUR FILES:

1. In BrowserCallbackServer.cpp:
   #include "avcontrolobject/AVControlObjectIpcServer.hpp"

2. In OperaMuminObjectFactory.cpp:
   #include "avcontrolobject/AVControlObjectIpcClient.hpp"

3. In AVControlObjectIpcServer.cpp:
   #include "AVControlObjectIpcServer.hpp"
   #include "AVControlObjectIpcTypes.hpp"
   #include "AVControlHelpers.hpp"
   #include "nebula/core/browser_client/AVControlObject.hpp"

4. In AVControlObjectIpcClient.cpp:
   #include "AVControlObjectIpcClient.hpp"
   #include "AVControlObjectIpcTypes.hpp"
   #include "nebula_src/adaptation_layer/ipc/external/NebulaIpcHelper.hpp"

BUILD SYSTEM:

In component.mak, ensure these are listed:

OBJS +=\
    avcontrolobject/AVControlObjectIpcTypes \
    avcontrolobject/AVControlObjectIpcServer \
    avcontrolobject/AVControlObjectIpcClient \
    avcontrolobject/AVControlHelpers

CRITICAL REMINDERS:

1. AVControlObjectIpcServer.hpp defines:
   - static void bindToServer(rpc::server& server)

2. Call this in BrowserCallbackServer.cpp:
   AVControlObjectIpcServer::bindToServer(server);

3. NOT this (wrong):
   bindAVControlObjectServer(server);  // ❌ WRONG

4. The server binding MUST happen before any media objects are created

5. All headers use include guards and forward declarations to minimize
   compile-time dependencies

6. The implementation follows the proxy/stub pattern:
   - Client (Browser) = Stub (sends RPC calls)
   - Server (Aurora) = Proxy (receives RPC, manages real objects)
*/
