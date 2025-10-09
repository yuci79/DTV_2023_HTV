/*
 * Copyright 2015 Vestel.  All rights reserved.
 *
 * @file
 * @brief OperaCertificateManager class
 */
#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERACERTIFICATEMANAGER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERACERTIFICATEMANAGER_HPP_INCLUDED

#include "OperaEngine.hpp"
#include "OperaWindowManager.hpp"
#include <omi/events/CertificateError.h>
#include <omi/events/ClientCertificateSelectEvent.h>
#include "utilities_public/String.hpp"
#include <vector>
#include "utilities_public/FrostMutex.hpp"

namespace omi
{
namespace evt
{
    class CertificateError;
}
}

typedef unsigned int ErrorId;
typedef std::pair<ErrorId, omi::evt::CertificateError*> IdErrorPair;
typedef std::vector<IdErrorPair> VectorOfCertificateError;

class OperaCertificateManager
{
public:
    OperaCertificateManager(OperaEngine& opera_engine);
    ~OperaCertificateManager();

    ExplodingBoolean initialise();

    /**
     * @brief An event handler for certificate errors
     * @param certificate_error The data for this event
     */
    void onCertificateError(omi::evt::CertificateError certificate_error);

    /**
     * @brief An event handler for certificate select event
     * @param certificate_select The data for this event
     */
    void onCertificateSelect(omi::evt::ClientCertificateSelectEvent certificate_select);

    /**
     * @brief Set the browser event publisher
     * @param event_publisher The event publisher instance to set
     */
    void setBrowserEventPublisher(BrowserEventPublisher* event_publisher);

    /**
     * @brief Handle certificate error
     *
     * @param allow True if certificate error will be allowed false otherwise.
     * @param error_id Id of certificate error
     */
    void handleAppCertificateError(bool allow, unsigned int error_id);

private: // Copy and assign not allowed.
    OperaCertificateManager(OperaCertificateManager const&);
    OperaCertificateManager& operator=(OperaCertificateManager const&);

    ErrorId nextId();

    /**
     * @brief Get iterator of certificate error which belongs to given id
     * @param error_id Id of certificate error
     * 
     * @return Iterator of certificate error in list
     */
    VectorOfCertificateError::iterator getCertErrorIterator(unsigned int error_id);

    OperaEngine& m_opera_engine;
    BrowserEventPublisher* m_event_publisher;
    OperaWindowManager const* m_window_manager;
    VectorOfCertificateError m_certificate_error_list;
    FrostMutex m_mutex;
};

#endif
