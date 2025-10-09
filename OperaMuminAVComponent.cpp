/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMuminAVComponent
 */

#include "OperaMuminAVComponent.hpp"
#include "frost/frost_semaphore.h"

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("mumin/component");

OperaMuminAVComponent::OperaMuminAVComponent( UVABackend::AVComponentType _type,
            const char* _encoding,
            bool _encrypted,
            double _aspect_ratio,
            const char* _language_bcp_47,
            const char* _language_iso_639_2,
            bool _audio_description,
            int _audio_channels,
            bool _hearing_impaired,
            bool _is_active,
            NEBULA_AVComponent* _component,
            bool _has_pid,
            int _pid,
            bool _has_componentTag,
            int _componentTag,
            const char* _id,
            const char* _label
            )
    : UVABackend::AVComponent(_type,
                          _encoding,
                          _encrypted,
                          _aspect_ratio,
                          _language_bcp_47,      // bcp 47 lang codes
                          _language_iso_639_2,   // iso639-2 lang codes
                          _audio_description,
                          _audio_channels,
                          _hearing_impaired,
                          NONE,
                          _label,
                          _id,
                          0,
                          NULL,
                          0,    // TODO: source id. This needs to be the actual source id for adaptive streaming
                          _has_pid,
                          _pid,
                          _has_componentTag,
                          _componentTag)
    , m_component(_component)
    , m_known_by_opera(false)
{
    TRACE_VERBOSE(("created \n"));
    setActive(_is_active);
}

OperaMuminAVComponent::~OperaMuminAVComponent()
{
    TRACE_VERBOSE(("destroyed\n"));
    if (m_component)
    {
        delete m_component;
    }
}

void
OperaMuminAVComponent::setActive(bool active)
{
    UVABackend::AVComponent::setActive(active);
    if(m_component)
    {
        m_component->is_active = (frost_bool)active;
    }
}

void
OperaMuminAVComponent::setKnownByOpera(bool known_by_opera)
{
    m_known_by_opera = known_by_opera;
}

bool
OperaMuminAVComponent::isKnownByOpera() const
{
    return m_known_by_opera;
}

void 
OperaMuminAVComponent::setComponent(NEBULA_AVComponent* component)
{
    TRACE_INFO(("setComponent is running\n"));
    m_component = component;
}

