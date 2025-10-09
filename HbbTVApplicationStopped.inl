/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file   
 * @brief  Implementation of the vewd_integ::HbbTVApplicationStopped template constructor. 
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_HBBTVAPPLICATIONSTOPPED_INL_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_HBBTVAPPLICATIONSTOPPED_INL_INCLUDED
/**
 * @note  This file need only be included where an object is constucted, it is not needed where 
 *        the object is used.
 */

namespace vewd_integ
{

template <class OMI_EVENT_T>
HbbTVApplicationStopped::HbbTVApplicationStopped(const OMI_EVENT_T& hbbtv_app)
    : window_id(hbbtv_app.window_id.internal_id())
    , app_id(hbbtv_app.app_id)
    , org_id(hbbtv_app.org_id)
    , app_url(hbbtv_app.app_url)
    , broadcast_related(hbbtv_app.broadcast_related)
{
}

}
#endif
