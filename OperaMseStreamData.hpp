/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines the class OperaMseStreamData
 */


#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMSESTREAMDATA_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMSESTREAMDATA_HPP_INCLUDED
#include "utilities_public/MseStreamData.hpp"

#include "uva_backend.h"

class UVADecoderBuffer;

/**
 * @brief Encapsulate a @DecoderBuffer instance. This allows destruction
 *        using the native deallocator.
 */
class OperaMseStreamData : public MseStreamData
{
public:
    OperaMseStreamData(UVADecoderBuffer* sample, 
                    UVADRMBackend* drm_backend,
                    UVADRMBackend::UVADRMDecrypter* decrypter); 
    virtual ~OperaMseStreamData();
    virtual bool decryptData();
    
    void createEncryptionInfo();

    /**
     * @brief Set HDR info which is obtained from UVAStreamConfig
     */
    void setStreamConfigInfo();
    
    void setDrmType(int drm_type);

private:
    UVADecoderBuffer* m_native_sample;
    UVADRMBackend* m_drm_backend;
    UVADRMBackend::UVADRMDecrypter* m_decrypter;
    bool m_secure_decrypt;
};

/**
 * @param sample The media data which should be passed to the decoder.
 * @param drm_backend Representing a DRM system resource
 * @param decrypter Used by the media stack to provide decryption functionality to media backends
 * @param direct_decrypt Decrypted data never returned to the caller, but is decoded and rendered at low-level
 *                      decrypter can be null if 'direct_render' is true.
 */
OperaMseStreamData* createOperaMseStreamData(UVADecoderBuffer* sample, 
                                            UVADRMBackend* drm_backend, 
                                            UVADRMBackend::UVADRMDecrypter* decrypter,
                                            bool secure_decrypt = false);

#endif /* OPERAMSESTREAMDATA_HPP_ */
