/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the RegisteredCallbacksConfig class.
 */

#include "3rd_party_wrapper/rpclib/include/TokenUtilities.hpp"

template<typename PointerType>
RegisteredCallbacksConfig::TokenType
RegisteredCallbacksConfig::toToken(PointerType pointer)
{
    return pointerToToken<TokenType, PointerType>(pointer);
}

template<typename PointerType>
PointerType
RegisteredCallbacksConfig::toPointer(RegisteredCallbacksConfig::TokenType token)
{
    return tokenToPointer<TokenType, PointerType>(token);
}
