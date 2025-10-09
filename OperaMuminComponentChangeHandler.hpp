/**
 * Copyright 2017 Vestel. All rights reserved.
 *
 * @file
 * @brief This file defines the class OperaMuminComponentChangeHandler.
 */
 
#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCOMPONENTCHANGEHANDLER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCOMPONENTCHANGEHANDLER_HPP_INCLUDED

#include "utilities_public/UString.hpp"
#include "uva_client.h"
#include "uva_event.h"

typedef UVABackend::AVComponent AVComponent;

/** 
 * @brief This class is intended to be a wrapper for the process of sending
 * a component change event to Opera. Depending on the Opera SDK version, the
 * interface provided by Opera for sending this event can differ.
 */ 

class OperaMuminComponentChangeHandler
{
public:
    /**
     * @brief Possible component change events.
     */
    enum Action
    {
        action_add,
        action_remove,
        action_changed
    };
    /**
     * @brief Constructor
     *
     * @param uva_client The UVAClient from Opera to handle sending Opera events.
     */
    OperaMuminComponentChangeHandler(UVAClient* uva_client);
    
    /**
     * @brief Determines appropriate component changed event to update the Opera 
     * component based on AVComponent and Action provided. If using SDK 4.9 or
     * later, please include the use of sendEvent() below to ensure the component
     * changed event is sent.
     *
     * @param component Pointer to AVComponent involved in component change
     * @param action Component action that has occurred.
     * @return true if Opera component change handled successfully, otherwise
     * false.
     */
    bool handleComponentChange(AVComponent* component, Action action);
    
    /**
     * @brief sendEvent() is only required if using SDK 4.9 or later.
     *
     * Use sendEvent() to control when to send the event. In some cases you may 
     * not need to send a component changed event immediately. For example, if 
     * multiple components have changed, a component changed event should only be
     * sent once add and remove operations have completed.
     *
     * @return true if component change event successfully sent to and handled
     * by Opera, else false.
     */
    bool sendEvent();
    
private:
    void printEventInfo(const AVComponent* component, const Action action) const;
    bool handleNewComponentChange(AVComponent* component, const Action action);
    ComponentChangedEvent::Action convertToOperaAction(const Action action) const;
    
private:
    std::vector<AVComponent*> m_components_to_add;
    std::vector<AVComponent*> m_components_to_remove;
    UVAClient* m_client;
};

typedef OperaMuminComponentChangeHandler ComponentChangeHandler;

#endif // CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINCOMPONENTCHANGEHANDLER_HPP_INCLUDED
