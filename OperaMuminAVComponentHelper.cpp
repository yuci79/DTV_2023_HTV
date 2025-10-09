/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMuminAVComponentHelper
 */

#include "OperaMuminAVComponentHelper.hpp"
#include "OperaMuminAVComponent.hpp"
#include "OperaMuminComponentChangeHandler.hpp"
#include "uva_client.h"
#include "uva_event.h"
#include "language/LanguageCode.hpp"

#include <stdio.h>
#include <cstring>
#include <algorithm>

#include "frost/frost_standards.h"

#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"
TRACE_IMPLEMENT("mumin/component/helper");

namespace
{
extern "C" void
fillComponentListCallback(NEBULA_AVComponentList* component_list,
                          OIPF_ComponentType type,
                          void* user_data)
{
    OperaMuminAVComponentHelper* helper = static_cast<OperaMuminAVComponentHelper*>(user_data);
    if(helper)
    {
        helper->fillComponentList(component_list, type);
    }
}

extern "C" void
activateSelectedComponentCallback(NEBULA_AVComponent* component, void* user_data)
{
    OperaMuminAVComponentHelper* helper = static_cast<OperaMuminAVComponentHelper*>(user_data);
    if(helper)
    {
        helper->activateSelectedComponent(component);
    }
}

extern "C" void
activateSelectedComponentOnIdCallback(OIPF_ComponentType type,
                                      const char* id,
                                      const char* lang,
                                      frost_bool activate,
                                      const char* label,
                                      void* user_data)
{
    OperaMuminAVComponentHelper* helper = static_cast<OperaMuminAVComponentHelper*>(user_data);
    if(helper)
    {
        bool to_activate = (activate == frost_true);
        helper->activateSelectedComponent(type,id,lang,to_activate,label);
    }
}

extern "C" void
audioComponentCallback(const char* language, bool ad_on, void* user_data)
{
    TRACE_INFO(("In %s(): need to update audio component\n", __FUNCTION__));
    OperaMuminAVComponentHelper* obj = static_cast<OperaMuminAVComponentHelper*>(user_data);

    if (obj)
    {
        UString lang_code(language);
        obj->selectProperAudioComponent(lang_code, ad_on);
    }
}
}

NEBULA_Kind convertToNebulaKind(UVABackend::AVComponent::Kind to_convert)
{
    NEBULA_Kind value_to_return = NEBULA_KindNone;
    switch (to_convert)
    {
        case UVABackend::AVComponent::ALTERNATIVE:
        value_to_return = NEBULA_KindAlternative;
        break;

        case UVABackend::AVComponent::CAPTIONS:
        value_to_return = NEBULA_KindCaptions;
        break;

        case UVABackend::AVComponent::CHAPTERS:
        value_to_return = NEBULA_KindChapter;
        break;

        case UVABackend::AVComponent::COMMENTARY:
        value_to_return = NEBULA_KindCommentary;
        break;

        case UVABackend::AVComponent::DESCRIPTION:
        value_to_return = NEBULA_KindDescription;
        break;

        case UVABackend::AVComponent::MAIN:
        value_to_return = NEBULA_KindMain;
        break;

        case UVABackend::AVComponent::MAIN_DESC:
        value_to_return = NEBULA_KindMainDesc;
        break;

        case UVABackend::AVComponent::METADATA:
        value_to_return = NEBULA_KindMetadata;
        break;

        case UVABackend::AVComponent::NONE:
        break;

        case UVABackend::AVComponent::SUBTITLES:
        value_to_return = NEBULA_KindSubtitles;
        break;

        case UVABackend::AVComponent::TRANSLATION:
        value_to_return = NEBULA_KindTranslation;
        break;
    }
    return value_to_return;
}

#define ACTIVE_INSTANCE_VALUE 0xac715e

OperaMuminAVComponentHelper::OperaMuminAVComponentHelper(
        AnyAvComponentController& component_controller, 
        UVAClient* client,
        bool using_external_components,
        AnyCommandThread* media_queue)
    : m_component_store(component_controller)
    , m_component_controller(component_controller)
    , m_client(client)
    , m_using_external_components(using_external_components)
    , m_is_instance_active(ACTIVE_INSTANCE_VALUE)
    , m_queue(media_queue)
{
    if (!m_component_mutex.initialise(true))    // Note: This module's current designs relies on this mutex being recursive
    {
        TRACE_ERROR(("OperaMuminAVComponentHelper: Cannot create mutex \n"));
    }
}

OperaMuminAVComponentHelper::~OperaMuminAVComponentHelper()
{
    {
        FrostMutexLock lock(m_component_mutex);
        m_is_instance_active = 0;
        removeComponentsLocked();
    }

    deregisterComponentCallbacks();
}

