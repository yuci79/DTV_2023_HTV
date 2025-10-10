// ============================================================================
// FILE: AVControlObjectIpcTypes.hpp
// DESCRIPTION: IPC type definitions and macro declarations
// ============================================================================

#ifndef CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCTYPES_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCTYPES_HPP_INCLUDED

#include "3rd_party_wrapper/rpclib/include/IpcTypes.hpp"
#include "nebula/core/browser_client/AnyAVControlObject.hpp"

// Declare IPC method names
DECLARE_IPC_NAME(AVControlObject_Create);
DECLARE_IPC_NAME(AVControlObject_Destroy);
DECLARE_IPC_NAME(AVControlObject_SetSource);
DECLARE_IPC_NAME(AVControlObject_Play);
DECLARE_IPC_NAME(AVControlObject_Stop);
DECLARE_IPC_NAME(AVControlObject_Seek);
DECLARE_IPC_NAME(AVControlObject_SetVideoOutputWindow);
DECLARE_IPC_NAME(AVControlObject_SetSpeed);
DECLARE_IPC_NAME(AVControlObject_SetFullScreen);
DECLARE_IPC_NAME(AVControlObject_SetVisibility);
DECLARE_IPC_NAME(AVControlObject_GetCurrentPosition);
DECLARE_IPC_NAME(AVControlObject_GetPlaybackStatus);
DECLARE_IPC_NAME(AVControlObject_Suspend);
DECLARE_IPC_NAME(AVControlObject_SetCookies);

// Make NEBULA_MediaPlayerStatus serializable
MSGPACK_ADD_ENUM(NEBULA_MediaPlayerStatus);

// Streaming type enum for IPC (avoid enum class dependencies)
enum IpcStreamingType {
    IPC_STREAMING_PROGRESSIVE = 0,
    IPC_STREAMING_MSE = 1,
    IPC_STREAMING_RTSP = 2,
    IPC_STREAMING_UDP = 3
};

#endif // CABOT_VEWD_INTEGRATION_IPC_AVCONTROLOBJECTIPCTYPES_HPP_INCLUDED
