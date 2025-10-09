/*
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Publisher for Media Player browser events.
 */
#include "OperaMediaPlayerEventPublisher.hpp"

#include "nebula/browser_interface/AnyWebBrowserMediaPlayerEventListener.hpp"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

#include "utilities_public/AnyCommand.hpp"
#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/String.hpp"

#include <algorithm>


class OperaMediaPlayerPlayingCommand: public AnyCommand
{
public:
    OperaMediaPlayerPlayingCommand(
        OperaMediaPlayerEventPublisher& publisher,
        bool const is_playing);
    ~OperaMediaPlayerPlayingCommand() { }
    void execute();
    
private:
    OperaMediaPlayerEventPublisher& m_publisher;
    bool const m_is_playing;
};

OperaMediaPlayerPlayingCommand::OperaMediaPlayerPlayingCommand(
    OperaMediaPlayerEventPublisher& publisher,
    bool const is_playing)
: m_publisher(publisher)
, m_is_playing(is_playing)
{
}

void
OperaMediaPlayerPlayingCommand::execute()
{
    m_publisher.internalPlayingState(m_is_playing);
}

class OperaMediaPlayerEventCommand: public AnyCommand
{
public:
    enum class Action
    {
        started,
        error,
        terminated
    };
    OperaMediaPlayerEventCommand(
        OperaMediaPlayerEventPublisher& publisher,
        OperaMediaPlayerEventCommand::Action const action,
        cabot::String const& url);
    ~OperaMediaPlayerEventCommand() { }
    void execute();
    
private:
    OperaMediaPlayerEventPublisher& m_publisher;
    OperaMediaPlayerEventCommand::Action const m_action;
    cabot::String const m_url;
};

OperaMediaPlayerEventCommand::OperaMediaPlayerEventCommand
    (OperaMediaPlayerEventPublisher& publisher, 
     OperaMediaPlayerEventCommand::Action const action,
     cabot::String const& url)
    : m_publisher(publisher)
    , m_action(action)
    , m_url(url)
{}

void
OperaMediaPlayerEventCommand::execute()
{
    switch (m_action)
    {
    case OperaMediaPlayerEventCommand::Action::started:
        m_publisher.internalPlayerStarted(m_url);
        break;

    case OperaMediaPlayerEventCommand::Action::error:
        m_publisher.internalPlayerError(m_url);
        break;

    case OperaMediaPlayerEventCommand::Action::terminated:
        m_publisher.internalPlayerTerminated(m_url);  
        break;

    default:
        SHOULD_NOT_BE_HERE();
    }
}

OperaMediaPlayerEventPublisher::OperaMediaPlayerEventPublisher()
    :m_thread(nullptr)
    , m_listeners_mutex_initialised(false)
{
    // see comment in OperaHbbTVBrowserEventPublisher
    m_listeners_mutex_initialised = m_listeners_mutex.initialise();
}

ExplodingBoolean
OperaMediaPlayerEventPublisher::initialise(AnyCommandThread & thread)
{
    m_thread = &thread;
    return ExplodingBoolean(m_listeners_mutex_initialised);
}

void
OperaMediaPlayerEventPublisher::detach(AnyWebBrowserMediaPlayerEventListener* listener)
{
    FrostMutexLock lock(m_listeners_mutex);
    m_event_listeners.erase(
        std::remove(m_event_listeners.begin(), 
                    m_event_listeners.end(), 
                    listener), m_event_listeners.end());
}

void
OperaMediaPlayerEventPublisher::attach(AnyWebBrowserMediaPlayerEventListener* listener)
{
    CABOT_ASSERT(listener);
    if (listener)
    {
        FrostMutexLock lock(m_listeners_mutex);
        if (std::find(m_event_listeners.begin(), 
                    m_event_listeners.end(), 
                    listener) == m_event_listeners.end())
        {
            m_event_listeners.push_back(listener);
        }
    }
}

void 
OperaMediaPlayerEventPublisher::notifyPlayingState(bool const is_playing)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
            new OperaMediaPlayerPlayingCommand(*this, is_playing));
    }
}

void 
OperaMediaPlayerEventPublisher::notifyUrlLoadingFinished(cabot::String const& url)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
            new OperaMediaPlayerEventCommand(
                *this, OperaMediaPlayerEventCommand::Action::started, url));
    }
}

void 
OperaMediaPlayerEventPublisher::notifyPlayerError(cabot::String const& url)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
            new OperaMediaPlayerEventCommand(
                *this, OperaMediaPlayerEventCommand::Action::error, url));
    }
}

void 
OperaMediaPlayerEventPublisher::notifyPlayerTerminated(cabot::String const& url)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
            new OperaMediaPlayerEventCommand(
                *this, OperaMediaPlayerEventCommand::Action::terminated, url));
    }
}

void 
OperaMediaPlayerEventPublisher::internalPlayerStarted(cabot::String const& url)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto& listener : m_event_listeners)
    {
        listener->notifyUrlLoadingFinished(url);
    }
}

void 
OperaMediaPlayerEventPublisher::internalPlayerError(cabot::String const& url)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto& listener : m_event_listeners)
    {
        listener->notifyPlayerError(url);
    }
}

void 
OperaMediaPlayerEventPublisher::internalPlayerTerminated(cabot::String const& url)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto& listener : m_event_listeners)
    {
        listener->notifyPlayerTerminated(url);
    }
}


void 
OperaMediaPlayerEventPublisher::internalPlayingState(bool const is_playing)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto& listener : m_event_listeners)
    {
        listener->notifyPlayingState(is_playing);
    }
}