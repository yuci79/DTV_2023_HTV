/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation of the NebulaAVSClientIpcAurora class.
 */

#include "NebulaAVSClientIpcAurora.hpp"
#include "nebula_src/adaptation_layer/ipc/NebulaAVSClientIpcTypes.hpp"
#include "nebula/adaptation_layer/nebula_avs_client.h"
#include <rpc/server.h>
#include <vector>
#include <string.h>

void
NebulaAVSClientIpcAurora::bindToServer(rpc::server& server)
{   
    // Add bindings for nebula_avs_client 'C' functions.
    server.bind(IPC_NAME(NEBULA_AVSClientGetProductInfo),[]()
    {
                                                                   
        const char* product_id = nullptr;
        const char* client_id = nullptr;
        
        NebulaIpcType<frost_bool,StringPair> result;       
        result.return_value = NEBULA_AVSClientGetProductInfo(&product_id, &client_id);

        if(product_id && client_id)
        {
            result.output_params.first.assign(product_id);
            result.output_params.second.assign(client_id); 
        }
        NEBULA_discardBuffer(product_id);
        NEBULA_discardBuffer(client_id);                                                                                                                                                                                     
        return result;
           
        
    });

    server.bind(IPC_NAME(NEBULA_AVSClientGetDeviceID),[]
    {
        const char* device_id = nullptr;      
        NebulaIpcType<frost_bool,std::string> result;
        
        result.return_value = NEBULA_AVSClientGetDeviceID(&device_id);  
        if(device_id)
        { 
            result.output_params.assign(device_id);
        }
        NEBULA_discardBuffer(device_id);                                                                                               
        return result;
    });
    server.bind(IPC_NAME(NEBULA_AVSClientGetClientProperties),[]
    {
        const char* client_properties = nullptr;      
        NebulaIpcType<frost_bool,std::string> result;
        
        result.return_value = NEBULA_AVSClientGetClientProperties(&client_properties);  
        if(client_properties)
        { 
            result.output_params.assign(client_properties);
        }
        NEBULA_discardBuffer(client_properties);                                                                                               
        return result;        
    });
    server.bind(IPC_NAME(NEBULA_AVSClientGetAuthInfo),[]()
    {
        const char* user_code = nullptr;
        const char* auth_url = nullptr;
        
        NebulaIpcType<frost_bool,StringPair> result;
        
        result.return_value = NEBULA_AVSClientGetAuthInfo(&user_code, &auth_url);

        if(user_code && auth_url)
        {
            result.output_params.first.assign(user_code);
            result.output_params.second.assign(auth_url); 
        }       
        NEBULA_discardBuffer(user_code);
        NEBULA_discardBuffer(auth_url);
        return result;         
        
    }); 
    server.bind(IPC_NAME(NEBULA_AVSClientSetEnabled),[]
    (bool enabled) 
    {
        return NEBULA_AVSClientSetEnabled(enabled);
    });
    server.bind(IPC_NAME(NEBULA_AVSClientIsEnabled), NEBULA_AVSClientIsEnabled);
    server.bind(IPC_NAME(NEBULA_AVSClientGetLocale),[]
    {
        
        const char* locale = nullptr;      
        NebulaIpcType<frost_bool,std::string> result;
        
        result.return_value = NEBULA_AVSClientGetLocale(&locale);  
        if(locale)
        { 
            result.output_params.assign(locale);
        }
        NEBULA_discardBuffer(locale);                                                                                               
        return result;
    });

    server.bind(IPC_NAME(NEBULA_AVSClientGetAlexaOnStandbyEnabled), NEBULA_AVSClientGetAlexaOnStandbyEnabled);
    server.bind(IPC_NAME(NEBULA_AVSClientSetAlexaOnStandbyEnabled),[]
    (bool enabled) 
    {
        return NEBULA_AVSClientSetAlexaOnStandbyEnabled(enabled);
    });
    
    server.bind(IPC_NAME(NEBULA_AVSClientSetupCompleted),[]
    (bool success) 
    {
        return NEBULA_AVSClientSetupCompleted(success);
    });
}
