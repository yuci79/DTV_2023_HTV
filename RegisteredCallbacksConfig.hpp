/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the RegisteredCallbacksConfig class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDCALLBACKSCONFIG_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_REGISTEREDCALLBACKSCONFIG_HPP_INCLUDED

#include <cstdint>

/**
* @brief A class to configure types, constants and helper functions relating to
* ipc callback registration.
*/
class RegisteredCallbacksConfig
{
public:
    using TokenType = std::uint32_t;
    static const TokenType m_registered_token_limit = 100;
    static const TokenType m_invalid_token = 0;

    /**
     * @brief Converts a pointer to a token.
     * @param pointer The pointer to convert.
     * @return The pointer converted to a token.
     */
    template<typename PointerType>
    static TokenType toToken(PointerType pointer);

    /**
     * @brief Converts a token to a pointer.
     * @param token The token to convert.
     * @return The token converted to a pointer.
     */
    template<typename PointerType>
    static PointerType toPointer(TokenType token);
};

#include "nebula_src/adaptation_layer/ipc/RegisteredCallbacksConfig.inl"

#endif
