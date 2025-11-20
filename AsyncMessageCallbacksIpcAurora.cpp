/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the AsyncMessageCallbacksIpcAurora class and helpers.
 */

#include "AsyncMessageCallbacksIpcAurora.hpp"
#include "VideoBroadcastObjectProxyIpcAurora.hpp"
#include "nebula_src/adaptation_layer/AnyNebulaAsyncMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaDrmRightsMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaParentalRatingMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/ipc/AsyncMessageProcessorIpcTypes.hpp"
#include "utilities_debug/trace.h"

namespace nebula
{
AnyNebulaAsyncMessageProcessor* RegisterAsyncMessageCallback(AnyNebulaAsyncMessageProcessor* );
void DeregisterAsyncMessageCallback(AnyNebulaAsyncMessageProcessor* );
} // namespace nebula

#include <rpc/server.h>

TRACE_IMPLEMENT("nebula/ipc/aurora/AsyncMessageCallbacksIpcAurora");

namespace
{
    AsyncMessageCallbacksIpcAurora local_proxy;
    VideoBroadcastObjectProxyIpcAurora local_ipc_vbo;    
} // namespace


AsyncMessageCallbacksIpcAurora::AsyncMessageCallbacksIpcAurora()
    : m_processor_handles({})
{

}

bool
AsyncMessageCallbacksIpcAurora::createProcessor(
                AsyncMesssageProcessorRegistrationIpcType registration_type)
{
    TRACE_INFO(("%s() Registration type %u\n", __func__, registration_type));

    if (registration_type >= MaxRegistrationIpcTypes)
    {
        TRACE_ERROR(("%s() Unexpected registration type %u\n", __func__, registration_type));
        return false;
    }

    auto processor_handle = m_processor_handles.at(registration_type);
    if (processor_handle)
    {
        TRACE_ERROR(("%s() Processor already exists for type %u\n", __func__, registration_type));
        return false;
    }

    switch (registration_type)
    {
        case NebulaDrmRightsMessageRegistration:
        {
            auto concrete = new NebulaDrmRightsMessageProcessor(local_ipc_vbo);
            processor_handle = nebula::RegisterAsyncMessageCallback(concrete);
            break;
        }

        case NebulaParentalRatingMessageRegistration:
        {
            auto concrete = new NebulaParentalRatingMessageProcessor(local_ipc_vbo);
            processor_handle = nebula::RegisterAsyncMessageCallback(concrete);
            break;
        }

        default:
            processor_handle = nullptr;
            break;
    }

    if (processor_handle)
    {
        m_processor_handles.at(registration_type) = processor_handle;
    }

    return !!processor_handle;
}

bool
AsyncMessageCallbacksIpcAurora::removeProcessor(
                AsyncMesssageProcessorRegistrationIpcType registration_type)
{
    TRACE_INFO(("%s() Registration type %u\n", __func__, registration_type));

    if (registration_type >= MaxRegistrationIpcTypes)
    {
        TRACE_ERROR(("%s() Unexpected registration type %u\n", __func__, registration_type));
        return false;
    }

    auto processor_handle = m_processor_handles.at(registration_type);
    if (!processor_handle)
    {
        TRACE_ERROR(("%s() Processor not found for type %u\n", __func__, registration_type));
        return false;
    }

    nebula::DeregisterAsyncMessageCallback(processor_handle);
    m_processor_handles.at(registration_type) = nullptr;
    return true;
}

void
AsyncMessageCallbacksIpcAurora::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(nebula_RegisterAsyncMessageCallback),
                [](AsyncMesssageProcessorRegistrationIpcType registration_type)
    {
        return local_proxy.createProcessor(registration_type); 
    });

    server.bind(IPC_NAME(nebula_DeregisterAsyncMessageCallback),
                [](AsyncMesssageProcessorRegistrationIpcType registration_type)   
    {
        return local_proxy.removeProcessor(registration_type);
    });
}

void
AsyncMessageCallbacksIpcAurora::reset()
{
    TRACE_INFO(("%s()\n", __func__));

    local_proxy.deregisterProcessors();
}

void
AsyncMessageCallbacksIpcAurora::deregisterProcessors()
{
    for (auto& processor_handle : m_processor_handles)
    {
        if (processor_handle)
        {
            nebula::DeregisterAsyncMessageCallback(processor_handle);
            processor_handle = nullptr;
        }
    }
}