/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Declarations of AsyncMessageProcessor registration functions which are provided
 *        only in IPC external.
 */

#ifndef CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAASYNCMESSAGEPROCESSORS_HPP_INCLUDED
#define CABOT_NEBULA_SRC_ADAPTATION_LAYER_IPC_NEBULAASYNCMESSAGEPROCESSORS_HPP_INCLUDED

#include "nebula_src/adaptation_layer/AnyNebulaAsyncMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaDrmRightsMessageProcessor.hpp"
#include "nebula_src/adaptation_layer/NebulaParentalRatingMessageProcessor.hpp"

/**
 * @note In aurora registration is handled by one function taking a base class pointer,
 *       RegisterAsyncMessageCallback(AnyNebulaAsyncMessageProcessor*)
 */
namespace nebula
{
AnyNebulaAsyncMessageProcessor* RegisterAsyncMessageCallback(NebulaDrmRightsMessageProcessor*);
AnyNebulaAsyncMessageProcessor* RegisterAsyncMessageCallback(NebulaParentalRatingMessageProcessor*);
}

#endif