/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the AsyncMessageCallbacksIpcExternal class and global access functions.
 */

#include "AsyncMessageCallbacksIpcExternal.hpp"
#include "NebulaIpcHelper.hpp"
#include "nebula/core/browser_client/AnyVideoBroadcastObjectProxyInterface.hpp"
#include "nebula_src/adaptation_layer/AnyNebulaAsyncMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaDrmRightsMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaParentalRatingMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/ipc/AsyncMessageProcessorIpcTypes.hpp"
#include "utilities_debug/trace.h"

#include <rpc/server.h>

#include <vector>

TRACE_IMPLEMENT("nebula/ipc/external/AsyncMessageCallbackspIcExternal");

/**
 * @brief Template class to connect AsyncMessageProcessors in external to aurora via IPC. 
 */
template<class ProxyType>
class AsyncMessageCallbackRegister  
{
public:
    AsyncMessageCallbackRegister(AsyncMesssageProcessorRegistrationIpcType registration_type);

    /**
     * @brief Destructor.
     */    
    virtual ~AsyncMessageCallbackRegister();

    /**
     * @brief Accepts registration of a processor of async messages.
     *        Takes ownership of the given message processor.
     * @return true on successful registration, otherwise false.
     */
    bool registerAsyncMessageProcessor(ProxyType* processor);

    /**
     * @brief Remove registration of a processor of async messages.
     *        Deletes the message processor given in a previous call to
     *        registerAsyncMessageProcessor().
     * @return true on successful deregistration of the given processor, otherwise false.
     */
    bool deregisterAsyncMessageProcessor(AnyNebulaAsyncMessageProcessor* processor);

    /**
     * @brief Locks the m_processors_mutex and executes the given callable function 
     *        for all registered processors.
     */
    template <typename Callable> void execute(Callable&& func);

private:
    const AsyncMesssageProcessorRegistrationIpcType m_registration_type;
    std::vector<ProxyType*> m_processors;
    std::mutex m_processors_mutex;
};

/**
 * @brief Class to morph a NebulaDrmRightsMessageProcessor into an Ipc External proxy.
 */
class NebulaDrmRightsMessageProcessorProxy: public NebulaDrmRightsMessageProcessor 
{
public:
    /** 
     * @brief Constructor to consume the base class donor.
     * @param donor Base class pointer deleted in this constructor.
     */
    explicit NebulaDrmRightsMessageProcessorProxy(NebulaDrmRightsMessageProcessor* donor);

    /** 
     * @brief Process the event expected by this message processor proxy.
     * @param error Message packed representation of the 
     *              AnyVideoBroadcastObjectProxyInterface::onDrmRightsError parameters. 
     */
    void onDrmRightsError(const MP_OnDrmRightsError& error);
};

/**
 * @brief Class to morph a NebulaParentalRatingMessageProcessor into an Ipc External proxy.
 */
class NebulaParentalRatingMessageProcessorProxy: public NebulaParentalRatingMessageProcessor
{
public:
    /** 
     * @brief Constructor to consume the base class donor.
     * @param donor Base class pointer deleted in this constructor.
     */
    explicit NebulaParentalRatingMessageProcessorProxy(NebulaParentalRatingMessageProcessor* donor);

    /** 
     * @brief Process the event expected by this message processor proxy.
     * @param event Message packed representation of the 
     *              AnyVideoBroadcastObjectProxyInterface::onOipfParentalRatingEvent parameters. 
     */
    void onOipfParentalRatingEvent(const MP_OnOipfParentalRatingEvent& event);
};

namespace
{
AsyncMessageCallbackRegister<NebulaDrmRightsMessageProcessorProxy> 
            local_drm_rights_register(NebulaDrmRightsMessageRegistration);


AsyncMessageCallbackRegister<NebulaParentalRatingMessageProcessorProxy> 
            local_parental_rating_register(NebulaParentalRatingMessageRegistration);

void 
onDrmRightsError(const MP_OnDrmRightsError& error)
{
    local_drm_rights_register.execute([&error]
        (NebulaDrmRightsMessageProcessorProxy* processor)
    {
        processor->onDrmRightsError(error);
    });
}

void
onOipfParentalRatingEvent(const MP_OnOipfParentalRatingEvent& event)
{
    local_parental_rating_register.execute([&event]
        (NebulaParentalRatingMessageProcessorProxy* processor)
    {
        processor->onOipfParentalRatingEvent(event);
    });
}

} // namespace


void AsyncMessageCallbacksIpcExternal::bindToServer(rpc::server& server)
{
    server.bind(IPC_NAME(AsyncMessage_onDrmRightsError), &onDrmRightsError);

    server.bind(IPC_NAME(AsyncMessage_onOipfParentalRatingEvent), &onOipfParentalRatingEvent);
}

/**
 * @brief Implementation of AsyncMessageCallbackRegister.
 */
