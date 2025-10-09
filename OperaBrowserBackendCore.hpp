/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines OperaBrowserBackendCore.
 */

#ifndef CABOT_VEWD_INTEGRATION_IPC_OPERABROWSERBACKENDCORE_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_IPC_OPERABROWSERBACKENDCORE_HPP_INCLUDED

#include "vewd_integration/oipf/EventDispatchConfigurer.hpp"
#include "vewd_integration/oipf/OperaOipfGlobalDrmClient.hpp"

class NebulaBroadcastComponentsIpcExternal;

/**
 * @brief Provides a backend singleton to for the vewd browser process.
 */
class OperaBrowserBackendCore
{
public:  
    /**
     * @brief Create the OperaBrowserBackendCore.
     */
    static OperaBrowserBackendCore& instance();

    bool initialise();
       void discard();
       bool supportedType(int type);
       bool createMuminBackend(int type, void* client);


private:
    /**
     * @brief Constructor called from instance(), hence private.
     */
    OperaBrowserBackendCore();

    /**
     * @brief Destructor.
     */
    virtual ~OperaBrowserBackendCore();

private:
    EventDispatchConfigurer m_event_dispatch_configurer;
    NebulaBroadcastComponentsIpcExternal* m_broadcast_components_interface;
    OperaOipfGlobalDrmClient m_drm_client;
};

#endif // CABOT_VEWD_INTEGRATION_IPC_OPERABROWSERBACKENDCORE_HPP_INCLUDED