bool
OperaMuminAVComponentHelper::updateComponents(bool select_initials)
{
    TRACE_INFO(("Updating components - select_initials=%d\n", 
                 static_cast<int>(select_initials)));
    if (!m_is_instance_active)
    {
        return false;
    }
    
    FrostMutexLock lock(m_component_mutex);

    if (!m_is_instance_active)
    {
        return false;
    }
    if (m_using_external_components)
    {
        return true;
    }

    int num_video_components = m_component_store.getNumComponents(OIPF_ComponentTypeVideo);
    int num_audio_components = m_component_store.getNumComponents(OIPF_ComponentTypeAudio);
    int num_subtitle_components = m_component_store.getNumComponents(OIPF_ComponentTypeSubtitle);

    int num_components = num_video_components + num_audio_components + num_subtitle_components;
    TRACE_INFO(("updateComponents: num_components: %d, video: %d, audio: %d, subtitle: %d\n",
            num_components, num_video_components, num_audio_components, num_subtitle_components));

    int initial_video_component = -1;
    int initial_audio_component = -1;
    
    if (num_components == 0)
    {
        TRACE_INFO(("%s(%d) No components found\n", __FUNCTION__, __LINE__));
    }

    VectorOfAVComponentPtr previous_components(m_component_list);

    //Remove all components apart from those that are hidden (i.e. not known by broadcast).
    m_component_list.erase(
        std::remove_if(m_component_list.begin(), 
                       m_component_list.end(),
                       [](VectorOfAVComponentPtr::value_type component)
                       {
                           return static_cast<OperaMuminAVComponent*>(component)->isKnownByOpera();
                       }),
        m_component_list.end());

    ComponentChangeHandler comp_handler(m_client);
    bool subtitle_found = false;

    if(select_initials)
    {
        initial_video_component = 0;
        initial_audio_component = 0;
        
        NEBULA_LanguageCode lang_code;
        memset(lang_code.iso_str, '\0', NEBULA_LANG_CODE_LEN);
        NEBULA_LanguageSettingGetAudioLanguage(&lang_code);
        
        UString language(lang_code.iso_str);
        
        bool is_audio_description_enabled = NEBULA_IsAudioDescriptionEnabled();
        
        UString component_id_internal;
        for (int i = 0; i < num_audio_components; ++i)
        {
            bool component_found = m_component_store.getComponentIDByIndex(OIPF_ComponentTypeAudio, i, component_id_internal);
            if (component_found)
            {
                NEBULA_AVComponent* component_data = new NEBULA_AVComponent;

                if (!component_data)
                {
                    TRACE_ERROR(("OperaMuminComponentHelper::getComponents(): cannot create comp data\n"));
                    return onFinishedUpdateComponents(false);
                }

                bool component_info_found = m_component_store.getComponentInfoByID(component_id_internal, *component_data);
                if (!component_info_found)
                {
                    delete component_data;
                    break;
                }
                if(language.compare(component_data->language_code) == 0)
                {
                    if(component_data->audio_description == frost_true)
                    {
                        initial_audio_component = i;
                        delete component_data;
                        break;
                    }
                    else
                    {
                        initial_audio_component = i;
                    }
                }
                delete component_data;
            }
        }
    }
    for (int j = 0; j < 3; ++j)
    {
        OIPF_ComponentType current_component_type = OIPF_ComponentTypeVideo;
        int num_current_component = 0;
        if (j == 0)
        {
            current_component_type = OIPF_ComponentTypeVideo;
            num_current_component = num_video_components;
        }
        else if (j == 1)
        {
            current_component_type = OIPF_ComponentTypeAudio;
            num_current_component = num_audio_components;
        }
        else if (j == 2)
        {
            current_component_type = OIPF_ComponentTypeSubtitle;
            num_current_component = num_subtitle_components;
        }

        UVABackend::AVComponentType mumin_component_type = convertNebulaComponentTypeToMumin(current_component_type);

        UString component_id_internal;
        UString component_id;
        bool component_found = false;
        OperaMuminAVComponent* comp = 0;
        frost_uint main_audio_component_tag = 0;
        frost_int main_audio_component_pid = 0;

        for (int i = 0; i < num_current_component; ++i)
        {
            component_found = m_component_store.getComponentIDByIndex(current_component_type, i, component_id_internal);
            bool is_active_component = false;
            if (select_initials)
            {
                if (current_component_type == OIPF_ComponentTypeVideo && i == initial_video_component)
                {
                    is_active_component = true;
                }
                if (current_component_type == OIPF_ComponentTypeAudio && i == initial_audio_component)
                {
                    is_active_component = true;
                }
            }
            comp = 0;
            if (component_found)
            {
                NEBULA_AVComponent* component_data = new NEBULA_AVComponent;

                if (!component_data)
                {
                    TRACE_ERROR(("OperaMuminComponentHelper::getComponents(): cannot create comp data\n"));
                    return onFinishedUpdateComponents(false);
                }

                bool component_info_found = m_component_store.getComponentInfoByID(component_id_internal, *component_data);
                if (!component_info_found)
                {
                    delete component_data;
                    return false;
                }

                if (current_component_type == OIPF_ComponentTypeSubtitle)
                {
                    subtitle_found = true;
                }

                double aspect_ratio = convertAspectRatio(component_data->aspect_ratio);
                UString encoding = getEncoding(component_data);
                UString language_iso_639_2 = getLanguage(current_component_type,
                                                         component_data);

                /* Pass ISO 639-1 language code truly into UVABackend::AVComponent
                 * as it was asked for in SIERRA-328.
                 * Because bcp 47 language tag is supposed to use only the ISO 639-1 
                 * two-character code if both ISO 639-1 and ISO 639-2 are available.
                 * https://tools.ietf.org/html/bcp47
                 */
                char language_bcp_47[4] = "";
                LanguageCode(component_data->language_code).findISO639Dash1Str(
                                                                      language_bcp_47);

                bool encrypted = component_data->encrypted;
                bool audio_desc = (component_data->audio_description == frost_true);
                int audio_channels = getAudioChannels(component_data->encoding);
                bool hearing_impaired = (component_data->hearing_impaired == frost_true);
                bool receiver_mixed_ad = (component_data->receiver_mixed_ad == frost_true);
                is_active_component = (component_data->is_active == frost_true);
                component_id = "";
                if (component_data->stream_id > -1)
                {
                    component_id.append(UString::fromInt(component_data->stream_id));
                }
                if (select_initials && !is_active_component)
                {
                    if (current_component_type == OIPF_ComponentTypeVideo && i == initial_video_component)
                    {
                        is_active_component = true;
                    }
                    if (current_component_type == OIPF_ComponentTypeAudio && i == initial_audio_component)
                    {
                        is_active_component = true;
                    }
                }

                bool has_component_tag = (component_data->component_tag != NEBULA_INVALID_COMPONENT_TAG);
                
                // SUNSTONE-1360:[BBC][MHEG Transition] TC-19: Incorrect Component Tag reported for AD component
                // Reference: OIPF Volume 5 DAE: Section 8.4.5: Exposing Audio Description streams as AVComponent objects
                // Audio Description AVComponents SHALL have the same component tag and pid as the main audio component.

                // KYLO-10367: Further update was required since component tags are optional.
                // See ETSI EN 300 468 1.16.1 J.2.2 PSI PMT signalling which states:
                // "A stream_identifier descriptor should be present."
                if (current_component_type == OIPF_ComponentTypeAudio)
                {
                    if (!audio_desc && main_audio_component_pid == 0)
                    {
                        if (has_component_tag)
                        {
                            main_audio_component_tag = component_data->component_tag;
                        }
                        main_audio_component_pid = component_data->pid;
                        TRACE_INFO(("Setting main audio component tag to %d (PID %d) has_component_tag %d\n", main_audio_component_tag,
                                                                                                            main_audio_component_pid,
                                                                                                            has_component_tag));
                    }
                    else if (audio_desc && main_audio_component_pid != 0 && receiver_mixed_ad)
                    {
                        TRACE_INFO(("Overriding AD CT %d (PID %d) with %d (PID %d) has_component_tag %d\n", component_data->component_tag,
                                                                                                            component_data->pid,
                                                                                                            main_audio_component_tag,
                                                                                                            main_audio_component_pid,
                                                                                                            has_component_tag));
                        if (main_audio_component_tag != 0)
                        {
                            component_data->component_tag = main_audio_component_tag;
                        }
                        component_data->pid = main_audio_component_pid;
                        main_audio_component_tag = 0;
                        main_audio_component_pid = 0;
                    }
                }

                TRACE_ALWAYS(("updateComponents() type: %d, aspect_ratio : %f, "
                            "encoding: %s, language_bcp_47: %s, "
                            "language_iso_639_2: %s active: %d, id: %s\n",
                             current_component_type, aspect_ratio,
                             encoding.charArray(), language_bcp_47,
                             language_iso_639_2.charArray(), is_active_component,
                             component_id.charArray()));

                comp = new OperaMuminAVComponent(mumin_component_type,
                                                        encoding.charArray(),
                                                        encrypted,
                                                        (mumin_component_type == UVABackend::COMPONENT_TYPE_VIDEO) ? aspect_ratio : -1,
                                                        UString(language_bcp_47).charArray(),
                                                        language_iso_639_2.charArray(),
                                                        audio_desc,
                                                        audio_channels,
                                                        hearing_impaired,
                                                        is_active_component,
                                                        component_data,
                                                        true,
                                                        component_data->pid,
                                                        has_component_tag,
                                                        component_data->component_tag,
                                                        component_id.charArray(),
                                                        component_id_internal.charArray());
            }
            else
            {
                UString encoding = "UNKNOWN";
                UString language = "und";
                //put some dummy data.
                comp = new OperaMuminAVComponent(mumin_component_type,
                                                        encoding.charArray(),
                                                        false,
                                                        16.0 / 9.0,
                                                        language.charArray(),
                                                        language.charArray(),
                                                        false,
                                                        2,
                                                        false,
                                                        is_active_component,
                                                        NULL,
                                                        false,
                                                        0,
                                                        false,
                                                        0,
                                                        component_id.charArray(),
                                                        component_id_internal.charArray());
            }

            if (!comp)
            {
                TRACE_ERROR(("%s(): Failed to allocate AVComponent\n", __FUNCTION__));
                continue;
            }
            int index;
            int index2;
            if(!isComponentInList(comp, previous_components, &index))
            {
                m_component_list.push_back(comp);
                if (comp_handler.handleComponentChange(comp, ComponentChangeHandler::action_add))
                {
                    TRACE_INFO(("Component added successfully.\n"));
                    ((OperaMuminAVComponent*)comp)->setKnownByOpera(true);
                    ((OperaMuminAVComponent*)comp)->setActive(is_active_component);
                }
            }
            else
            {
                OperaMuminAVComponent* previous_component = 
                    static_cast<OperaMuminAVComponent*>(previous_components[index]);
                const bool component_active = comp->isActive();
                const bool previous_component_active = previous_component->isActive();
                if (component_active != previous_component_active)
                {
                    previous_component->setActive(component_active);
                    //Only notify when a component becomes active.
                    if (component_active && comp_handler.handleComponentChange(previous_component,
                                                           ComponentChangeHandler::action_changed))
                    {
                        TRACE_INFO(("Component activeness changed successfully.\n"));
                    }
                }

                if(isComponentInList(previous_components[index], m_component_list, &index2))
                {
                    TRACE_INFO(("Component is already in list\n"));
                }
                else
                {
                    m_component_list.push_back(previous_components[index]);
                }

                delete comp;
            }
            if(select_initials && is_active_component)
            {
                selectComponentLocked(comp);
            }                
        }
    }

    TRACE_VERBOSE(("updateComponents - remove previous components\n"));

    for(VectorOfAVComponentPtr::iterator prev_it =
            previous_components.begin(); prev_it != previous_components.end();
            ++prev_it)
    {
        /* According to the comments under SIERRA-270, language codes of any component
         * in m_component_list might possibly be changed in case the values are incorrect.
         * In that case, component would be removed twice and that would bring about crash
         * problem. To solve that, address comparison is applied rather than comparing 
         * instance variables.
         */
        auto iterator = std::find_if(m_component_list.begin(),
                                     m_component_list.end(),
                                     [&prev_it](AVComponentPtr& av_component_ptr)
                                     {
                                         return av_component_ptr == *prev_it;
                                     });
        if (iterator == m_component_list.end())
        {
            TRACE_VERBOSE(("updateComponents - action_remove %p\n", *prev_it));

            if (comp_handler.handleComponentChange(*prev_it, ComponentChangeHandler::action_remove))
            {
                TRACE_INFO(("Component removed successfully.\n"));
            }
        }
    }

    bool status = comp_handler.sendEvent();
    return onFinishedUpdateComponents(status);
}

