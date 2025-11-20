/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares structures required for NebulaBroadcastComponentsIpcExternal.
 */
#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULABROADCASTCOMPONENTSIPCEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULABROADCASTCOMPONENTSIPCEXTERNAL_HPP_INCLUDED

#include "nebula/adaptation_layer/AnyAvComponentController.hpp"

/**
 * @brief Class to provide access to the NebulaBroadcastComponents via IPC. 
 */
class NebulaBroadcastComponentsIpcExternal: public AnyAvComponentController
{
public:
    NebulaBroadcastComponentsIpcExternal() = default;

    virtual ~NebulaBroadcastComponentsIpcExternal() = default;

public:
    // Implementing AnyAvComponentController
    bool getComponents(OIPF_ComponentType type, NEBULA_AVComponentList* component_list) override;
    bool selectComponent(NEBULA_AVComponent const* component) override;
    bool deselectComponent(NEBULA_AVComponent const* component) override;

};
#endif
