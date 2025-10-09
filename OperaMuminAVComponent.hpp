/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMuminAVComponent
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINAVCOMPONENT_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINAVCOMPONENT_HPP_INCLUDED

#include "uva_backend.h"
#include "nebula/adaptation_layer/nebula_components.h"

class OperaMuminAVComponent : public UVABackend::AVComponent
{
public:
    OperaMuminAVComponent( UVABackend::AVComponentType _type,
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
            );

    virtual ~OperaMuminAVComponent();

    void setActive(bool active);
    
    /**
     * @brief Set whether this component is known by broadcast and Opera.
     * @param known_by_opera True if this component is known by both broadcast and Opera, false 
     * if only known by Opera (i.e. a hidden component).
     */
    void setKnownByOpera(bool known_by_opera);
    
    /**
     * @return True if this component is known by both broadcast and Opera, false if only known
     * by Opera (i.e. a hidden component).
     */
    bool isKnownByOpera() const;
    void setComponent(NEBULA_AVComponent* component);

    NEBULA_AVComponent* m_component;
    
private:
    // copy constructor & assignment operations disallowed
    OperaMuminAVComponent(OperaMuminAVComponent const &);
    OperaMuminAVComponent& operator=(OperaMuminAVComponent const &);

    bool m_known_by_opera;
};

#endif /* CABOT_OPERA_BROWSER_MUMIN_OPERAMUMINAVCOMPONENT_HPP_INCLUDED */
