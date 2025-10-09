/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Publisher for browser events
 */

#include "BrowserEventPublisher.hpp"

#include "nebula/browser_interface/AnyWebBrowserEventListener.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_public/AnyCommand.hpp"
#include "utilities_public/DoSyncCommand.hpp"


TRACE_IMPLEMENT("opera/browser_listener");

class AppCreatedNotificationCommand: public AnyCommand
{
public:
    AppCreatedNotificationCommand(BrowserEventPublisher& publisher,
                                  cabot::String const& app_url,
                                  WebBrowserAppType app_type,
                                  bool broadcast_related,
                                  bool linear_ip_service);
    ~AppCreatedNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    cabot::String const    m_app_url;
    WebBrowserAppType      m_app_type;
    bool                   m_broadcast_related;
    bool                   m_linear_ip_service;
};

AppCreatedNotificationCommand::AppCreatedNotificationCommand(BrowserEventPublisher& publisher,
                                                             cabot::String const& app_url,
                                                             WebBrowserAppType app_type,
                                                             bool broadcast_related,
                                                             bool linear_ip_service)
    : m_publisher(publisher)
    , m_app_url(app_url)
    , m_app_type(app_type)
    , m_broadcast_related(broadcast_related)
    , m_linear_ip_service(linear_ip_service)
{ }

void
AppCreatedNotificationCommand::execute()
{
    m_publisher.internalNotifyAppCreated(m_app_url, m_app_type, m_broadcast_related, m_linear_ip_service);
}


class AppTerminatedNotificationCommand: public AnyCommand
{
public:
    AppTerminatedNotificationCommand(BrowserEventPublisher& publisher,
                                     bool app_creation_error,
                                     WebBrowserAppType app_type,
                                     bool broadcast_related,
                                     bool bind_to_current_channel,
                                     cabot::String const& url);
    ~AppTerminatedNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    WebBrowserAppType      m_app_type;
    bool                   m_bind_to_current_channel;
    bool                   m_app_creation_error;
    bool                   m_broadcast_related;
    cabot::String          m_url;
};

AppTerminatedNotificationCommand::AppTerminatedNotificationCommand(
        BrowserEventPublisher& publisher, bool app_creation_error,
        WebBrowserAppType app_type,
        bool broadcast_related,
        bool bind_to_current_channel,
        cabot::String const& url)
    : m_publisher(publisher)
    , m_app_type(app_type)
    , m_app_creation_error(app_creation_error)
    , m_broadcast_related(broadcast_related)
    , m_bind_to_current_channel(bind_to_current_channel)
    , m_url(url)
{ }

void
AppTerminatedNotificationCommand::execute()
{
    m_publisher.internalNotifyAppTerminated(m_url, m_app_creation_error, m_app_type,
            m_broadcast_related, m_bind_to_current_channel);
}

class AppDocDoneNotificationCommand: public AnyCommand
{
public:
    AppDocDoneNotificationCommand(BrowserEventPublisher& publisher, 
                                  cabot::String const& url,
                                  int const http_status_code);
    ~AppDocDoneNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    cabot::String const m_url;
    int const m_http_status_code;
};

AppDocDoneNotificationCommand::AppDocDoneNotificationCommand(
        BrowserEventPublisher& publisher, cabot::String const& url, int const http_status_code)
    : m_publisher(publisher)
    , m_url(url)
    , m_http_status_code(http_status_code)
{ }

void
AppDocDoneNotificationCommand::execute()
{
    m_publisher.internalNotifyAppDocDone(m_url, m_http_status_code);
}

class AppHostChangedNotificationCommand: public AnyCommand
{
public:
    AppHostChangedNotificationCommand(BrowserEventPublisher& publisher, cabot::String const& url);
    ~AppHostChangedNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    cabot::String const    m_url;
};

AppHostChangedNotificationCommand::AppHostChangedNotificationCommand(
        BrowserEventPublisher& publisher, cabot::String const& url)
    : m_publisher(publisher)
    , m_url(url)
{ }

void
AppHostChangedNotificationCommand::execute()
{
    m_publisher.internalNotifyAppHostChanged(m_url);
}


class AppContentErrorNotificationCommand: public AnyCommand
{
public:
    AppContentErrorNotificationCommand(BrowserEventPublisher& publisher,
                                            int error_code,
                                            cabot::String const& url,
                                            WebBrowserAppType app_type,
                                            bool is_memory_low,
                                            bool is_video_memory_low);
    ~AppContentErrorNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    int                    m_error_code;
    const cabot::String    m_url;
    bool                   m_is_memory_low;
    bool                   m_is_video_memory_low;
    WebBrowserAppType      m_app_type;
};

