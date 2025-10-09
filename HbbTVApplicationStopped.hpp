/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file   
 * @brief  Declares the HbbTVApplicationStopped class in the vewd_integ namespace.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_HBBTVAPPLICATIONSTOPPED_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_HBBTVAPPLICATIONSTOPPED_HPP_INCLUDED

#include "omi/Handle.h"
#include <string>

// Namespace to distinguish the generated event from the omi::evt equivalent.
namespace vewd_integ
{
/**
 * @brief Contains members extracted from an omi:evt::HbbTVApplicationStarted or 
 *        omi::evt::HbbTVApplicationStopped event which are needed to artificially 
 *        generate an event equivalent to an omi:evt::HbbTVApplicationStopped event. 
 */
class HbbTVApplicationStopped
{
public:
    /**
     * @brief Construct an instance of this class from a suitable omi::evt original.
     */
    template <class OMI_EVENT_T>
    explicit HbbTVApplicationStopped(const OMI_EVENT_T& hbbtv_app);

    /**
     * @brief const public members to replicate omi::evt::HbbTVApplicationStopped
     */
    const omi::Handle window_id;
    const unsigned short app_id;
    const unsigned int org_id;
    const std::string app_url;
    const bool broadcast_related;
};
}
#endif