void
OperaMuminAVComponentHelper::registerComponentCallbacks()
{
    TRACE_INFO(("registerComponentCallbacks()\n"));

    m_component_store.registerAudioComponentCallback(
                                             audioComponentCallback,
                                             static_cast<void*>(this));
    m_component_store.registerFillComponentListCallback(
                                             fillComponentListCallback,
                                             static_cast<void*>(this));
    m_component_store.registerActivateSelectedComponentCallback(
                                              activateSelectedComponentCallback,
                                              static_cast<void*>(this));
    m_component_store.registerActivateSelectedComponentOnIdCallback(
                                              activateSelectedComponentOnIdCallback,
                                              static_cast<void*>(this));
}

void
OperaMuminAVComponentHelper::deregisterComponentCallbacks()
{
    TRACE_INFO(("deregisterComponentCallbacks()\n"));

    m_component_store.deregisterAudioComponentCallback(static_cast<void*>(this));
    m_component_store.deregisterFillComponentListCallback(static_cast<void*>(this));
    m_component_store.deregisterActivateSelectedComponentCallback(static_cast<void*>(this));
    m_component_store.deregisterActivateSelectedComponentOnIdCallback(static_cast<void*>(this));
}

bool
OperaMuminAVComponentHelper::onFinishedUpdateComponents(bool result)
{
    m_component_store.notifyComponentsUpdated();
    return result;
}

