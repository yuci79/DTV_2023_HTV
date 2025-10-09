#include "vewd_integration/mumin/OperaMuminFactoryStub.hpp"
//#include <iostream>
#include <stdio.h>
// ✅ Pull in UVA definitions
#include <uva_backend.h>
#include <uva_client.h>

// Existing stubbed functions
void Opera_MuminFactoryInitialise()
{
    //std::cout << "[Stub] Opera_MuminFactoryInitialise() called (no-op)" << std::endl;
    std::printf("[Stub] Opera_MuminFactoryInitialise() called (no-op)\n");

}

bool Opera_MuminFactorySupportedType(int backendType)
{
    //std::cout << "[Stub] Opera_MuminFactorySupportedType("
    //          << backendType << ") called (return false)" << std::endl;
    std::printf("[Stub] Opera_MuminFactorySupportedType(%d) called (return false)\n", backendType);

    return false;
}

void* Opera_MuminFactoryCreateBackend(int backendType, void* client)
{
    //std::cout << "[Stub] Opera_MuminFactoryCreateBackend("
    //          << backendType << ") called (return nullptr)" << std::endl;
    std::printf("[Stub] Opera_MuminFactoryCreateBackend(%d) called (return nullptr)\n", backendType);

    return nullptr;
}

void Opera_MuminFactoryDiscardBackend(void* backend)
{
    //std::cout << "[Stub] Opera_MuminFactoryDiscardBackend() called (no-op)" << std::endl;
    std::printf("[Stub] Opera_MuminFactoryDiscardBackend() called (no-op)\n");

}

// =====================================================
// Required exported UVA_* entry points for dynamic loader
// =====================================================
extern "C" {

// Main UVA entry point
UVABackend* UVA_RequestBackend(UVABackend::UVABackendType type, UVAClient* client)
{
    //std::cout << "[Stub] UVA_RequestBackend(type=" << static_cast<int>(type) << ") called" << std::endl;
    std::printf("[Stub] UVA_RequestBackend(type= %d) called\n", static_cast<int>(type));

    return reinterpret_cast<UVABackend*>(
        Opera_MuminFactoryCreateBackend(static_cast<int>(type), client)
    );
}

// Mobile variant (signature must match SDK!)
UVABackend* UVA_RequestBackendMB(UVABackend::UVABackendType type,
                                 const UVABackend::CustomAttributes& attrs,
                                 UVAClient* client)
{
    //std::cout << "[Stub] UVA_RequestBackendMB(type=" << static_cast<int>(type)
    //          << ", attrs=ignored) called" << std::endl;
    std::printf("[Stub] UVA_RequestBackendMB(type= %d, attrs=ignored) called\n", static_cast<int>(type));

    return reinterpret_cast<UVABackend*>(
        Opera_MuminFactoryCreateBackend(static_cast<int>(type), client)
    );
}

// OOIF factory (return null until implemented)
void* UVA_RequestOOIFBackendFactory()
{
    //std::cout << "[Stub] UVA_RequestOOIFBackendFactory() called (return nullptr)" << std::endl;
    std::printf("[Stub] UVA_RequestOOIFBackendFactory() called (return nullptr)");

    return nullptr;
}

} // extern "C"
