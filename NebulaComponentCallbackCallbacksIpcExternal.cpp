/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the NebulaComponentCallbackCallbacksIpcExternal class and some associated functions.
 */

#include "NebulaComponentCallbackCallbacksIpcExternal.hpp"
#include "NebulaComponentCallbackCallbackIpcTypesExternal.hpp"
#include "RegisteredCallbacks.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaComponentCallbackIpcTypes.hpp"
#include "NebulaIpcHelper.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/external/NebulaComponentCallbackCallbacksIpcExternal");
#include "RegisteredCallbacks.inl"

namespace
{
// Local callback registers
RegisteredCallbacks<AudioComponentCallback> local_audio_component_register;
RegisteredCallbacks<FillComponentListCallback> local_fill_component_list_register;
RegisteredCallbacks<ActivateSelectedComponentCallback> local_activate_selected_component_register;
RegisteredCallbacks<ActivateSelectedComponentOnIdCallback>
                                local_activate_selected_component_on_id_register;

// Proxy external callbacks
void
proxyAudioComponent(std::string language,
                    bool ad_on,
                    RegisteredCallbacksConfig::TokenType token)
{
    AudioComponentCallback callback;
    if (local_audio_component_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(language.c_str(), ad_on, user_data);
        }
    }
}

std::vector<MP_NEBULA_AVComponent>
proxyFillComponentList(OIPF_ComponentType type, RegisteredCallbacksConfig::TokenType token)
{
    std::vector<MP_NEBULA_AVComponent> components;
    
    FillComponentListCallback callback;
    if (local_fill_component_list_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            NEBULA_AVComponentList component_list;
            component_list.number_of_current_components = 0;
            function_pointer(&component_list, type, user_data);
            for (int index = 0; index < component_list.number_of_current_components; index++)
            {
                components.push_back(component_list.components[index]);
            }
        }
    }

    return components;
}

void
proxyActivateSelectedComponent(MP_NEBULA_AVComponent component,
                               RegisteredCallbacksConfig::TokenType token)
{
    ActivateSelectedComponentCallback callback;
    if (local_activate_selected_component_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(&component, user_data);
        }
    }
}

void
proxyActivateSelectedComponentOnId(OIPF_ComponentType type,
                                   std::string id,
                                   std::string lang,
                                   frost_bool activate,
                                   std::string label,
                                   RegisteredCallbacksConfig::TokenType token)
{
    ActivateSelectedComponentOnIdCallback callback;
    if (local_activate_selected_component_on_id_register.find(token, callback))
    {
        void* user_data = callback.m_user_data;
        auto function_pointer = callback.m_function_pointer;
        TRACE_INFO(("%s() token %u, user_data %p \n", __func__, token, user_data));

        if (function_pointer)
        {
            function_pointer(type,
                             id.c_str(),
                             lang.c_str(),
                             activate,
                             label.c_str(),
                             user_data);
        }
    }
}

}

#ifdef __cplusplus
extern "C"
{
#endif

frost_bool
NEBULA_RegisterAudioComponentCallback(
                                NEBULA_AudioComponentCallback callback,
                                void * user_data)
{
    return local_audio_component_register.registerCallback(
                            {callback, user_data},
                            IPC_NAME(NEBULA_RegisterAudioComponentCallback));
}

frost_bool
NEBULA_DeregisterAudioComponentCallback(void * user_data)
{
    return local_audio_component_register.deregisterCallback(
                            {nullptr, user_data},
                            IPC_NAME(NEBULA_DeregisterAudioComponentCallback));
}

frost_bool
NEBULA_RegisterFillComponentListCallback(
                        NEBULA_FillComponentListCallback fill_component_list_callback,
                        void* user_data)
{
    return local_fill_component_list_register.registerCallback(
                            {fill_component_list_callback, user_data},
                            IPC_NAME(NEBULA_RegisterFillComponentListCallback));
}

frost_bool
NEBULA_DeregisterFillComponentListCallback(void* user_data)
{
    return local_fill_component_list_register.deregisterCallback(
                            {nullptr, user_data},
                            IPC_NAME(NEBULA_DeregisterFillComponentListCallback));
}

frost_bool
NEBULA_RegisterActivateSelectedComponentCallback(
                        NEBULA_ActivateSelectedComponentCallback activate_selected_component_callback,
                        void* user_data)
{
    return local_activate_selected_component_register.registerCallback(
                            {activate_selected_component_callback, user_data},
                            IPC_NAME(NEBULA_RegisterActivateSelectedComponentCallback));
}

frost_bool
NEBULA_DeregisterActivateSelectedComponentCallback(void* user_data)
{
    return local_activate_selected_component_register.deregisterCallback( 
                            {nullptr, user_data},
                            IPC_NAME(NEBULA_DeregisterActivateSelectedComponentCallback));
}

frost_bool
NEBULA_RegisterActivateSelectedComponentOnIdCallback(
                        NEBULA_ActivateSelectedComponentOnIdCallback activate_selected_component_id_callback,
                        void* user_data)
{
    return local_activate_selected_component_on_id_register.registerCallback(
                            {activate_selected_component_id_callback, user_data},
                            IPC_NAME(NEBULA_RegisterActivateSelectedComponentOnIdCallback));
}

frost_bool
NEBULA_DeregisterActivateSelectedComponentOnIdCallback(void* user_data)
{
    return local_activate_selected_component_on_id_register.deregisterCallback(
                            {nullptr, user_data},
                            IPC_NAME(NEBULA_DeregisterActivateSelectedComponentOnIdCallback));
}

#ifdef __cplusplus
}
#endif

void NebulaComponentCallbackCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(NEBULA_AudioComponentCallback), proxyAudioComponent);
    server.bind(IPC_NAME(NEBULA_FillComponentListCallback), proxyFillComponentList);
    server.bind(IPC_NAME(NEBULA_ActivateSelectedComponentCallback),
                proxyActivateSelectedComponent);
    server.bind(IPC_NAME(NEBULA_ActivateSelectedComponentOnIdCallback),
                proxyActivateSelectedComponentOnId);
}
