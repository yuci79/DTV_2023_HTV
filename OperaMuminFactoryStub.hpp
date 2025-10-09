#ifndef VEWD_INTEGRATION_MUMIN_OPERAMUMINFACTORYSTUB_HPP
#define VEWD_INTEGRATION_MUMIN_OPERAMUMINFACTORYSTUB_HPP

// Forward declarations
class UVABackend;
class UVAClient;

// Stubbed Mumin factory IPC interface for SDK423 builds
// Provides the same exported symbols as the real factory, but no real backends.

void Opera_MuminFactoryInitialise();
bool Opera_MuminFactorySupportedType(int backendType);
void* Opera_MuminFactoryCreateBackend(int backendType, void* client);
void Opera_MuminFactoryDiscardBackend(void* backend);

#endif // VEWD_INTEGRATION_MUMIN_OPERAMUMINFACTORYSTUB_HPP