AppContentErrorNotificationCommand::AppContentErrorNotificationCommand(
        BrowserEventPublisher& publisher,
        int error_code,
        cabot::String const & url,
        WebBrowserAppType app_type,
        bool is_memory_low,
        bool is_video_memory_low)
    : m_publisher(publisher)
    , m_error_code(error_code)
    , m_url(url)
    , m_app_type(app_type)
    , m_is_memory_low(is_memory_low)
    , m_is_video_memory_low(is_video_memory_low)
{ }

void
AppContentErrorNotificationCommand::execute()
{
    m_publisher.internalNotifyAppContentError(m_error_code, m_url,
                         m_app_type, m_is_memory_low, m_is_video_memory_low);
}

class BrowserProcessStateChangeNotificationCommand : public AnyCommand
{
public:
    BrowserProcessStateChangeNotificationCommand(BrowserEventPublisher& publisher,
                                                 BrowserProcessState state);
    ~BrowserProcessStateChangeNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    BrowserProcessState m_state;
};

BrowserProcessStateChangeNotificationCommand::BrowserProcessStateChangeNotificationCommand(
        BrowserEventPublisher& publisher,
        BrowserProcessState state)
    : m_publisher(publisher),
      m_state(state)
{ }

void
BrowserProcessStateChangeNotificationCommand::execute()
{
    m_publisher.internalNotifyBrowserProcessStateChange(m_state);
}

class CertificateErrorNotificationCommand: public AnyCommand
{
public:
    CertificateErrorNotificationCommand(BrowserEventPublisher& publisher,
                                            int error_code,
                                            cabot::String app_url,
                                            unsigned int error_id);
    ~CertificateErrorNotificationCommand() { }

    void execute();

private:
    BrowserEventPublisher& m_publisher;
    int                    m_error_code;
    cabot::String          m_app_url;
    unsigned int          m_error_id;
};

CertificateErrorNotificationCommand::CertificateErrorNotificationCommand(
        BrowserEventPublisher& publisher,
        int error_code,
        cabot::String app_url,
        unsigned int error_id)
    : m_publisher(publisher)
    , m_error_code(error_code)
    , m_app_url(app_url)
    , m_error_id(error_id)
{ }

void
CertificateErrorNotificationCommand::execute()
{
    m_publisher.internalNotifyCertificateError(m_error_code, m_app_url, m_error_id);
}

BrowserEventPublisher::BrowserEventPublisher()
{
    if (!m_browser_event_listeners_mutex.initialise())
    {
        TRACE_ERROR(("Failed to initialise event publisher mutex\n"));
    }
}

BrowserEventPublisher::~BrowserEventPublisher()
{ }

ExplodingBoolean
BrowserEventPublisher::initialise()
{
    bool init_ok = false;

    if (m_commands.initialise("BrowserEventPublisherThread",
                              FROST_medium_priority, 512, 128))
    {
        init_ok = true;
    }

    return ExplodingBoolean(init_ok);
}

void
BrowserEventPublisher::attach(AnyWebBrowserEventListener* listener)
{
    TRACE_VERBOSE(("registerListener() called %p\n", listener));
    CABOT_ASSERT(listener);

    FrostMutexLock lock(m_browser_event_listeners_mutex);
    const ListenerIterator end = m_browser_event_listeners.end();

    bool listener_already_exists = false;
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        if (*ii == listener)
        {
            listener_already_exists = true;
            break;
        }
    }

    if (!listener_already_exists)
    {
        m_browser_event_listeners.push_back(listener);
    }
}

void
BrowserEventPublisher::detach(AnyWebBrowserEventListener* listener)
{
    TRACE_VERBOSE(("deregisterListener() called %p\n", listener));

    FrostMutexLock lock(m_browser_event_listeners_mutex);

    const ListenerIterator end = m_browser_event_listeners.end();
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        if (*ii == listener)
        {
            m_browser_event_listeners.erase(ii);
            break;
        }
    }
}

void
BrowserEventPublisher::notifyAppCreated(cabot::String const& app_url,
                           WebBrowserAppType app_type,
                           bool broadcast_related,
                           bool linear_ip_service)
{
    m_commands.enqueueCommand(new AppCreatedNotificationCommand(*this,
                                                     app_url,
                                                     app_type,
                                                     broadcast_related,
                                                     linear_ip_service));
}

void BrowserEventPublisher::notifyAppTerminated(cabot::String const& app_url, bool app_creation_error,
        WebBrowserAppType app_type, bool broadcast_related, bool select_current_service)
{
    m_commands.enqueueCommand(
            new AppTerminatedNotificationCommand(*this, app_creation_error,
                    app_type, broadcast_related, select_current_service, app_url));
}

