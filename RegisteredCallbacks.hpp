/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the RegisteredCallbacks class
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_REGISTEREDCALLBACKS_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_EXTERNAL_REGISTEREDCALLBACKS_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.hpp"
#include "3rd_party_wrapper/rpclib/include/TokenMap.hpp"
#include "frost/frost_basictypes.h"

/**
 * @brief A template class for mapping a TokenType to a CallbackType.
 */
template<typename CallbackType>
class RegisteredCallbacks 
    : public TokenMap<RegisteredCallbacksConfig::TokenType, CallbackType>
{
public:

    /**
     * @brief Constructor.
     */
    RegisteredCallbacks();

   /**
     * @brief Commonly used wrapper for TokenMap::add().
     *        Handles duplicates rejected or accepted by the aurora register function.
     *        Registers the callback with the aurora peer and if successful adds the 
     *        callback and token to the collection of registered callbackTypes. 
     * @param callback Reference to the CallbackType to be added.
     * @param ipc_name Rpc name string to call the aurora register command for this callbackType.
     * @param args An optional set of arguments to pass to the rpc function.
     * @return frost_true if successful, otherwise frost_false.
     */
    template<typename... Args>
    frost_bool registerCallback(const CallbackType& callback,
                                const std::string& ipc_name,
                                Args&&... args);

   /**
     * @brief Commonly used wrapper for TokenMap::remove(callback). 
     *        Finds the registered callback, removes from the collection of registered 
     *        callbackTypes and deregisters with the aurora peer.
     * @param callback Reference to the CallbackType to be removed.
     * @param ipc_name Rpc name string to call the aurora deregister command for this callbackType.
     * @param args An optional set of arguments to pass to the rpc function.
     * @return frost_true if successful, otherwise frost_false.
     */
    template<typename... Args>
    frost_bool deregisterCallback(const CallbackType& callback,
                                  const std::string& ipc_name,
                                  Args&&... args);
};
#endif
