/*
 * Copyright 2018 Vestel.  All rights reserved.
 *
 * @file
 * @brief Template methods of OperaEngine.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAENGINE_INL_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAENGINE_INL_INCLUDED

template<typename TEvent, typename TReceiver>
void OperaEngine::addEventHandler(void (TReceiver::*handler)(TEvent),
                                  TReceiver* receiver)
{
    typedef std::mem_fun1_t<void, TReceiver, TEvent> MemberFunction;
    typedef std::binder1st<MemberFunction> BoundFunction;

    BoundFunction function = std::bind1st(std::mem_fun(handler), receiver);

    AnyOperaEventProcessor* processor = new OperaEventProcessor<TEvent,
        BoundFunction>(function);

    m_event_processors.insert(std::make_pair(TEvent::TYPE, processor));
}

template<typename TMessage>
typename TMessage::FutureType OperaEngine::post(const TMessage &message)
{
    tracePost(message);
    return typename TMessage::FutureType( m_opera->post(new TMessage(message)) );
}
#endif
