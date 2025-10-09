/*
 * Copyright 2017 Vestel.  All rights reserved.
 *
 * @file
 * @brief Publisher for HBBTV browser events
 */
#include "OperaHbbTVBrowserEventPublisher.hpp"
#include "HbbTVApplicationStopped.hpp"

#include "nebula/browser_interface/AnyWebBrowserHbbTVEventListener.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_public/AnyCommand.hpp"
#include "utilities_public/AnyCommandThread.hpp"
#include "utilities_public/String.hpp"

#include <omi/events/AITParsingStatus.h>
#include <omi/events/HbbTVAITAppLoadRequest.h>
#include <omi/events/HbbTVApplicationStarted.h>
#include <omi/events/HbbTVBroadcastRelatedStatusChanged.h>
#include <omi/events/HbbTVAppURLLoadRequest.h>

#include <algorithm>

TRACE_IMPLEMENT("opera/hbbtv_browser_publisher");


/**
 * AitParsingStatusCommand
 */
class AitParsingStatusCommand: public AnyCommand
{
public:
    AitParsingStatusCommand(OperaHbbTVBrowserEventPublisher& publisher,
                            omi::evt::AITParsingStatus const& ait_parsing_status);

    void execute() override;

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    omi::evt::AITParsingStatus const m_ait_parsing_status;
};

AitParsingStatusCommand::AitParsingStatusCommand(
        OperaHbbTVBrowserEventPublisher& publisher,
        omi::evt::AITParsingStatus const& ait_parsing_status)
    : m_publisher(publisher)
    , m_ait_parsing_status(ait_parsing_status)
{
}

void
AitParsingStatusCommand::execute()
{
    m_publisher.internalNotifyAitParsingStatus(m_ait_parsing_status);
}

/**
 * NotStartedNotificationCommand
 */
class NotStartedNotificationCommand: public AnyCommand
{
public:
    NotStartedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher,
                                  const bool ait_parsing);
    ~NotStartedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const bool m_ait_parsing;
};

NotStartedNotificationCommand::NotStartedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher, const bool ait_parsing)
    : m_publisher(publisher)
    , m_ait_parsing(ait_parsing)
{}

void
NotStartedNotificationCommand::execute()
{
    m_publisher.internalNotifyNotStarted(m_ait_parsing);
}

/**
 * NotStartedOnUrlNotificationCommand
 */
class NotStartedOnUrlNotificationCommand: public AnyCommand
{
public:
    NotStartedOnUrlNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher,
                                       cabot::String const& url,
                                       const bool load_aborted);
    ~NotStartedOnUrlNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    cabot::String const m_url;
    const bool m_load_aborted;
};

NotStartedOnUrlNotificationCommand::NotStartedOnUrlNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher, 
     cabot::String const& url,
     bool const load_aborted)
    : m_publisher(publisher)
    , m_url(url)
    , m_load_aborted(load_aborted)
{}

void
NotStartedOnUrlNotificationCommand::execute()
{
    m_publisher.internalNotifyNotStarted(m_url, m_load_aborted);
}

/**
 * StartedNotificationCommand
 */
class StartedNotificationCommand: public AnyCommand
{
public:
    StartedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher, 
                               omi::evt::HbbTVApplicationStarted const& application);
    ~StartedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const omi::evt::HbbTVApplicationStarted m_application;
};

StartedNotificationCommand::StartedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher, omi::evt::HbbTVApplicationStarted const& application)
    : m_publisher(publisher)
    , m_application(application)    
{}

void
StartedNotificationCommand::execute()
{
    m_publisher.internalNotifyStarted(m_application);
}

/**
 * StoppeddNotificationCommand
 */
class StoppedNotificationCommand: public AnyCommand
{
public:
    StoppedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher, 
                               vewd_integ::HbbTVApplicationStopped const& application);
    ~StoppedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const vewd_integ::HbbTVApplicationStopped m_application;
};