void
OperaMuminAVComponentHelper::removeComponentsLocked()
{
    if (m_using_external_components)
    {
        return;
    }

    CABOT_ASSERT(m_client);
    ComponentChangeHandler comp_handler(m_client);
    TRACE_INFO(("removeComponents: SIZE: %zu \n", m_component_list.size()));
    for (VectorOfAVComponentPtr::iterator it = m_component_list.begin();
            it != m_component_list.end(); ++it)
    {
        OperaMuminAVComponent* comp = static_cast<OperaMuminAVComponent*>(*it);
        comp->setActive(false);
        if (comp_handler.handleComponentChange(comp, ComponentChangeHandler::action_remove))
        {
            TRACE_INFO(("Component removed successfully.\n"));
        }
        else
        {
            if (comp->isKnownByOpera())
            {
                comp->markUnusedInBackend();
            }
        }
    }
    comp_handler.sendEvent();
    m_component_list.clear();
}

bool
OperaMuminAVComponentHelper::selectComponent(UVABackend::AVComponent* component)
{   
    FrostMutexLock lock(m_component_mutex);

    const bool result = selectComponentLocked(component);
    TRACE_INFO(("selectComponent - %p, result=%d\n", component, static_cast<int>(result)));

    return result;
}

bool
OperaMuminAVComponentHelper::selectComponentLocked(UVABackend::AVComponent* component)
{   
    if (component)
    {
        TRACE_INFO(("Select component with type: %d\n", component->type));

        VectorOfAVComponentPtr::iterator comp_it = findComponentLocked(component);
        if (comp_it != m_component_list.end())
        {
            TRACE_VERBOSE(("selectComponent - found\n"));

            if (m_using_external_components)
            {
                return selectExternalAVComponentLocked(*comp_it);
            }
            else
            {
                OperaMuminAVComponent* av_comp = static_cast<OperaMuminAVComponent*>(*comp_it);
                return m_component_store.selectComponentByData(av_comp->m_component);
            }
        }
    }
    return false;
}

bool
OperaMuminAVComponentHelper::unselectComponent(UVABackend::AVComponent* component)
{
    TRACE_INFO(("Unselecting component %p\n", component));
    bool result = false;

    FrostMutexLock lock(m_component_mutex);
    result = unselectComponentLocked(component);
    
    return result;
}

bool
OperaMuminAVComponentHelper::unselectComponentLocked(UVABackend::AVComponent* component)
{
    TRACE_INFO(("Unselect component of kind: %d\n", component->kind));
    if (component)
    {
        VectorOfAVComponentPtr::iterator comp_it = findComponentLocked(component);
        if (comp_it != m_component_list.end())
        {
            if (m_using_external_components)
            {
                return deselectExternalAVComponentLocked(*comp_it);
            }
            else
            {
                OperaMuminAVComponent* av_comp = static_cast<OperaMuminAVComponent*>(*comp_it);
                return m_component_store.unselectComponentByData(av_comp->m_component);
            }
        }
    }

    SHOULD_NOT_BE_HERE();
    return false;
}

bool
OperaMuminAVComponentHelper::registerExternalComponents(const std::vector<AVComponent*>& components)
{
    CABOT_ASSERT(m_using_external_components);
    bool result = false;
    FrostMutexLock lock(m_component_mutex);
    TRACE_INFO(("Registering external components \n"));
    if (m_using_external_components)
    {
        m_component_list.clear();
        TRACE_INFO(("Adding %zu external components\n", components.size()));
        for(std::vector<AVComponent*>::const_iterator it = components.begin(); 
                it != components.end(); ++it)
        {
            if((*it)->language_iso_639_2 && strlen((*it)->language_iso_639_2) > 3 &&
               (*it)->language_iso_639_2[2] == '-' )
            {
                (*it)->language_iso_639_2[2] = '\0';
            }
            if((*it)->language_bcp_47 && strlen((*it)->language_bcp_47) > 3 &&
               (*it)->language_bcp_47[2] == '-' )
            {
                (*it)->language_bcp_47[2] = '\0';
            }
            m_component_list.push_back((*it));  
        }
        
        NEBULA_LanguageCode lang_code;
        memset(lang_code.iso_str, '\0', NEBULA_LANG_CODE_LEN);
        NEBULA_LanguageSettingGetAudioLanguage(&lang_code);        
        UString language(lang_code.iso_str);
        bool is_audio_description_enabled = NEBULA_IsAudioDescriptionEnabled();
        selectAudioComponentLocked(language, is_audio_description_enabled);
        // ANAKIN-945 removed the subtitle check, should always update component store on update
        m_component_store.notifyComponentsUpdated();
        
        result = true;
    }
    return result;
}

bool
OperaMuminAVComponentHelper::unregisterExternalComponents(const std::vector<AVComponent*>& components)
{
    CABOT_ASSERT(m_using_external_components);
    bool result = false;
    FrostMutexLock lock(m_component_mutex);
    TRACE_INFO(("Unregistering external components \n"));
    
    if (m_using_external_components)
    {
        for(std::vector<AVComponent*>::const_iterator it = components.begin(); 
                it != components.end(); ++it)
        {
            for (VectorOfAVComponentPtr::iterator comp_it = m_component_list.begin(); 
                    comp_it != m_component_list.end(); )
            {
                if ( (*comp_it) == (*it) )
                {
                    comp_it = m_component_list.erase(comp_it);
                }
                else
                {
                    comp_it++;
                }
            }
        }
        // ANAKIN-945 removed the subtitle check, should always update component store on update
        m_component_store.notifyComponentsUpdated();
        
        result = true;
    }
    return result;
}

