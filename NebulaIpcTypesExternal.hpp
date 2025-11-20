/**
 * Copyright 2023 Vestel.  All rights reserved.
 *
 * @file
 * @brief Helper functions for nebula ipc external.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAIPCTYPESEXTERNAL_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_NEBULAIPCTYPESEXTERNAL_HPP_INCLUDED

#include "frost/frost_basictypes.h"
#include <vector>
#include <string>

/**
 * @brief Create a NEBULA object from an Ipc object.
 * @note The created object will be freed by the caller.
 *
 * @param ipc_item is the Ipc object to be copied.
 * 
 * @return pointer to the created object.
 */
template<typename NebulaType, typename IpcType>
NebulaType*
createNebulaFromIpc(const IpcType& ipc_item);

/**
 * @brief Template specialization of createNebulaFromIpc for strings.
 */
template<>
char*
createNebulaFromIpc(const std::string& ipc_item);

/**
 * @brief Create a NEBULA C array from a vector of MP_NEBULA_* objects.
 * @note The output array and its contents will be freed by the caller.
 *
 * @param ipc_vector   The vector of MP_NEBULA_* data.
 * @param count        [out] The number of items.
 * @param nebula_array [out] Array of NEBULA pointers.
 */
template<typename NebulaType,
         typename IpcVectorType,
         typename CountType>
void
createNebulaArrayFromIpcVector(const IpcVectorType& ipc_vector,
                               CountType& count,
                               NebulaType**& nebula_array);

#include "nebula_src/adaptation_layer/ipc/external/NebulaIpcTypesExternal.inl"

#endif