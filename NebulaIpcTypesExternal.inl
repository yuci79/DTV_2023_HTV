/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Helper functions for nebula ipc external.
 */

template<typename NebulaType, typename IpcType>
NebulaType*
createNebulaFromIpc(const IpcType& ipc_item)
{
    NebulaType* nebula_item = static_cast<NebulaType*>(malloc(sizeof(NebulaType)));
    if (nebula_item)
    {
        *nebula_item = ipc_item;
    }
    return nebula_item;
}

template<typename NebulaType,
         typename IpcVectorType,
         typename CountType>
void
createNebulaArrayFromIpcVector(const IpcVectorType& ipc_vector,
                               CountType& count,
                               NebulaType**& nebula_array)
{
    nebula_array = nullptr;
    count = static_cast<CountType>(ipc_vector.size());
    if (count > 0)
    {
        nebula_array = static_cast<NebulaType**>(malloc(count * sizeof(NebulaType*)));
        if (nebula_array)
        {
            unsigned int index = 0;
            for (const auto& ipc_item : ipc_vector)
            {
                nebula_array[index++] = createNebulaFromIpc<NebulaType>(ipc_item);
            }
        }
    }
}