template<class ProxyType>
AsyncMessageCallbackRegister<ProxyType>::AsyncMessageCallbackRegister(
            AsyncMesssageProcessorRegistrationIpcType registration_type)
    : m_registration_type(registration_type)
{

}

template<class ProxyType>
AsyncMessageCallbackRegister<ProxyType>::~AsyncMessageCallbackRegister()
{
    std::lock_guard<std::mutex> lock(m_processors_mutex);
    for (const auto* processor : m_processors)
    {
        delete processor;
    }
    m_processors.clear();
}

template<class ProxyType>
bool
AsyncMessageCallbackRegister<ProxyType>::registerAsyncMessageProcessor(ProxyType* processor)
{
    TRACE_INFO(("%s() %p\n", __func__, processor));

    if (!processor)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_processors_mutex);    
    const bool was_empty = m_processors.empty();
    m_processors.push_back(processor);
    
    bool success = true;
    if (was_empty)
    {
        success = nebulaRpcCall<bool>(IPC_NAME(nebula_RegisterAsyncMessageCallback),
                                      m_registration_type);
    }

    if (!success)
    {
        TRACE_ERROR(("%s() RPC Registration failed\n", __func__));
        m_processors.pop_back();
        delete processor;
    }
    
    return success;
}

template<class ProxyType>
bool 
AsyncMessageCallbackRegister<ProxyType>::deregisterAsyncMessageProcessor(
                        AnyNebulaAsyncMessageProcessor* processor)
{
    TRACE_INFO(("%s() %p\n", __func__, processor));

    bool found = false;

    std::lock_guard<std::mutex> lock(m_processors_mutex);
    auto iter = std::find(m_processors.begin(), m_processors.end(), processor);
    if (iter != m_processors.end())
    {
        found = true;
        delete (*iter);   // Delete the concrete object.
        m_processors.erase(iter);
        if (m_processors.empty())
        {
            bool ok = nebulaRpcCall<bool>(IPC_NAME(nebula_DeregisterAsyncMessageCallback),
                                          m_registration_type);
            if (!ok)
            {
                TRACE_ERROR(("%s() RPC Deregistration failed\n", __func__));
            }
        }
    }
    
    return found;
}

template<class ProxyType> template<typename Callable>
void
AsyncMessageCallbackRegister<ProxyType>::execute(Callable&& func)
{
    std::lock_guard<std::mutex> lock(m_processors_mutex);
    for (auto& processor : m_processors)
    {
        if (processor)
        {
            func(processor);
        }
    }
}

/**
 * @brief Implementation of NebulaDrmRightsMessageProcessorProxy.
 */
NebulaDrmRightsMessageProcessorProxy::NebulaDrmRightsMessageProcessorProxy(
            NebulaDrmRightsMessageProcessor* donor)
    : NebulaDrmRightsMessageProcessor(*donor)
{
    delete donor;
}

void
NebulaDrmRightsMessageProcessorProxy::onDrmRightsError(const MP_OnDrmRightsError& error)
{
    m_vbo.onDrmRightsError(error.error_state,
                           error.content_id,
                           error.drm_system_id,
                           error.rights_issuer_url);
}

/**
 * @brief Implementation of NebulaParentalRatingMessageProcessorProxy.
 */
NebulaParentalRatingMessageProcessorProxy::NebulaParentalRatingMessageProcessorProxy(
            NebulaParentalRatingMessageProcessor* donor)
    : NebulaParentalRatingMessageProcessor(*donor)
{
    delete donor;
}

void 
NebulaParentalRatingMessageProcessorProxy::onOipfParentalRatingEvent(
                const MP_OnOipfParentalRatingEvent& event)
{
    m_vbo.onOipfParentalRatingEvent(event.rating_type,
                                    event.rating_value,
                                    event.rating_country,
                                    event.drm_system_id,
                                    event.blocked);
}

/**
 * Global interface to register/deregister.
 */
namespace nebula
{
AnyNebulaAsyncMessageProcessor* 
RegisterAsyncMessageCallback(NebulaDrmRightsMessageProcessor* processor)
{
    auto proxy = new NebulaDrmRightsMessageProcessorProxy(processor);
    return local_drm_rights_register.registerAsyncMessageProcessor(proxy) ? proxy : nullptr;
}

AnyNebulaAsyncMessageProcessor* 
RegisterAsyncMessageCallback(NebulaParentalRatingMessageProcessor* processor)
{
    auto proxy = new NebulaParentalRatingMessageProcessorProxy(processor);
    return local_parental_rating_register.registerAsyncMessageProcessor(proxy) ? proxy : nullptr;
}

void 
DeregisterAsyncMessageCallback(AnyNebulaAsyncMessageProcessor* processor)
{
    bool found = false;

    // Try all possible registers.
    found = found || local_drm_rights_register.deregisterAsyncMessageProcessor(processor);
    found = found || local_parental_rating_register.deregisterAsyncMessageProcessor(processor);

    if (!found)
    {
        TRACE_ERROR(("%s() failed to find %p\n", __func__, processor));
    }
}

} // namespace nebula
