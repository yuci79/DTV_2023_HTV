/**
 * Copyright 2017 Vestel.  All rights reserved.
 *
 * @file   
 * @brief  This file defines the class HbbTvAppKeyset.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVAPPKEYSET_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVAPPKEYSET_HPP_INCLUDED

#include "vewd_common/KeySetUtilities.hpp"

//These two keyset values follow the Hbbtv 1.4.1 specs (see Table12 sec10.2.2.1)
//default keyset always available to all applications
const int keyset_always_available = keyset_red | keyset_green | keyset_yellow | keyset_blue
                                    | keyset_navigation; 

//This is the default value specified in OIPF vol5 DAE specs section 7.2.5
//used only to set the keyset to a default value for broadcast independent app  
const int default_hbbtv_keyset = keyset_red | keyset_green | keyset_yellow | keyset_blue
                        | keyset_navigation | keyset_vcr | keyset_scroll | keyset_info
                        | keyset_numeric | keyset_alpha;
    
class OperaWindow;

class OperaHbbTVAppKeyset
{
public:
    OperaHbbTVAppKeyset();
    ~OperaHbbTVAppKeyset();
 
public:

     /**
     * @brief Set the keyset to the default value 
     *
     * @param window The current active opera window
     */
    void setDefaultKeyset(OperaWindow* window);

     /**
     * @brief Set the keyset to the specified value 
     *
     * @param window The current active opera window
     * @param keyset_value  New keyset value
     * @param is_autostart  If the app is autostart
     * @param is_broadcast_related  If the app is broadcast related
     * @param is_active If the app is active
     * @param is_hbbtv_service If the app is related to an HbbTV service
     */
    void setKeysetValue(OperaWindow* window,
                        const int keyset_value,
                        bool is_autostart,
                        bool is_broadcast_related,
                        bool is_active,
                        bool is_hbbtv_service); 
    
private: 
    OperaHbbTVAppKeyset(OperaHbbTVAppKeyset const&);
    OperaHbbTVAppKeyset& operator=(OperaHbbTVAppKeyset const&);

    /**
     * @brief Checks to see if the key set has already been set
     *
     * @param window The current active opera window
     */
    bool isKeysetAlreadySet (OperaWindow const * const window) const;    
};

#endif
//CABOT_VEWD_INTEGRATION_CORE_HBBTVAPPKEYSET_HPP_INCLUDED
