/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines the RegisteredStreamHandlesConfig class.
 */

#include "3rd_party_wrapper/rpclib/include/TokenUtilities.hpp"

template<typename PointerType>
RegisteredStreamHandlesConfig::TokenType
RegisteredStreamHandlesConfig::toToken(PointerType pointer)
{
    return pointerToToken<TokenType, PointerType>(pointer);
}

template<typename PointerType>
PointerType
RegisteredStreamHandlesConfig::toHandle(RegisteredStreamHandlesConfig::TokenType token)
{
    return tokenToPointer<TokenType, PointerType>(token);
}
