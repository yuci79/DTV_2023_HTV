/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Event dispatcher concrete event handlers
 */
#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAEVENTPROCESSOR_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAEVENTPROCESSOR_HPP_INCLUDED

#include "AnyOperaEventProcessor.hpp"

namespace omi
{
class Event;
}

template <typename TEvent, typename TEventHandler>
class OperaEventProcessor : public AnyOperaEventProcessor
{
public:
    OperaEventProcessor(TEventHandler handler)
        : m_handler(handler)
    {}

    virtual void processEvent(const omi::Event& event)
    {
        TEvent concrete_event(event);
        if (concrete_event)
        {
            m_handler(concrete_event);
        }
    }

private:
    TEventHandler m_handler;
};



#endif