StoppedNotificationCommand::StoppedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher, vewd_integ::HbbTVApplicationStopped const& application)
    : m_publisher(publisher)
    , m_application(application)    
{}

void
StoppedNotificationCommand::execute()
{
    m_publisher.internalNotifyStopped(m_application);
}

/**
 * SuspendedNotificationCommand
 */
class SuspendedNotificationCommand: public AnyCommand
{
public:
    SuspendedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher);
    ~SuspendedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
};

SuspendedNotificationCommand::SuspendedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher)
    : m_publisher(publisher)
{}

void
SuspendedNotificationCommand::execute()
{
    m_publisher.internalNotifySuspended();
}

/**
 * ResumedNotificationCommand
 */
class ResumedNotificationCommand: public AnyCommand
{
public:
    ResumedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher);
    ~ResumedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
};

ResumedNotificationCommand::ResumedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher)
    : m_publisher(publisher)
{}

void
ResumedNotificationCommand::execute()
{
    m_publisher.internalNotifyResumed();
}

/**
 * InitialisedNotificationCommand
 */
class InitialisedNotificationCommand: public AnyCommand
{
public:
    InitialisedNotificationCommand(OperaHbbTVBrowserEventPublisher& publisher);
    ~InitialisedNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
};

InitialisedNotificationCommand::InitialisedNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher)
    : m_publisher(publisher)
{}

void
InitialisedNotificationCommand::execute()
{
    m_publisher.internalNotifyInitialised();
}

/**
 *  BroadcastRelatedStatusNotificationCommand
 */
class BroadcastRelatedStatusNotificationCommand: public AnyCommand
{
public:
    BroadcastRelatedStatusNotificationCommand
        (OperaHbbTVBrowserEventPublisher& publisher,
         omi::evt::HbbTVBroadcastRelatedStatusChanged const& application);
    ~BroadcastRelatedStatusNotificationCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    omi::evt::HbbTVBroadcastRelatedStatusChanged m_application;
};

BroadcastRelatedStatusNotificationCommand::BroadcastRelatedStatusNotificationCommand
    (OperaHbbTVBrowserEventPublisher& publisher,
     omi::evt::HbbTVBroadcastRelatedStatusChanged const& application)
    : m_publisher(publisher)
    , m_application(application)
{}

void
BroadcastRelatedStatusNotificationCommand::execute()
{
    m_publisher.internalNotifyBroadcastStatus(m_application);
}

/**
 *  AitAppLoadRequestCommand
 */
class AitAppLoadRequestCommand: public AnyCommand
{
public:
    AitAppLoadRequestCommand
        (OperaHbbTVBrowserEventPublisher& publisher,
         omi::evt::HbbTVAITAppLoadRequest const& app_load_request);
    ~AitAppLoadRequestCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const omi::evt::HbbTVAITAppLoadRequest m_app_load_request;
};

AitAppLoadRequestCommand::AitAppLoadRequestCommand
    (OperaHbbTVBrowserEventPublisher& publisher,
     omi::evt::HbbTVAITAppLoadRequest const& app_load_request)
    : m_publisher(publisher)
    , m_app_load_request(app_load_request)
{}

void
AitAppLoadRequestCommand::execute()
{
    m_publisher.internalNotifyAitAppLoadRequest(m_app_load_request);
}

/**
 *  AppUrlLoadRequestCommand
 */
class AppUrlLoadRequestCommand: public AnyCommand
{
public:
    AppUrlLoadRequestCommand
        (OperaHbbTVBrowserEventPublisher& publisher,
         omi::evt::HbbTVAppURLLoadRequest const& url_load_request);
    ~AppUrlLoadRequestCommand() { }

    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const omi::evt::HbbTVAppURLLoadRequest m_url_load_request;
};

