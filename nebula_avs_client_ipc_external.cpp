/**
 * Copyright 2021 Cabot Communications.  All rights reserved.
 *
 * @file
 * @brief Nebula IPC implementation of nebula_avs_client.
 */

#include "nebula_src/adaptation_layer/ipc/NebulaAVSClientIpcTypes.hpp"
#include <string>
#include "NebulaIpcHelper.hpp"

namespace 
{
    const char* copyBuffer(const std::string& src)
    {
        if (src.empty())
        {
            return nullptr; 
        }
       // Allocate memory for the destination buffer
        char* buffer = new char[src.length() + 1];

        // Copy the contents of the source string to the buffer
        strcpy(buffer, src.c_str());

        return buffer;
    }
}

extern "C" frost_bool
NEBULA_AVSClientGetDeviceID(const char** device_id)
{
    std::string s_device_id;
    const frost_bool result = nebulaRpcCall<frost_bool, std::string>(s_device_id,
                                            IPC_NAME(NEBULA_AVSClientGetDeviceID));  
       
    *device_id = copyBuffer(s_device_id);

    return result;   
}

extern "C" frost_bool
NEBULA_AVSClientGetClientProperties(const char** client_properties)
{       
    std::string s_client_properties;
    const frost_bool result = nebulaRpcCall<frost_bool, std::string>(s_client_properties,
                                            IPC_NAME(NEBULA_AVSClientGetClientProperties));   

    *client_properties = copyBuffer(s_client_properties);

    return result;   
}

extern "C" frost_bool
NEBULA_AVSClientGetProductInfo(const char** product_id, const char** client_id)
{
    StringPair s_data;

    const frost_bool result = nebulaRpcCall<frost_bool, StringPair>(s_data,
                                            IPC_NAME(NEBULA_AVSClientGetProductInfo));
     
    *product_id = copyBuffer(s_data.first);
    *client_id = copyBuffer(s_data.second);

    return result;   
}

extern "C" frost_bool
NEBULA_AVSClientGetAuthInfo(const char** user_code, const char** auth_url)
{  
    StringPair s_data;
    const frost_bool result = nebulaRpcCall<frost_bool, StringPair>(s_data,
                                            IPC_NAME(NEBULA_AVSClientGetAuthInfo)); 
    *user_code = copyBuffer(s_data.first);
    *auth_url = copyBuffer(s_data.second);

    return result;              
}

extern "C" frost_bool
NEBULA_AVSClientGetLocale(const char** locale)
{
    std::string s_locale;
    const frost_bool result = nebulaRpcCall<frost_bool, std::string>(s_locale,
                                            IPC_NAME(NEBULA_AVSClientGetLocale)); 
                                            
    *locale = copyBuffer(s_locale);
                                            
    return result;
}

extern "C" frost_bool 
NEBULA_AVSClientIsEnabled()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AVSClientIsEnabled));
}

extern "C" frost_bool 
NEBULA_AVSClientGetAlexaOnStandbyEnabled()
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AVSClientGetAlexaOnStandbyEnabled));
}

extern "C" frost_bool
NEBULA_AVSClientSetAlexaOnStandbyEnabled(bool enabled)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AVSClientSetAlexaOnStandbyEnabled),enabled);
}

extern "C" frost_bool
NEBULA_AVSClientSetupCompleted(bool  success)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AVSClientSetupCompleted),success);
}

extern "C" frost_bool
NEBULA_AVSClientSetEnabled(bool enabled)
{
    return nebulaRpcCall<frost_bool>(IPC_NAME(NEBULA_AVSClientSetEnabled),enabled);
}

extern "C" void
NEBULA_discardBuffer(const char* buffer)
{
    delete [] buffer;
}