//Synchronise the active state of a component in the nebula component store with the matching
//component in the list of mumin av components.
//Note:
// - Only send a component changed event for components that have become active.
// - Hidden components will be uneffected.
void
OperaMuminAVComponentHelper::synchroniseComponentActiveState(OIPF_ComponentType component_type,
                                                             int index)
{
    UString component_id_internal;
    if (m_component_store.getComponentIDByIndex(component_type, 
                                                index,
                                                component_id_internal))
    {
        NEBULA_AVComponent component_data{};
        if (m_component_store.getComponentInfoByID(component_id_internal, 
                                                   component_data))
        {
            VectorOfAVComponentPtr::iterator iterator = 
                        findComponentLocked(component_data.type, component_data.index);

            if (iterator != m_component_list.end())
            {
                OperaMuminAVComponent* current_component = 
                            static_cast<OperaMuminAVComponent*>(*iterator);

                if (component_data.is_active != current_component->isActive())
                {
                    current_component->setActive(component_data.is_active);
                    if (component_data.is_active)
                    {
                        ComponentChangeHandler comp_handler(m_client);
                        if (comp_handler.handleComponentChange(current_component, 
                                            ComponentChangeHandler::action_changed))
                        {
                            TRACE_INFO(("Component activeness changed successfully.\n"));
                        }
                    }
                }
            }
            else
            {
                TRACE_ERROR(("Unknown AVComponent - type=%d, index=%d\n",
                                component_data.type, component_data.index));
            }
        }
        else
        {
            TRACE_ERROR(("getComponentInfoByID() failed - component_id=%s\n",
                            component_id_internal.c_str()));
        }

    }
    else
    {
        TRACE_ERROR(("getComponentIDByIndex() failed - type=%d, index=%d\n",
                      component_type, index));
    }
}

void
OperaMuminAVComponentHelper::updateComponentActiveness(
                                        NEBULA_AVComponent const* component_data,
                                        bool active)
{
    if (m_using_external_components || !component_data)
    {
        return;
    }
    
    if (m_is_instance_active != ACTIVE_INSTANCE_VALUE)
    {
        return;
    }

    TRACE_INFO(("updateComponentActiveness - %p active=%d\n", component_data, active));
  
    FrostMutexLock lock(m_component_mutex); // Note: recursive mutex alert. Was removed but caused YODA-8155.

    if (m_is_instance_active != ACTIVE_INSTANCE_VALUE)
    {
        return;
    }

    bool found = false;
    OperaMuminAVComponent* matching_component = 0;

    for (VectorOfAVComponentPtr::iterator it =
            m_component_list.begin(); it != m_component_list.end();
            ++it)
    {
        OperaMuminAVComponent* component = static_cast<OperaMuminAVComponent*>(*it);
        if ((component->m_component->type == component_data->type)
                && (component->m_component->index == component_data->index) )
        {
            matching_component = static_cast<OperaMuminAVComponent*>(*it);
            found = true;
            //break;
        }
        //De-activate previous components
        if (active == true &&
            component->isActive() &&
            (component->m_component->type == component_data->type))
        {
            component->setActive(false);
        }
    }
    if (found && matching_component)
    {
        ComponentChangeHandler comp_handler(m_client);
        matching_component->setActive(active);
        if (comp_handler.handleComponentChange(matching_component, 
                            ComponentChangeHandler::action_changed) &&
                            comp_handler.sendEvent())
        {
            TRACE_INFO(("Component activeness changed successfully.\n"));
        }
    }
}

void
OperaMuminAVComponentHelper::unselectActiveComponentLocked(UVABackend::AVComponentType type)
{
    //if(m_using_external_components)
    {   //no need to call unselect active component for backend streaming since selecting new component will automatically deselect previously active component
        for (VectorOfAVComponentPtr::iterator it = m_component_list.begin();
                it != m_component_list.end(); ++it)
        {
            if ((*it)->isActive() && ((*it)->type == type))
            {
                unselectComponentLocked(*it);
                break;
            }
        }
    }
}

bool
OperaMuminAVComponentHelper::selectInitialComponents(UVABackend::SourceDescription::SourceType type)
{
    NEBULA_LanguageCode lang_code;
    memset(lang_code.iso_str, '\0', NEBULA_LANG_CODE_LEN);
    NEBULA_LanguageSettingGetAudioLanguage(&lang_code);

    UString language(lang_code.iso_str);

    bool is_audio_description_enabled = NEBULA_IsAudioDescriptionEnabled();

    if(m_using_external_components)
    {
        return true;
    }

    bool is_video_selected = selectFirstComponent(type, OIPF_ComponentTypeVideo);
    bool is_audio_selected = selectProperAudioComponent(language, is_audio_description_enabled);

    if ( !is_audio_selected )
    {
        is_audio_selected = selectFirstComponent(type, OIPF_ComponentTypeAudio);
    }

    return is_audio_selected && is_video_selected;
}

bool
OperaMuminAVComponentHelper::selectAudioComponentLocked(UString const& language, bool ad_on)
{
    VectorOfAVComponentPtr::iterator audio_component = m_component_list.end();

    for (VectorOfAVComponentPtr::iterator it = m_component_list.begin();
            it != m_component_list.end(); ++it)
    {
        UString audio_language = (*it)->language_iso_639_2;
        /* reduce language code size to two
         * because it may come like en-gb, we must handle it en language code*/
        audio_language.truncate(2);
        /* convert alpha-2 codes to alpha-3 bibliographic codes */
        NEBULA_LanguageCode alpha_3_code;
        memset(alpha_3_code.iso_str, '\0', NEBULA_LANG_CODE_LEN);
        NEBULA_NormaliseLanguageCode(audio_language.charArray(), alpha_3_code.iso_str);

        if ( ((*it)->type == UVABackend::COMPONENT_TYPE_AUDIO) &&
               (language.compare(alpha_3_code.iso_str) == 0) )
        {
            if ( (*it)->audio_description == ad_on )
            {
                audio_component = it;
                break;
            }
            else
            {
                audio_component = it;
            }
        }
    }

    if (audio_component != m_component_list.end() && !(*audio_component)->isActive())
    {
        if (!(*audio_component)->audio_description)
        {
            unselectActiveComponentLocked(UVABackend::COMPONENT_TYPE_AUDIO);
        }
        if (selectComponentLocked(*audio_component))
        {
            return true;
        }
    }

    return false;
}

