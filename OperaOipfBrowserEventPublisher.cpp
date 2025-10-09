/*
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Publisher for HBBTV browser events
 */
#include "OperaOipfBrowserEventPublisher.hpp"

#include "nebula/browser_interface/AnyWebBrowserOipfEventListener.hpp"

#include <algorithm>
#include <string>

#include "utilities_public/AnyCommand.hpp"
#include "utilities_public/AnyCommandThread.hpp"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("opera/oipf_browser_publisher");

////////////////////////////////////////////////////////////////////////////////

class OperaOipfBrowserEventPublisher::ApplicationStartedCommand : public AnyCommand
{
public:
    ApplicationStartedCommand(OperaOipfBrowserEventPublisher & publisher, 
                                  const char* url);
    void execute() override;

private:
    OperaOipfBrowserEventPublisher & m_publisher;
    std::string m_url;
};

OperaOipfBrowserEventPublisher::ApplicationStartedCommand::ApplicationStartedCommand(
    OperaOipfBrowserEventPublisher & publisher, 
    const char* url)
    : m_publisher(publisher)
    , m_url(url)
{
}

void
OperaOipfBrowserEventPublisher::ApplicationStartedCommand::execute()
{
    m_publisher.internalNotifyApplicationStarted(m_url.c_str());
}

////////////////////////////////////////////////////////////////////////////////

class OperaOipfBrowserEventPublisher::ApplicationStoppedCommand : public AnyCommand
{
public:
    ApplicationStoppedCommand(OperaOipfBrowserEventPublisher & publisher);
    void execute() override;

private:
    OperaOipfBrowserEventPublisher & m_publisher;
};

OperaOipfBrowserEventPublisher::ApplicationStoppedCommand::ApplicationStoppedCommand(
    OperaOipfBrowserEventPublisher & publisher)
    : m_publisher(publisher)
{
}

void
OperaOipfBrowserEventPublisher::ApplicationStoppedCommand::execute()
{
    m_publisher.internalNotifyApplicationStopped();
}

////////////////////////////////////////////////////////////////////////////////
class OperaOipfBrowserEventPublisher::ApplicationFailedCommand : public AnyCommand
{
public:
    ApplicationFailedCommand(OperaOipfBrowserEventPublisher & publisher);
    void execute() override;

private:
    OperaOipfBrowserEventPublisher & m_publisher;
};

OperaOipfBrowserEventPublisher::ApplicationFailedCommand::ApplicationFailedCommand(
    OperaOipfBrowserEventPublisher & publisher)
    : m_publisher(publisher)
{
}

void
OperaOipfBrowserEventPublisher::ApplicationFailedCommand::execute()
{
    m_publisher.internalNotifyApplicationFailed();
}
////////////////////////////////////////////////////////////////////////////////

OperaOipfBrowserEventPublisher::OperaOipfBrowserEventPublisher()
    : m_thread(nullptr)
    , m_listeners_mutex_initialised(false)
{
    // Initialise the mutex in the constructor to allow attach()/detach()
    // calls before the browser core has fully initialised.
    m_listeners_mutex_initialised = m_listeners_mutex.initialise();
}

ExplodingBoolean
OperaOipfBrowserEventPublisher::initialise(AnyCommandThread & thread)
{
    m_thread = &thread;
    return ExplodingBoolean(m_listeners_mutex_initialised);
}

void
OperaOipfBrowserEventPublisher::detach(AnyWebBrowserOipfEventListener* listener)
{
    TRACE_VERBOSE(("deregisterListener() called %p\n", listener));
    FrostMutexLock lock(m_listeners_mutex);

    // erase-remove idiom
    m_listeners.erase(std::remove(m_listeners.begin(),
                                      m_listeners.end(),
                                      listener),
                                      m_listeners.end());
}

void
OperaOipfBrowserEventPublisher::attach(AnyWebBrowserOipfEventListener* listener)
{
    TRACE_VERBOSE(("registerListener(AnyWebBrowserOipfEventListener) called %p\n", listener));
    CABOT_ASSERT(listener);

    FrostMutexLock lock(m_listeners_mutex);
    if (std::find(m_listeners.begin(),
                  m_listeners.end(),
                  listener) == m_listeners.end())
    {
        m_listeners.push_back(listener);
    }
}

void
OperaOipfBrowserEventPublisher::notifyOipfAppStarted(const char* url)
{
    TRACE_VERBOSE(("notifyOipfAppStarted()\n"));
    CABOT_ASSERT(m_thread);

    m_thread->enqueueCommand(new ApplicationStartedCommand(*this, url));
}

void
OperaOipfBrowserEventPublisher::internalNotifyApplicationStarted(const char* url) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto & listener : m_listeners)
    {
        listener->notifyOipfStarted(url);
    }
}

void
OperaOipfBrowserEventPublisher::notifyOipfAppStopped()
{
    TRACE_VERBOSE(("notifyOipfAppStopped()\n"));
    CABOT_ASSERT(m_thread);

    m_thread->enqueueCommand(
            new ApplicationStoppedCommand(*this));
}

void
OperaOipfBrowserEventPublisher::internalNotifyApplicationStopped() const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto & listener : m_listeners)
    {
        listener->notifyOipfStopped();
    }
}

void
OperaOipfBrowserEventPublisher::notifyOipfAppFailed()
{
    TRACE_VERBOSE(("notifyOipfAppFailed()\n"));
    CABOT_ASSERT(m_thread);

    m_thread->enqueueCommand(
            new ApplicationFailedCommand(*this));
}

void
OperaOipfBrowserEventPublisher::internalNotifyApplicationFailed() const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (auto & listener : m_listeners)
    {
        listener->notifyOipfFailed();
    }
}

// EOF 
