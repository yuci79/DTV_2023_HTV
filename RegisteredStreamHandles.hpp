/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declares the RegisteredStreamHandles class.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_REGISTEREDSTREAMHANDLES_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_AURORA_REGISTEREDSTREAMHANDLES_HPP_INCLUDED

#include "nebula_src/adaptation_layer/ipc/RegisteredStreamHandlesConfig.hpp"
#include "3rd_party_wrapper/rpclib/include/TokenMap.hpp"

/**
 * @brief A template class for mapping a TokenType to a StreamHandleType.
 */
template<typename StreamHandleType>
class RegisteredStreamHandles 
    : public TokenMap<RegisteredStreamHandlesConfig::TokenType, StreamHandleType>
{
public:
    RegisteredStreamHandles();
};

#include "nebula_src/adaptation_layer/ipc/aurora/RegisteredStreamHandles.inl"

#endif