bool
OperaMuminAVComponentHelper::selectProperAudioComponent(UString& language, bool ad_on)
{
    FrostMutexLock lock(m_component_mutex);
    return selectAudioComponentLocked(language, ad_on);
}

bool
OperaMuminAVComponentHelper::selectFirstComponent(UVABackend::SourceDescription::SourceType type,
                                                  OIPF_ComponentType comp_type)
{
    FrostMutexLock lock(m_component_mutex);

    bool found_comp_type = false;
    bool is_any_component_active = false;
    for (VectorOfAVComponentPtr::iterator it = m_component_list.begin();
            it != m_component_list.end(); ++it)
    {
        if (((*it)->type == (UVABackend::AVComponentType)comp_type))
        {
            found_comp_type = true;
            //break;
        }
        if (((*it)->type == (UVABackend::AVComponentType)comp_type) &&
            (*it)->isActive())
        {
            is_any_component_active = true;
        }
    }

    if (!found_comp_type)
    {
        //even if there is no component added to component list
        //we have to call ICE_MediaPlayerSelectStream to start playing
        //this fix is added for Smooth Streaming & MSE Streaming
        if (type == UVABackend::SourceDescription::MSE_STREAMING)
        {
            return false;
        }

        NEBULA_AVComponent* component = new NEBULA_AVComponent;
        component->type = comp_type;
        component->is_active = frost_true;
        component->index = 0;

        OperaMuminAVComponent* comp = new OperaMuminAVComponent((UVABackend::AVComponentType)comp_type,
                                               "",
                                               false,
                                               0.0,
                                               "",
                                               "",
                                               false,
                                               0,
                                               false,
                                               true,
                                               component,
                                               false,
                                               0,
                                               false,
                                               0,
                                               "",
                                               "");

        if (comp)
        {
            m_component_list.push_back(comp);
            ComponentChangeHandler comp_handler(m_client);
            
            if (comp_handler.handleComponentChange(comp, 
                ComponentChangeHandler::action_add)
                && comp_handler.sendEvent())
            {
                TRACE_INFO(("Component added successfully.\n"));
                comp->setKnownByOpera(true);
                comp->setActive(true);
                if (comp_handler.handleComponentChange(comp, 
                                ComponentChangeHandler::action_changed) &&
                                comp_handler.sendEvent())
                {
                    TRACE_INFO(("Component activeness changed successfully.\n"));
                }
            }
            selectComponentLocked(comp);
            return true;
        }
    }
    else if (!is_any_component_active)
    {
        unselectActiveComponentLocked((UVABackend::AVComponentType)comp_type);
        for (VectorOfAVComponentPtr::iterator it = m_component_list.begin();
                it != m_component_list.end(); ++it)
        {
            if ( !(*it)->isActive() &&
                    ((*it)->type == (UVABackend::AVComponentType)comp_type) )
            {
                if ( selectComponentLocked(*it) )
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool
OperaMuminAVComponentHelper::selectExternalAVComponentLocked(AVComponent* external_comp)
{
    return sendExternalAVComponentActivateEventLocked(external_comp, true);
}

bool
OperaMuminAVComponentHelper::deselectExternalAVComponentLocked(AVComponent* external_comp)
{
    return sendExternalAVComponentActivateEventLocked(external_comp, false);
}

bool
OperaMuminAVComponentHelper::sendExternalAVComponentActivateEventLocked(
                                        AVComponent* external_comp,
                                        bool active)
{
    CABOT_UNUSED_PARAM(external_comp);
    CABOT_UNUSED_PARAM(active);
    return false;
}

double
OperaMuminAVComponentHelper::convertAspectRatio(NEBULA_AspectRatio ratio)
{
    double aspect_ratio = 1.78;
    if (ratio == NEBULA_AspectRatio_4_3)
    {
        aspect_ratio = 1.33;
    }
    else if (ratio == NEBULA_AspectRatio_16_9)
    {
        aspect_ratio = 1.78;
    }
    return aspect_ratio;
}

UString
OperaMuminAVComponentHelper::getEncoding(NEBULA_AVComponent const* component)
{
    UString encoding = "UNKNOWN";
    switch (component->encoding)
    {
        case HBBTV_EncodingFormat_mpeg1_layer3:
            if(component->pid)
            {
                encoding = "MPEG1_L2";
            }
            else
            {
                encoding = "MPEG1_LAYER3";
            }
            break;
        case HBBTV_EncodingFormat_eac3:
            encoding = "E-AC3";
            break;
        case HBBTV_EncodingFormat_heaac:
            encoding = "HEAAC";
            break;
        case HBBTV_EncodingFormat_mpeg2_sd_25:
            encoding = "MPEG2_SD_25";
            break;
        case HBBTV_EncodingFormat_avc_sd_25:
            encoding = "AVC_SD_25";
            break;
        case HBBTV_EncodingFormat_avc_hd_25:
            encoding = "AVC_HD_25";
            break;
        case HBBTV_EncodingFormat_dts:
            encoding = "DTS";
            break;
        default:
            break;
    }
    return encoding;
}

HBBTV_EncodingFormat
OperaMuminAVComponentHelper::getEncodingFormat(char* encoding)
{
    HBBTV_EncodingFormat encoding_format = HBBTV_EncodingFormat_Unknown;
    if (encoding)
    {
        if (strstr(encoding,"MPEG1") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_mpeg1_layer3;
        }
        else if (strstr(encoding,"E-AC3") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_eac3;
        }
        else if (strstr(encoding,"HEAAC") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_heaac;
        }
        else if (strstr(encoding,"MPEG2_SD_25") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_mpeg2_sd_25;
        }
        else if (strstr(encoding,"AVC_SD_25") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_avc_sd_25;
        }
        else if (strstr(encoding,"AVC_HD_25") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_avc_hd_25;
        }
        else if (strstr(encoding,"DTS") != 0)
        {
            encoding_format = HBBTV_EncodingFormat_dts;
        }
    }
    return encoding_format;
}


int
OperaMuminAVComponentHelper::getAudioChannels(HBBTV_EncodingFormat encoding_format)
{
    /*
     * Hack for multi-codec streaming tests in MIT-xperts HBBTV tests
     * DOLBY video format / AVComponents
     *  Multi-Codec in mp4
     *  Multi-Codec in MPEG-TS
     *
     *  MIT test page checks audio channel numbers to run the following steps:
     *      "Select audio: EAC3, 5.1 ch"
     *      "Select audio: HE-AAC, 2.0ch"
     *  Since setting channel numbers is safe and reference TV sets pass
     *  these tests, supported audio channel numbers are added for various encoding formats.
     */
    int audio_channels;
    switch (encoding_format)
    {
        case HBBTV_EncodingFormat_eac3:
            audio_channels = 5;
            break;
        case HBBTV_EncodingFormat_mpeg1_layer3:
        case HBBTV_EncodingFormat_heaac:
            audio_channels = 2;
            break;
        default:
            audio_channels = 0;
            break;
    }
    return audio_channels;
}

UString
OperaMuminAVComponentHelper::getLanguage(
                               OIPF_ComponentType component_type,
                               NEBULA_AVComponent const* component)
{
    UString language = component->language_code;
    if (language == "ger")
    {
        language = "deu";
    }
    if (language == "fre")
    {
        language = "fra";
    }

    if (component_type == OIPF_ComponentTypeVideo)
    {
        language = "";
    }
    return language;
}

UVABackend::AVComponentType
OperaMuminAVComponentHelper::convertNebulaComponentTypeToMumin(OIPF_ComponentType nebula_type)
{
    UVABackend::AVComponentType mumin_component_type = UVABackend::COMPONENT_TYPE_VIDEO;
    switch (nebula_type)
    {
        case OIPF_ComponentTypeVideo:
            mumin_component_type = UVABackend::COMPONENT_TYPE_VIDEO;
            break;
        case OIPF_ComponentTypeAudio:
            mumin_component_type = UVABackend::COMPONENT_TYPE_AUDIO;
            break;
        case OIPF_ComponentTypeSubtitle:
            mumin_component_type = UVABackend::COMPONENT_TYPE_SUBTITLE;
            break;
        default:
            SHOULD_NOT_BE_HERE();
    }
    return mumin_component_type;
}

VectorOfAVComponentPtr::iterator
OperaMuminAVComponentHelper::findComponentLocked(UVABackend::AVComponent const* component)
{
    VectorOfAVComponentPtr::iterator it = m_component_list.begin();
    for (; it != m_component_list.end(); ++it)
    {
        if (*it == component)
        {
            break;
        }
    }

    return it;
}

VectorOfAVComponentPtr::iterator
OperaMuminAVComponentHelper::findComponentLocked(OIPF_ComponentType component_type, 
                                                int index)
{
    return std::find_if(m_component_list.begin(), m_component_list.end(),
                        [component_type, index] 
                        (const VectorOfAVComponentPtr::value_type& component)
                        {
                            const OperaMuminAVComponent* av_component = 
                                        static_cast<OperaMuminAVComponent*>(component);

                            return (av_component->m_component->type == component_type &&
                                    av_component->m_component->index == index);
                        }
            );
}

bool
OperaMuminAVComponentHelper::isComponentInList(UVABackend::AVComponent* component, VectorOfAVComponentPtr list, int* index)
{
    bool ret = false;
    int tmp_size = -1;

    if(!component)
    {
        TRACE_ERROR(("Component is not valid\n"));
        *index = -1;
        return ret;
    }

    UString component_encoding = component->encoding;
    UString comp_lang_bcp_47 = component->language_bcp_47;
    UString comp_lang_iso_639_2 = component->language_iso_639_2;

    for(VectorOfAVComponentPtr::iterator it =
            list.begin(); it != list.end();
            ++it)
    {
        tmp_size++;

        if(!(*it))
        {
            TRACE_ERROR(("AV component iterator is not valid.\n"));
            continue;
        }

        UString it_encoding = (*it)->encoding;
        UString it_language_bcp_47    = (*it)->language_bcp_47;
        UString it_language_iso_639_2 = (*it)->language_iso_639_2;

        if(((*it)->pid == (component->pid)) &&
                ((*it)->type == (component->type)) &&
                ((*it)->audio_channels == (component->audio_channels))  &&
                ((*it)->audio_description == (component->audio_description))  &&
                ((*it)->bandwidth == (component->bandwidth))  &&
                ((*it)->encrypted == (component->encrypted))  &&
                ((*it)->hearing_impaired == (component->hearing_impaired))  &&
                ((*it)->kind == (component->kind)) &&
//              ((*it)->isActive() == component->isActive()) &&
                (component_encoding == it_encoding)
                )
        {
            if (it_language_bcp_47 != comp_lang_bcp_47 ||
                it_language_iso_639_2 != comp_lang_iso_639_2)
            {
                continue;
            }
            ret = true;
            break;
        }
    }

    if(ret)
    {
        *index = tmp_size;
        TRACE_INFO(("Component found in list.\n"));
    }
    else
    {
        *index = -1;
        TRACE_INFO(("Could not find a/v component (%p).\n", component));
    }

    return ret;
}

void
OperaMuminAVComponentHelper::fillComponentList(NEBULA_AVComponentList* component_list,
                                               OIPF_ComponentType component_type)
{
    if (m_is_instance_active != ACTIVE_INSTANCE_VALUE)
    {
        return;
    }

    FrostMutexLock lock(m_component_mutex);
    int index = 0;

    if (m_is_instance_active != ACTIVE_INSTANCE_VALUE)
    {
        return;
    }

    UVABackend::AVComponentType type = convertNebulaComponentTypeToMumin(component_type);

    for(VectorOfAVComponentPtr::iterator
             it = m_component_list.begin(); it != m_component_list.end() && 
                                            index < MAX_COMPONENT_SIZE; ++it)
    {
        if (!(*it))
        {
            TRACE_ERROR(("AV component iterator is not valid.\n"));
            continue;
        }

        if ( (*it)->type == type && component_list->components)
        {
            component_list->components[index].type = component_type;
            
            if ((*it)->id != nullptr)
            {
                strncpy(component_list->components[index].id, 
                        (*it)->id, 
                        NEBULA_AVComponent_id_length);
            }
            component_list->components[index].audio_channels = (*it)->audio_channels;
            component_list->components[index].audio_description =
                                           (*it)->audio_description ? frost_true : frost_false;
            component_list->components[index].encrypted =
                                           (*it)->encrypted ? frost_true : frost_false;
            component_list->components[index].hearing_impaired =
                                           (*it)->hearing_impaired ? frost_true : frost_false;
            component_list->components[index].is_active =
                                           (*it)->isActive() ? frost_true : frost_false;

            // SUNSTONE-1118: nullptr being passed in. Leading to an SEG fault.
            if ((*it)->language_iso_639_2 != nullptr)
            {
                strncpy(component_list->components[index].language_code,
                                    (*it)->language_iso_639_2, NEBULA_LANG_CODE_LEN);
            }
            else
            {
                component_list->components[index].language_code[0] = '\0';
            }
            component_list->components[index].kind = convertToNebulaKind((*it)->kind);

            if ((component_list->components[index].kind == NEBULA_KindNone) &&
                (component_type == OIPF_ComponentTypeSubtitle))
            {
                component_list->components[index].kind = NEBULA_KindSubtitles;
            }

            index++;
        }
    }
    component_list->number_of_current_components = index;
}

void
OperaMuminAVComponentHelper::activateSelectedComponent(
        OIPF_ComponentType type,
        const char* id,
        const char* lang,
        bool activate,
        const char* label)
{    
    if (id != nullptr)
    {
        FrostMutexLock lock(m_component_mutex);
        for(VectorOfAVComponentPtr::iterator
                it = m_component_list.begin(); it != m_component_list.end(); ++it)
        {
            if (!(*it))
            {
                TRACE_ERROR(("AV component iterator is not valid.\n"));
                continue;
            }

            UString it_language_iso_639_2 = (*it)->language_iso_639_2;
            frost_bool can_select_component = frost_true;

            if ((type == OIPF_ComponentTypeSubtitle) &&
                (it_language_iso_639_2.compare(lang) != 0))
            {
                can_select_component = frost_false;
            }
            
            if (convertNebulaComponentTypeToMumin(type) == (*it)->type &&
                (*it)->id != nullptr && 
                strcmp((*it)->id, id) == 0 &&
                can_select_component)
            {
                unselectActiveComponentLocked((*it)->type);
                if (activate)
                {
                    if (!selectComponentLocked(*it))
                    {
                        TRACE_ERROR(("AV component could not be selected.\n"));
                    }
                    else
                    {
                        TRACE_INFO(("AV component id = '%s' activated.\n", (*it)->id));
                    }
                }
                break;
            }
        }
    }
}

void
OperaMuminAVComponentHelper::activateSelectedComponent(NEBULA_AVComponent* component)
{
    FrostMutexLock lock(m_component_mutex);
    UVABackend::AVComponentType type = convertNebulaComponentTypeToMumin(component->type);

    for(VectorOfAVComponentPtr::iterator
             it = m_component_list.begin(); it != m_component_list.end(); ++it)
    {
        if(!(*it))
        {
            TRACE_ERROR(("AV component iterator is not valid.\n"));
            continue;
        }

        char const* language = component->language_code;
        UString it_language_iso_639_2 = (*it)->language_iso_639_2;

        if(((*it)->type == type) &&
                ((*it)->audio_channels == (component->audio_channels))  &&
                ((*it)->audio_description == (component->audio_description == frost_true))  &&
                ((*it)->encrypted == (component->encrypted == frost_true))  &&
                ((*it)->hearing_impaired == (component->hearing_impaired == frost_true))  &&
                ((*it)->isActive() == component->is_active) &&
                ((it_language_iso_639_2.compare(language) == 0))
                )
        {
            unselectActiveComponentLocked((*it)->type);
            if ( !selectComponent(*it) )
            {
                TRACE_ERROR(("AV component could not be selected.\n"));
            }
            break;
        }
    }
}

bool
OperaMuminAVComponentHelper::addUserComponent(UVABackend::AVComponent* component)
{
    FrostMutexLock lock(m_component_mutex);

    TRACE_INFO(("addUserComponent - %p\n", component));

    NEBULA_AVComponent* component_data = new NEBULA_AVComponent;
    OperaMuminAVComponent* av_comp = static_cast<OperaMuminAVComponent*>(component);
    NEBULA_AVComponent* new_component_data = m_component_store.addUserComponent((OIPF_ComponentType)component->type);
    new_component_data->stream_id = 0;
    new_component_data->is_active = frost_false;
    new_component_data->pid = 0;
    new_component_data->system_format = NEBULA_AVSystemFormat_Unknown;
    new_component_data->encoding = getEncodingFormat(component->encoding);
    new_component_data->encrypted = frost_false;
    new_component_data->aspect_ratio = component->aspect_ratio < 1.77 ? NEBULA_AspectRatio_4_3 : NEBULA_AspectRatio_16_9;
    if (component->language_iso_639_2 != nullptr)
    {
        strncpy(new_component_data->language_code,
                            component->language_iso_639_2, NEBULA_LANG_CODE_LEN);
    }
    else
    {
        new_component_data->language_code[0] = '\0';
    }
    new_component_data->audio_description = (frost_bool)component->audio_description;
    new_component_data->audio_channels = component->audio_channels;
    new_component_data->hearing_impaired = frost_false;
    new_component_data->receiver_mixed_ad = frost_false;
    if(component->has_componentTag)
    {
        new_component_data->component_tag = component->componentTag;
    }
    else
    {
        new_component_data->component_tag = 0;
    }
    new_component_data->kind = NEBULA_KindUnknown;
    *component_data = *new_component_data;
    av_comp->setComponent(component_data);
    m_component_list.push_back(component);

    TRACE_INFO(("addUserComponent - finished\n"));

    return true;
}
