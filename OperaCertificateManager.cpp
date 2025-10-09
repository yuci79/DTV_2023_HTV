/*
 * Copyright 2015 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of OperaCertificateManager
 */
#include "OperaCertificateManager.hpp"
#include "OperaWindow.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/certificatemanager");

OperaCertificateManager::OperaCertificateManager(OperaEngine& opera_engine)
    : m_opera_engine(opera_engine)
    , m_event_publisher(nullptr)
    , m_window_manager(nullptr)
{
}

OperaCertificateManager::~OperaCertificateManager()
{
}

ExplodingBoolean
OperaCertificateManager::initialise()
{
    TRACE_INFO(("Initialise..\n"));
    m_opera_engine.addEventHandler(&OperaCertificateManager::onCertificateError, this);
    m_opera_engine.addEventHandler(&OperaCertificateManager::onCertificateSelect, this);
    m_window_manager = m_opera_engine.getWindowManager();
    return ExplodingBoolean(m_mutex.initialise());
}

void
OperaCertificateManager::setBrowserEventPublisher(BrowserEventPublisher* event_publisher)
{
    m_event_publisher = event_publisher;
}

void
OperaCertificateManager::onCertificateError(omi::evt::CertificateError certificate_error)
{
    CABOT_ASSERT(m_window_manager);
    CABOT_ASSERT(m_event_publisher);

    bool is_ui_window = false;
    cabot::String app_url = (certificate_error.ssl_info.request_url).c_str();

    TRACE_INFO(("Certificate error type = %d url = %s \n", certificate_error.ssl_info.certificate_error, app_url.charArray()));
    if (m_window_manager)
    {
        OperaWindow* window = m_window_manager->getWindowByHandle(certificate_error.window_id);
        if (window)
        {
            if (window->type() == OperaWindow::WindowType::window_ui)
            {
                is_ui_window = true;
            }
        }
    }

    if ((app_url.find("bbc.co.uk") != cabot::String::npos) ||
        (app_url.find("bbctvapps.co.uk") != cabot::String::npos) ||
        (app_url.find("catal") != cabot::String::npos) ||
        (app_url.find("portaltv.tv") != cabot::String::npos) ||
        is_ui_window)
    {
        certificate_error.Allow();
    }
    else
    {
        omi::evt::CertificateError* cert_error = new omi::evt::CertificateError(certificate_error);
        ErrorId error_id = nextId();
        if (cert_error)
        {
            FrostMutexLock lock(m_mutex);
            m_certificate_error_list.push_back(std::make_pair(error_id, cert_error));
        }

        if (m_event_publisher)
        {
            app_url.truncate(40);
            app_url.append("...");
            m_event_publisher->notifyCertificateError(certificate_error.ssl_info.certificate_error,
                                                      app_url,
                                                      static_cast<unsigned int>(error_id));
        }
    }
}

void 
OperaCertificateManager::onCertificateSelect(omi::evt::ClientCertificateSelectEvent certificate_select)
{
    int selected_certificate_index = -1;
    cabot::String app_url = (certificate_select.server).c_str();
    TRACE_INFO(("Certificate select url = %s certificate size=%zu\n", app_url.charArray(), certificate_select.certificates.size()));
    cabot::String certificate_name("");
    if (app_url.find("test.nticate.com") != cabot::String::npos)
    {
        certificate_name = "Cellnex";
    }
    else if (app_url.find("localhost:8933") != cabot::String::npos)
    {
        certificate_name = "MMSDK_Client_Cert";
    }
    if (certificate_name.length() != 0)
    {
        int certificate_index = 0;
        using omi::evt::ClientCertificateSelectEvent;
        for (std::vector<ClientCertificateSelectEvent::ClientCertificateInfo>::const_iterator it =
                certificate_select.certificates.begin();
                it != certificate_select.certificates.end(); ++it)
        {
            TRACE_VERBOSE(("Subject: %s\n", (*it).subject.c_str()));
            TRACE_VERBOSE(("\tissuer: %s\n", (*it).issuer.c_str()));
            TRACE_VERBOSE(("\tserial_number: %s\n", (*it).serial_number.c_str()));
            TRACE_VERBOSE(("\tstart_time: %f\n", (*it).start_time));
            TRACE_VERBOSE(("\texpiry_time: %f\n", (*it).expiry_time));
            if(strstr((*it).subject.c_str(),certificate_name.c_str()) !=0)
            {
                selected_certificate_index = certificate_index;
                break;
            }
            certificate_index += 1;
        }
    }
    if (selected_certificate_index != -1)
    {
        certificate_select.Select(selected_certificate_index);
    }
}

VectorOfCertificateError::iterator
OperaCertificateManager::getCertErrorIterator(unsigned int error_id)
{
    TRACE_INFO(("OperaCertificateManager::getCertErrorById() called id = %d\n", error_id));

    VectorOfCertificateError::iterator it = m_certificate_error_list.begin();

    for (; it != m_certificate_error_list.end(); ++it)
    {
        if (static_cast<ErrorId>(error_id) == it->first)
        {
            break;
        }
    }

    return it;
}

void
OperaCertificateManager::handleAppCertificateError(bool allow, unsigned int error_id)
{
    FrostMutexLock lock(m_mutex);
    VectorOfCertificateError::iterator cert_error_it = getCertErrorIterator(error_id);
    if (cert_error_it != m_certificate_error_list.end())
    {
        omi::evt::CertificateError* certificate = cert_error_it->second;

        CABOT_ASSERT(certificate);
        if (allow)
        {
            TRACE_INFO(("Certificate error allowed..\n"));
            certificate->Allow();
        }
        else
        {
            TRACE_INFO(("Certificate error denied..\n"));
            certificate->Deny();
        }
        m_certificate_error_list.erase(cert_error_it);
        delete certificate;
    }
}

ErrorId
OperaCertificateManager::nextId()
{
    FrostMutexLock lock(m_mutex);

    static unsigned int id = 0;
    return static_cast<ErrorId>(id++);
}
