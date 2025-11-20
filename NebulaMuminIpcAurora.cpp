#include "nebula/adaptation_layer/ipc/NebulaMuminIpcTypes.hpp"
//#include "nebula/adaptation_layer/nebula_common.h"
#include "frost/frost_basictypes.h"
#include "utilities_debug/trace.h"
#include "utilities_private/PrioritisedCommandThread.hpp"

//#include "vewd_integration/mumin/OperaMuminMediaBackend.hpp"
//#include "uva_backend.h"
//#include "NebulaIpcHelper.hpp"

TRACE_IMPLEMENT("nebula/mumin_service");

static frost_bool g_mumin_running = frost_false;

// Singleton backend pointer
//static OperaMuminMediaBackend* g_backend = nullptr;

// Optional helper to create backend safely
static bool start_backend()
{
    TRACE_ALWAYS(("MuminService: start_backend()\n"));
}

static void stop_backend()
{
    TRACE_ALWAYS(("MuminService: stop_backend()\n"));
}

extern "C" {

// ---------------- RPC entry points ----------------

// Starts the Mumin backend service
frost_bool NEBULA_MuminStartService(frost_int options)
{
    TRACE_ALWAYS(("NEBULA_MuminStartService()\n"));

    if (!start_backend()) {
        TRACE_ERROR(("NEBULA_MuminStartService: backend start failed\n"));
        return frost_false;
    }

    g_mumin_running = frost_true;
    //nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_MuminStatusChangedCallback), g_mumin_running);
    return frost_true;
}

// Stops the Mumin backend service
frost_bool NEBULA_MuminStopService(void)
{
    TRACE_ALWAYS(("NEBULA_MuminStopService()\n"));
    stop_backend();

    g_mumin_running = frost_false;
    //nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_MuminStatusChangedCallback), g_mumin_running);
    return frost_true;
}

// Returns current status of the backend
frost_int NEBULA_MuminGetStatus(void)
{
    return g_mumin_running ? 1 : 0;
}

// Registers a callback for status changes
frost_bool NEBULA_MuminStatusChangedCallback(frost_int status, void* user_data)
{
	//nebulaRpcCall<frost_int>(IPC_NAME(NEBULA_MuminStatusChangedCallback), status);
    return frost_true;
}

} // extern "C"