AppUrlLoadRequestCommand::AppUrlLoadRequestCommand
    (OperaHbbTVBrowserEventPublisher& publisher,
     omi::evt::HbbTVAppURLLoadRequest const& url_load_request)
    : m_publisher(publisher)
    , m_url_load_request(url_load_request)
{}

void
AppUrlLoadRequestCommand::execute()
{
    m_publisher.internalNotifyAppUrlLoadRequest(m_url_load_request);
}

/**
 *  UrlChangedCommand
 */
class UrlChangedCommand: public AnyCommand
{
public:
    UrlChangedCommand(OperaHbbTVBrowserEventPublisher& publisher, cabot::String const& url);
    ~UrlChangedCommand() { }
    void execute();

private:
    OperaHbbTVBrowserEventPublisher& m_publisher;
    const cabot::String m_url;
};

UrlChangedCommand::UrlChangedCommand
    (OperaHbbTVBrowserEventPublisher& publisher, cabot::String const& url)
    : m_publisher(publisher)
    , m_url(url)
{}

void
UrlChangedCommand::execute()
{
    m_publisher.internalNotifyUrlChanged(m_url);
}

namespace
{

AitParsingStatus
map(omi::evt::AITParsingStatus::AITParsingStatusCode status)
{
    switch (status)
    {
    case omi::evt::AITParsingStatus::PARSING_COMPLETED:
        return AitParsingStatus::completed;
    case omi::evt::AITParsingStatus::PARSING_INVALIDATED:
        return AitParsingStatus::invalidated;
    case omi::evt::AITParsingStatus::PARSING_ERROR:
    default:
        return AitParsingStatus::error;
    }
}

} // namespace

OperaHbbTVBrowserEventPublisher::OperaHbbTVBrowserEventPublisher()
    :m_thread(nullptr)
    , m_listeners_mutex_initialised(false)
{
    // Initialise the mutex in the constructor to allow attach()/detach()
    // calls before the browser core has fully initialised.
    m_listeners_mutex_initialised = m_listeners_mutex.initialise();
}

OperaHbbTVBrowserEventPublisher::~OperaHbbTVBrowserEventPublisher()
{ 
}

ExplodingBoolean
OperaHbbTVBrowserEventPublisher::initialise(AnyCommandThread & thread)
{
    m_thread = &thread;
    return ExplodingBoolean(m_listeners_mutex_initialised);
}

void
OperaHbbTVBrowserEventPublisher::detach(AnyWebBrowserHbbTVEventListener* listener)
{
    TRACE_VERBOSE(("deregisterListener() called %p\n", listener));
    FrostMutexLock lock(m_listeners_mutex);
    
    // erase-remove idiom
    m_hbbtv_browser_event_listeners.erase(std::remove(m_hbbtv_browser_event_listeners.begin(), 
                                                      m_hbbtv_browser_event_listeners.end(), 
                                                      listener),
                                                      m_hbbtv_browser_event_listeners.end());
}

void
OperaHbbTVBrowserEventPublisher::attach(AnyWebBrowserHbbTVEventListener* listener)
{
    TRACE_VERBOSE(("registerListener(AnyWebBrowserHbbTVEventListener) called %p\n", listener));
    CABOT_ASSERT(listener);

    FrostMutexLock lock(m_listeners_mutex);
    if (std::find(m_hbbtv_browser_event_listeners.begin(), 
                  m_hbbtv_browser_event_listeners.end(), 
                  listener) == m_hbbtv_browser_event_listeners.end())
    {
        m_hbbtv_browser_event_listeners.push_back(listener);
    }
}

