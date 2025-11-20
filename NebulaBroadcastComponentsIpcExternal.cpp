/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of NebulaBroadcastComponentsIpcExternal.
 *
 */

#include "NebulaBroadcastComponentsIpcExternal.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaComponentIpcTypes.hpp"

#include "NebulaIpcHelper.hpp"

#include <vector>


namespace nebula
{
static AnyAvComponentController* g_component_if = nullptr;

void setBroadcastComponentAccessor(AnyAvComponentController* broadcast_component_if)
{
    g_component_if = broadcast_component_if;
}

AnyAvComponentController* broadcastComponentAccessor()
{
    return g_component_if;
}

} // namespace nebula

bool 
NebulaBroadcastComponentsIpcExternal::getComponents(
            OIPF_ComponentType type, 
            NEBULA_AVComponentList* component_list)
{
    if (!component_list)
    {
        return false;
    }

    std::vector<MP_NEBULA_AVComponent> components;
    const auto result = nebulaRpcCall<bool, std::vector<MP_NEBULA_AVComponent>>(
                                        components,
                                        IPC_NAME(NebulaComponents_getComponents), 
                                        type);

    const auto count = (components.size() > MAX_COMPONENT_SIZE) ? MAX_COMPONENT_SIZE 
                                                                : components.size();

    component_list->number_of_current_components = static_cast<frost_int>(count);
    std::copy(components.begin(), components.begin() + count, component_list->components);

    return result;
}

bool
NebulaBroadcastComponentsIpcExternal::selectComponent(
            NEBULA_AVComponent const* component)
{
    if (!component)
    {
        return false;
    }

    const MP_NEBULA_AVComponent mp_component(*component);
    return nebulaRpcCall<bool>(IPC_NAME(NebulaComponents_selectComponent),
                               mp_component);
}

bool
NebulaBroadcastComponentsIpcExternal::deselectComponent(
            NEBULA_AVComponent const* component)
{
    if (!component)
    {
        return false;
    }

    const MP_NEBULA_AVComponent mp_component(*component);
    return nebulaRpcCall<bool>(IPC_NAME(NebulaComponents_deselectComponent),
                               mp_component);
}
