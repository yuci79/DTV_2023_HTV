/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for the RegisteredStreamHandles template class.
 */

template<typename StreamHandleType>
RegisteredStreamHandles<StreamHandleType>::RegisteredStreamHandles()
    : TokenMap<RegisteredStreamHandlesConfig::TokenType, StreamHandleType>(
            RegisteredStreamHandlesConfig::m_registered_token_limit,
            RegisteredStreamHandlesConfig::m_invalid_token)
{
}
