/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the RegisteredStreamHandlesConfig class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDSTREAMHANDLESCONFIG_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDSTREAMHANDLESCONFIG_HPP_INCLUDED

#include <limits>
#include <cstdint>

/**
 * @brief A class to configure types, constants and helper functions relating to 
 * ipc stream handles.
 */
class RegisteredStreamHandlesConfig
{
public:
    using TokenType = std::uint32_t;
    static const TokenType m_registered_token_limit = std::numeric_limits<TokenType>::max();
    static const TokenType m_invalid_token = 0;

    /**
     * @brief Converts a handle to a token.
     * @param handle The handle to convert.
     * @return The handle converted to a token.
     */
    template<typename HandleType>
    static TokenType toToken(HandleType handle);

    /**
     * @brief Converts a token to a handle.
     * @param token The token to convert.
     * @return The token converted to a handle.
     */
    template<typename HandleType>
    static HandleType toHandle(TokenType token);
};

#include "nebula_src/adaptation_layer/ipc/RegisteredStreamHandlesConfig.inl"

#endif
