/**
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the RegisteredCallbacks class template functions.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDCALLBACKS_INL_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDCALLBACKS_INL_INCLUDED

#include "NebulaIpcHelper.hpp"

/**
 * @note This inline file uses TRACE macros. 
 *       It MUST NOT be included in a header file.
 *       Where used it MUST be included in a cpp source file AFTER the TRACE_IMPLEMENT macro.
 */

template<typename CallbackType>
RegisteredCallbacks<CallbackType>::RegisteredCallbacks()
    : TokenMap<RegisteredCallbacksConfig::TokenType, CallbackType>(
            RegisteredCallbacksConfig::m_registered_token_limit,
            RegisteredCallbacksConfig::m_invalid_token)
{
}

template<typename CallbackType>
template<typename... Args>
frost_bool 
RegisteredCallbacks<CallbackType>::registerCallback(const CallbackType& callback,
                                                    const std::string& ipc_name,
                                                    Args&&... args)
{
    bool duplicate = false;
    // Two phase name lookup - 'this->' required to reference dependant base class declarations.
    const auto token = this->add(callback, duplicate);
    if (token == this->m_invalid_token)
    {
        TRACE_WARN(("%s() No token\n", __func__));
        return frost_false;
    }

    const frost_bool success = nebulaRpcCall<frost_bool>(ipc_name, token,
                                                         std::forward<Args>(args)...);
    if ((success == frost_false) && !duplicate)
    {
        if (!this->remove(token))
        {
            TRACE_ERROR(("%s() Failed to remove token %u\n", __func__, token));
        }
    }

    TRACE_INFO(("%s() token %u, %ssuccess %s\n", __func__,
                token,
                duplicate ? "duplicate, " : "",
                success ? "true" : "false"));

    return success;
}

template<typename CallbackType>
template<typename... Args>
frost_bool 
RegisteredCallbacks<CallbackType>::deregisterCallback(const CallbackType& callback,
                                                      const std::string& ipc_name,
                                                      Args&&... args)
{
    // Two phase name lookup - 'this->' required to reference dependant base class declarations.
    const auto token = this->remove(callback);
    if (token == this->m_invalid_token)
    {
        TRACE_WARN(("%s() No token\n", __func__));
        return frost_false;
    }

    const frost_bool success = nebulaRpcCall<frost_bool>(ipc_name, token,
                                                         std::forward<Args>(args)...);

    TRACE_INFO(("%s() token %u, success %s\n", __func__,
                token,
                success? "true" : "false"));
    return success;
}

#endif