void
BrowserEventPublisher::notifyAppDocDone(cabot::String const& url, int const http_status_code)
{
    m_commands.enqueueCommand(new AppDocDoneNotificationCommand(*this, url, http_status_code));
}

void
BrowserEventPublisher::notifyAppHostChanged(cabot::String const& url)
{
    m_commands.enqueueCommand(new AppHostChangedNotificationCommand(*this, url));
}

void
BrowserEventPublisher::notifyAppContentError(int error_code, 
                                             cabot::String const& url, 
                                             WebBrowserAppType app_type, 
                                             bool is_memory_low,
                                             bool is_video_memory_low)
{
    m_commands.enqueueCommand(new AppContentErrorNotificationCommand(*this, error_code,
                                    url, app_type, is_memory_low, is_video_memory_low));
}

void
BrowserEventPublisher::notifyBrowserProcessStateChange(BrowserProcessState state,
                                                       bool synchronous)
{
    auto command = new BrowserProcessStateChangeNotificationCommand(*this, state);
    if (synchronous)
    {
        doSyncCommand(m_commands, command);
    }
    else
    {
        m_commands.enqueueCommand(command);
    }
}

void
BrowserEventPublisher::notifyCertificateError(int error_code, cabot::String app_url, unsigned int error_id)
{
    m_commands.enqueueCommand(new CertificateErrorNotificationCommand(*this, error_code, app_url, error_id));
}

void BrowserEventPublisher::internalNotifyAppCreated(
    cabot::String const& app_url, WebBrowserAppType app_type, bool broadcast_related, bool linear_ip_service)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);

    const ListenerIterator end = m_browser_event_listeners.end();
    for(ListenerIterator ii = m_browser_event_listeners.begin(); ii != end;
        ++ii)
    {
        (*ii)->notifyAppCreated(app_url, broadcast_related, app_type, linear_ip_service);
    }
}

void
BrowserEventPublisher::internalNotifyAppTerminated(cabot::String const& url, bool app_creation_error,
        WebBrowserAppType app_type, bool broadcast_related, bool reselect_current_service)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);
    const ListenerIterator end = m_browser_event_listeners.end();

    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        (*ii)->notifyAppTerminated(url, app_creation_error, app_type,
                broadcast_related, reselect_current_service);
    }
}

void
BrowserEventPublisher::internalNotifyAppDocDone(cabot::String const& url, int const http_status_code)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);

    const ListenerIterator end = m_browser_event_listeners.end();
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        (*ii)->notifyAppDocDone(url, http_status_code);
    }
}

void
BrowserEventPublisher::internalNotifyAppHostChanged(cabot::String const& url)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);

    const ListenerIterator end = m_browser_event_listeners.end();
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        (*ii)->notifyAppHostChanged(url);
    }
}

void
BrowserEventPublisher::internalNotifyAppContentError(int error_code, 
                                                     cabot::String const& url, 
                                                     WebBrowserAppType app_type,
                                                     bool is_memory_low,
                                                     bool is_video_memory_low)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);

    if(error_code == -3)
    {
        error_code = nebula::LoadingFailureCode::LOADING_ERROR_ABORTED;
    }
    else if (error_code <= -100 && error_code >= -199) // connection related error codes in chromium
    {
        error_code = nebula::LoadingFailureCode::LOADING_ERROR_CONNECTION_LOSS;
    }
    else
    {
        error_code = nebula::LoadingFailureCode::LOADING_ERROR_GENERAL;
    }

    if(is_memory_low == true)
    {
        error_code = nebula::LoadingFailureCode::LOADING_ERROR_MEMORY_LOW;
    }

    if (is_video_memory_low == true)
    {
        error_code = nebula::LoadingFailureCode::LOADING_ERROR_VIDEO_MEMORY_LOW;
    }

    const ListenerIterator end = m_browser_event_listeners.end();
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        (*ii)->notifyAppContentError(error_code, url, app_type);
    }
}

void
BrowserEventPublisher::internalNotifyBrowserProcessStateChange(BrowserProcessState state)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);
    for (auto listener : m_browser_event_listeners)
    {
        listener->notifyBrowserProcessStateChange(state);
    }
}

void
BrowserEventPublisher::internalNotifyCertificateError(int error_code, cabot::String app_url, unsigned int error_id)
{
    FrostMutexLock lock(m_browser_event_listeners_mutex);

    const ListenerIterator end = m_browser_event_listeners.end();
    for (ListenerIterator ii = m_browser_event_listeners.begin(); ii != end; ++ii)
    {
        (*ii)->notifyCertificateError(error_code, app_url, error_id);
    }
}

