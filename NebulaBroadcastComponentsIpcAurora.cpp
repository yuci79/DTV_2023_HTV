/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaBroadcastComponentsIpcAurora class.
 */

#include "NebulaBroadcastComponentsIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaComponentIpcTypes.hpp"
#include "nebula_src/adaptation_layer/NebulaBroadcastComponentsInterface.hpp"

#include <rpc/server.h>

#include <vector>

void
NebulaBroadcastComponentsIpcAurora::bindToServer(rpc::server& server)
{
    // Add bindings for AnyAvComponentController functions
    server.bind(IPC_NAME(NebulaComponents_getComponents), [](OIPF_ComponentType type)
    {
        NebulaIpcType<bool, std::vector<MP_NEBULA_AVComponent>> result;
        result.return_value = false;
  
        auto broadcast_component_accessor = nebula::broadcastComponentAccessor();
        if (broadcast_component_accessor)
        {
            NEBULA_AVComponentList component_list {};
            result.return_value = broadcast_component_accessor->getComponents(type, 
                                                                              &component_list);
            const NEBULA_AVComponent* first = component_list.components;
            result.output_params.assign(first,
                                        first + component_list.number_of_current_components);
        }
        return result;
    });

    server.bind(IPC_NAME(NebulaComponents_selectComponent), 
            [](const MP_NEBULA_AVComponent component)
    {
        auto broadcast_component_accessor = nebula::broadcastComponentAccessor();
        if (!broadcast_component_accessor)
        {
            return false;
        }
        return broadcast_component_accessor->selectComponent(&component);
    });

    server.bind(IPC_NAME(NebulaComponents_deselectComponent),
            [](const MP_NEBULA_AVComponent component)
    {
        auto broadcast_component_accessor = nebula::broadcastComponentAccessor();
        if (!broadcast_component_accessor)
        {
            return false;
        }
        return broadcast_component_accessor->deselectComponent(&component);
    });
}
