/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMuminAVComponentHelper
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINAVCOMPONENTHELPER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINAVCOMPONENTHELPER_HPP_INCLUDED

#include "uva_backend.h"
#include "uva_client.h"
#include "uva_event.h"
#include "nebula/adaptation_layer/nebula_components.h"
#include "nebula/adaptation_layer/nebula_language_setting.h"
#include "nebula/adaptation_layer/nebula_configuration.h"
#include "utilities_public/UString.hpp"
#include <vector>
#include "utilities_public/FrostMutex.hpp"
#include "utilities_public/AnyCommandThread.hpp"
#include "nebula_src/adaptation_layer/NebulaAvComponentStore.hpp"

class OperaMuminAVComponent;

typedef UVABackend::AVComponent AVComponent;
typedef AVComponent* AVComponentPtr;

typedef std::vector<AVComponentPtr> VectorOfAVComponentPtr;

class OperaMuminAVComponentHelper
{
public:
    OperaMuminAVComponentHelper(AnyAvComponentController& component_controller, 
                                    UVAClient* client,
                                    bool using_external_components,
                                    AnyCommandThread* media_queue = 0);
    ~OperaMuminAVComponentHelper();

    bool updateComponents(bool select_initials = false);
    bool selectComponent(UVABackend::AVComponent* component);
    bool unselectComponent(UVABackend::AVComponent* component);
    void updateComponentActiveness(const NEBULA_AVComponent *component_data, bool active);
    bool registerExternalComponents(const std::vector<AVComponent*>& components);
    bool unregisterExternalComponents(const std::vector<AVComponent*>& components);
    bool selectInitialComponents(UVABackend::SourceDescription::SourceType type);
    bool selectProperAudioComponent(UString& language, bool ad_on);
    void fillComponentList(NEBULA_AVComponentList* component_list,
                           OIPF_ComponentType component_type);
    void activateSelectedComponent(NEBULA_AVComponent* component);
    void activateSelectedComponent(OIPF_ComponentType type,const char* id,const char* lang,bool activate,const char* label);
    bool addUserComponent(UVABackend::AVComponent* component);

    void registerComponentCallbacks();
    void deregisterComponentCallbacks();

private:
    // Do not implement
    OperaMuminAVComponentHelper(OperaMuminAVComponentHelper const &);
    OperaMuminAVComponentHelper& operator=(OperaMuminAVComponentHelper const &);

    double convertAspectRatio(NEBULA_AspectRatio ratio);
    UString getEncoding(NEBULA_AVComponent const* component);
    HBBTV_EncodingFormat getEncodingFormat(char* encoding);
    UString getLanguage(OIPF_ComponentType component_type, NEBULA_AVComponent const* component);
    int getAudioChannels(HBBTV_EncodingFormat encoding_format);
    UVABackend::AVComponentType convertNebulaComponentTypeToMumin(OIPF_ComponentType nebula_type);
    VectorOfAVComponentPtr::iterator findComponentLocked(UVABackend::AVComponent const* component);
    VectorOfAVComponentPtr::iterator findComponentLocked(OIPF_ComponentType component_type, 
                                                         int index);
    bool selectAudioComponentLocked(UString const& language, bool ad_on);
    void unselectActiveComponentLocked(UVABackend::AVComponentType type);
    bool selectExternalAVComponentLocked(AVComponent* external_comp);
    bool deselectExternalAVComponentLocked(AVComponent* external_comp);
    bool selectFirstComponent(UVABackend::SourceDescription::SourceType type, OIPF_ComponentType comp_type);
    void removeComponentsLocked();
    bool selectComponentLocked(UVABackend::AVComponent* component);
    bool unselectComponentLocked(UVABackend::AVComponent* component);
    bool sendExternalAVComponentActivateEventLocked(AVComponent* external_comp, bool active);
    bool isComponentInList(UVABackend::AVComponent* component, VectorOfAVComponentPtr list, int* index);
    void synchroniseComponentActiveState(OIPF_ComponentType component_type, int index);
    bool onFinishedUpdateComponents(bool result);

    NebulaAvComponentStore m_component_store;
    AnyAvComponentController& m_component_controller;
    AnyCommandThread*      m_queue;
    UVAClient* m_client;
    VectorOfAVComponentPtr m_component_list;
    bool m_using_external_components;
    int m_is_instance_active;
    FrostMutex m_component_mutex;
};

#endif //CABOT_OPERA_BROWSER_MUMIN_OPERAMUMINAVCOMPONENTHELPER_HPP_INCLUDED
