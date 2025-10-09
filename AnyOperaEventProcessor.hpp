/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines an interface for event processors.
 */
#ifndef CABOT_VEWD_INTEGRATION_CORE_ANYOPERAEVENTPROCESSOR_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_ANYOPERAEVENTPROCESSOR_HPP_INCLUDED

namespace omi
{
class Event;
}

class AnyOperaEventProcessor
{
public:
    virtual void processEvent(const omi::Event& event) = 0;
    virtual ~AnyOperaEventProcessor() {}
};

#endif
