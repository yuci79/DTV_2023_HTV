/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaComponentCallbackCallbacksIpcAurora class.
 */

#include "NebulaComponentCallbackCallbacksIpcAurora.hpp"
#include "nebula/adaptation_layer/ipc/aurora/AuroraRpcClient.hpp"
#include "nebula/adaptation_layer/ipc/NebulaIpcConfiguration.hpp"
#include "nebula/adaptation_layer/nebula_component_callback.h"
#include "nebula_src/adaptation_layer/ipc/NebulaComponentCallbackIpcTypes.hpp"
#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/NebulaComponentCallbackCallbacksIpcAurora");

namespace
{
extern "C"
void proxyAudioComponent(char const* language,
                         bool ad_on,
                         void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_AudioComponentCallback), 
                                std::string(language),
                                ad_on,
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyFillComponentList(NEBULA_AVComponentList* component_list,
                            OIPF_ComponentType type,
                            void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));
    
    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);

    std::vector<MP_NEBULA_AVComponent> components;
    const bool ok = client.callReturn(components,
                                      IPC_NAME(NEBULA_FillComponentListCallback),
                                      type,
                                      RegisteredCallbacksConfig::toToken(user_data));
    if (ok)
    {
        const int count = static_cast<int>(components.size());
        component_list->number_of_current_components = count;
        std::copy(components.begin(), components.begin() + count, component_list->components);
    }
    else
    {
        component_list->number_of_current_components = 0;
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyActivateSelectedComponent(NEBULA_AVComponent* component, void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_ActivateSelectedComponentCallback), 
                                MP_NEBULA_AVComponent(*component),
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}

extern "C"
void proxyActivateSelectedComponentOnId(OIPF_ComponentType type,
                                        const char* id,
                                        const char* lang,
                                        frost_bool activate,
                                        const char* label,
                                        void* user_data)
{
    TRACE_INFO(("%s()\n", __func__));

    AuroraRpcClient client(NebulaIpcPorts::browser_callback_server_port);
    const bool ok = client.call(IPC_NAME(NEBULA_ActivateSelectedComponentOnIdCallback), 
                                type,
                                std::string(id),
                                std::string(lang),
                                activate,
                                std::string(label),
                                RegisteredCallbacksConfig::toToken(user_data));
    if (!ok)
    {
        TRACE_ERROR(("%s() Failed\n", __func__));
    }
}
}

void
NebulaComponentCallbackCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_RegisterAudioComponentCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterAudioComponentCallback(
                    proxyAudioComponent,
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterAudioComponentCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterAudioComponentCallback(
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterFillComponentListCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterFillComponentListCallback(
                    proxyFillComponentList, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterFillComponentListCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterFillComponentListCallback(
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterActivateSelectedComponentCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterActivateSelectedComponentCallback(
                    proxyActivateSelectedComponent, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterActivateSelectedComponentCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterActivateSelectedComponentCallback(
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_RegisterActivateSelectedComponentOnIdCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_RegisterActivateSelectedComponentOnIdCallback(
                    proxyActivateSelectedComponentOnId, 
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });

    server.bind(IPC_NAME(NEBULA_DeregisterActivateSelectedComponentOnIdCallback),
                [](RegisteredCallbacksConfig::TokenType token)
    {
        return NEBULA_DeregisterActivateSelectedComponentOnIdCallback(
                    RegisteredCallbacksConfig::toPointer<void*>(token));
    });
}