void
OperaHbbTVBrowserEventPublisher::notifyAitParsingStatus(
        omi::evt::AITParsingStatus const& ait_parsing_status)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new AitParsingStatusCommand(*this, ait_parsing_status));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVApplicationNotStartedOnURL(
    cabot::String const& url, const bool load_aborted)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new NotStartedOnUrlNotificationCommand(*this, url, load_aborted));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVApplicationNotStarted(const bool ait_parsing)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new NotStartedNotificationCommand(*this, ait_parsing));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVApplicationStarted
                            (omi::evt::HbbTVApplicationStarted const& application_started)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new StartedNotificationCommand(*this, application_started));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVApplicationStopped
                            (vewd_integ::HbbTVApplicationStopped const& application_stopped)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new StoppedNotificationCommand(*this, application_stopped));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVSuspended()
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new SuspendedNotificationCommand(*this));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVResumed()
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new ResumedNotificationCommand(*this));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVInitialised()
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new InitialisedNotificationCommand(*this));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVBroadcastRelatedChange
                        (omi::evt::HbbTVBroadcastRelatedStatusChanged const& application)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new BroadcastRelatedStatusNotificationCommand(*this, application));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVAITAppLoadRequest
    (omi::evt::HbbTVAITAppLoadRequest hbbtv_app_event)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new AitAppLoadRequestCommand(*this, hbbtv_app_event));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVAppURLLoadRequest 
    (omi::evt::HbbTVAppURLLoadRequest  app_url_load_request)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(
                new AppUrlLoadRequestCommand(*this, app_url_load_request));
    }
}

void 
OperaHbbTVBrowserEventPublisher::notifyHbbTVURLChanged(
    cabot::String const& url)
{
    if (m_thread)
    {
        m_thread->enqueueCommand(new UrlChangedCommand(*this, url));
    }
}

void
OperaHbbTVBrowserEventPublisher::internalNotifyNotStarted(const bool ait_parsing) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyAnyHbbTVApplicationsNotStarted(ait_parsing);
    }
}

void
OperaHbbTVBrowserEventPublisher::internalNotifyAitParsingStatus(
        omi::evt::AITParsingStatus const& ait_parsing_status)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyAitParsingStatus(map(ait_parsing_status.status));
    }
}

void
OperaHbbTVBrowserEventPublisher::internalNotifyNotStarted(
    cabot::String const& url,
    bool const load_aborted) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVApplicationNotStarted(url, load_aborted);
    }
}

void
OperaHbbTVBrowserEventPublisher::internalNotifyStarted
                                (omi::evt::HbbTVApplicationStarted const& application) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVApplicationStarted(application.org_id,
                                                application.app_id,
                                                cabot::String(application.app_url.c_str()),
                                                application.broadcast_related);
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyStopped
                                (vewd_integ::HbbTVApplicationStopped const& application) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVApplicationTerminated(application.org_id,
                                                   application.app_id,
                                                   cabot::String(application.app_url.c_str()),
                                                   application.broadcast_related);
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyResumed() const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVResumed();
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifySuspended() const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVSuspended();
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyInitialised() const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVInitialised();
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyBroadcastStatus
                    (omi::evt::HbbTVBroadcastRelatedStatusChanged const& application) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener * listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVApplicationChanged(application.org_id,
                                                application.app_id,
                                                cabot::String(application.app_url.c_str()),
                                                application.broadcast_related);
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyAitAppLoadRequest(
    omi::evt::HbbTVAITAppLoadRequest const& app_load_request)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener* listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyHbbTVAITAppLoadRequest(app_load_request.org_id,
                                               app_load_request.app_id,
                                               cabot::String(app_load_request.app_url.c_str()),
                                               app_load_request.broadcast_related);
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyAppUrlLoadRequest(
    omi::evt::HbbTVAppURLLoadRequest const& url_load_request)
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener* listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyApplicationCreateApplication(cabot::String(url_load_request.app_url.c_str()));
    }
}

void 
OperaHbbTVBrowserEventPublisher::internalNotifyUrlChanged(
    cabot::String const& url) const
{
    FrostMutexLock lock(m_listeners_mutex);
    for (AnyWebBrowserHbbTVEventListener* listener: m_hbbtv_browser_event_listeners)
    {
        listener->notifyUrlChanged(url.c_str());
    }
}

// EOF 
