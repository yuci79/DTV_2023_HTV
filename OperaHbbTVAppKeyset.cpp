/**
 * Copyright 2017 Vestel.  All rights reserved.
 *
 * @file   
 * @brief  This file contains the implementation of the class OperaHbbTVAppKeyset.
 */
#include "OperaHbbTVAppKeyset.hpp"

#include "OperaWindow.hpp"

#include "profile_helper/ProfileHelper.h"

#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/hbbtvappkeyset");

OperaHbbTVAppKeyset::OperaHbbTVAppKeyset()
{
}

OperaHbbTVAppKeyset::~OperaHbbTVAppKeyset()
{
}

void
OperaHbbTVAppKeyset::setDefaultKeyset(OperaWindow* window)
{
    if (window && !isKeysetAlreadySet(window))
    {
        window->setKeySet(default_hbbtv_keyset, std::vector<int>());
    }
}

void
OperaHbbTVAppKeyset::setKeysetValue(OperaWindow* window,
                                    const int keyset_value,
                                    bool is_autostart,
                                    bool is_broadcast_related,
                                    bool is_active,
                                    bool is_hbbtv_service)
{
    if (window)
    {
        if (Profile_IsFreeviewPlayEnabled())
        {
            //set the keyset by following the HbbTv 2.0.1 specs 10.2.2.1
            if (is_autostart && is_broadcast_related && !is_active)
            {               
                if (keyset_value <= keyset_always_available)
                {
                    window->setKeySet(keyset_value, std::vector<int>());
                }
                else
                {
                    window->setKeySet(keyset_always_available, std::vector<int>());
                }
            }
            else
            {
                if (is_hbbtv_service && !is_broadcast_related && (keyset_value & keyset_info != 0))
                {
                    // SUNSTONE-2367:HbbTV IP Delivered Linear Services and RCU Button Handling
                    // FVP Tech Spec v6.0.2 Section 8.3 RCU Handling
                    // Where a device has separate ‘info’ and ‘enter/ok’ keys the ‘info’ key may be used by 
                    // the device to offer channel banner information against an IP linear service.
                    // So, we remove the INFO bit (0x80) from the key set to allow the info RCU key
                    // to go to the UI to access the TVBanner when on an HbbTV IP service (is_hbbtv_service).
                    int modified_keyset_value = keyset_value & ~keyset_info;
                    TRACE_WARN(("Removing INFO (0x80) from keyset %#x to %#x for broadcast independent app\n",
                                keyset_value, modified_keyset_value));
                    window->setKeySet(modified_keyset_value, std::vector<int>());
                }
                else
                {
                    window->setKeySet(keyset_value, std::vector<int>());
                }
                
            }
        }
        else
        {
            window->setKeySet(keyset_value, std::vector<int>());
        }
    }
}

bool 
OperaHbbTVAppKeyset::isKeysetAlreadySet (OperaWindow const * const window) const
{
    bool result = false;

    if (window) 
    {
        int keyset_value = 0;
        std::vector<int> other_keys;
        window->getKeySet(&keyset_value, &other_keys);

        result = (keyset_value != 0);
    }

    return result;
}
