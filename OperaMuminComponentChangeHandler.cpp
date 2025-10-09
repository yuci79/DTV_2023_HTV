/**
 * Copyright 2017 Vestel. All rights reserved.
 *
 * @file
 * @brief This file contains the implementation of the class 
 * OperaMuminComponentChangeHandler.
 */

#include "OperaMuminComponentChangeHandler.hpp"
#include "utilities_debug/trace.h"
#include "utilities_debug/cabot_assert.h"

TRACE_IMPLEMENT("mumin/componentchange");

namespace
{
#ifdef CABOT_TRACE
    char const *toString(const ComponentChangeHandler::Action action)
    {
        char const* action_str = nullptr;
        switch (action)
        {
            case ComponentChangeHandler::action_add:
                action_str = "Adding component";
            break;
            case ComponentChangeHandler::action_remove:
                action_str = "Removing component";
            break;
            case ComponentChangeHandler::action_changed:
                action_str = "Change in component";
            break;
            default:
                action_str = "Unknown";
        }
        return action_str;
    }
#endif
}

OperaMuminComponentChangeHandler::OperaMuminComponentChangeHandler(
                                  UVAClient* uva_client) : 
                                  m_client(uva_client) 
{
}
                                               
bool OperaMuminComponentChangeHandler::handleComponentChange(
                                       AVComponent* component,
                                       Action action)
{
    bool result = false;
    if (!component)
    {
        TRACE_ERROR(("Error in %s: component is null\n", __FUNCTION__));
        return result;
    }
    if (!m_client)
    {
        TRACE_ERROR(("ComponentChangeHandler UVAClient is null\n"));
        return result;
    }
    printEventInfo(component, action);
    result = handleNewComponentChange(component, action);
    return result;
}

void OperaMuminComponentChangeHandler::printEventInfo(const AVComponent* component,
                                                      const Action action) const
{
#ifdef CABOT_TRACE
    TRACE_INFO(("%s: type = %d (%p)\n", ::toString(action), component->type, component)); 
#endif
    TRACE_VERBOSE((
    "\naspect ratio = (%f) audio_channels = (%d)\n" 
    "audio_description = (%d) bandwidth = (%d)\n"
    "encoding = (%s) encrypted = (%d)\n"
    "hearing_impaired = (%d) id = (%s)\n" 
    "kind = (%d) label = (%s)\n"
    "lang_bcp_47 = (%s) lang_iso_639_2 = (%s)\n" 
    "lang_representation_id = (%s) has_component_tag = (%d)\n" 
    "component_tag = (%d) has_pid = (%d)\n" 
    "pid = (%d)\n",
    component->aspect_ratio, component->audio_channels, 
    component->audio_description, component->bandwidth, component->encoding,
    component->encrypted, component->hearing_impaired, component->id, 
    component->kind, component->label, component->language_bcp_47, 
    component->language_iso_639_2, component->representation_id, 
    component->has_componentTag, component->componentTag, component->has_pid, 
    component->pid));
}

bool OperaMuminComponentChangeHandler::sendEvent()
{
    if (!m_components_to_add.empty() || !m_components_to_remove.empty())
    {
        TRACE_INFO(("Firing MultiComponentChangedEvent to Opera\n"
                    "Number of components to add: %zu\n"
                    "Number of components to remove: %zu\n",
                    m_components_to_add.size(), m_components_to_remove.size()));
                    
        UVAEvent* event = new MultiComponentChangedEvent(m_components_to_remove,
                                                         m_components_to_add);
        if(!event || m_client->handleEvent(event) != UVA_OK)
        {
            TRACE_ERROR(("Failed to fire MultiComponentChangedEvent\n"));
            return false;
        }
        m_components_to_add.clear();
        m_components_to_remove.clear();
    }
    return true;
}

bool OperaMuminComponentChangeHandler::handleNewComponentChange(
                                                  AVComponent* component,
                                                  const Action action)
{
    bool result = true;
    switch (action)
    {
        case action_add:
            m_components_to_add.push_back(component);
        break;
        case action_remove:
            m_components_to_remove.push_back(component);
        break;
        case action_changed:
        {
            UVAEvent* event = nullptr;
            event = new ComponentChangedEvent(component);
            result = (event && m_client->handleEvent(event) == UVA_OK);
        }
        break;
        default:
            result = false;
            SHOULD_NOT_BE_HERE();
    }
    return result;
}
