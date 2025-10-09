#include "OperaMuminObjectFactory.hpp"
#include <cstdint>

// declare the external IPC functions we created earlier
extern "C" {
    bool Opera_MuminFactoryInitialise();
    bool Opera_MuminFactorySupportedType(int backendType);
    std::intptr_t Opera_MuminFactoryCreateBackend(int backendType, std::intptr_t clientHandle);
    void Opera_MuminFactoryDiscardBackend(std::intptr_t backendHandle);
}

OperaMuminObjectFactory::OperaMuminObjectFactory()
{
    // ctor: nothing heavy here — leave to initialise()
}

OperaMuminObjectFactory::~OperaMuminObjectFactory()
{
    // dtor: cleanup if needed
}

bool OperaMuminObjectFactory::initialise()
{
    // forward to external IPC function that will call the aurora-server side
    return Opera_MuminFactoryInitialise();
}

UVABackend::UVABackendType OperaMuminObjectFactory::supportedType(UVABackend::UVABackendType type)
{
    // The original signature was returning something like bool/enum.
    // We'll call the IPC function and convert as needed.
    bool supported = Opera_MuminFactorySupportedType(static_cast<int>(type));
    if (supported) {
        return type;
    }
    return UVABackend::UNKNOWN; // adapt to your enum
}

UVABackend* OperaMuminObjectFactory::createMuminBackend(UVABackend::UVABackendType type, UVAClient* client)
{
    // We cannot use real pointers across processes. Return a *local proxy* object that
    // forwards operations to aurora. For now we return nullptr as a placeholder and
    // log a TODO. See roadmap below for the remote proxy implementation.
    //
    // If you want a quick test harness: return nullptr on failure or a minimal stub backend.
    std::intptr_t handle = Opera_MuminFactoryCreateBackend(static_cast<int>(type),
                                                          reinterpret_cast<std::intptr_t>(client));
    if (handle == 0) {
        return nullptr;
    }

    // TODO: implement RemoteUVABackend that implements UVABackend and forwards calls using handle
    return nullptr;
}

void OperaMuminObjectFactory::discardMuminBackend(UVABackend* backend)
{
    // If backend is a remote proxy, convert to handle and call RPC discard.
    // For now call the discard RPC with a guess (if you returned a fake handle earlier).
    if (!backend) return;

    // If we had a RemoteUVABackend with method getHandle(), we'd do:
    // Opera_MuminFactoryDiscardBackend(remote->getHandle());
}
