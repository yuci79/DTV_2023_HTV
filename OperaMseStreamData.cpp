/**
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementation of the class
 * OperaMseStreamData
 */

#include "OperaMseStreamData.hpp"

#include "uva_chunkdemuxer.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("mumin/mse_stream_data");

namespace
{
    /**
     * @brief Convert UVAStreamConfig::Codec type to StreamCodec
     * @return Return converted value
     */
    StreamCodec convertToEclipseCodec(UVAStreamConfig::Codec codec)
    {
        StreamCodec eclipse_codec = unknown_codec;
        switch (codec)
        {
        case UVAStreamConfig::AAC:
            eclipse_codec = audio_aac;
            break;
        case UVAStreamConfig::MP3:
            eclipse_codec = audio_mp3;
            break;
        case UVAStreamConfig::PCM:
            eclipse_codec = audio_pcm;
            break;
        case UVAStreamConfig::VORBIS:
            eclipse_codec = audio_vorbis;
            break;
        case UVAStreamConfig::OPUS:
            eclipse_codec = audio_opus;
            break;
        case UVAStreamConfig::EAC3:
            eclipse_codec = audio_eac3;
            break;
        case UVAStreamConfig::AC3:
            eclipse_codec = audio_ac3;
            break;
        case UVAStreamConfig::DTS:
            eclipse_codec = audio_dts;
            break;
        case UVAStreamConfig::END_AUDIO_TYPES:
            eclipse_codec = end_of_audio_codecs;
            break;
        case UVAStreamConfig::H264:
            eclipse_codec = video_h264;
            break;
        case UVAStreamConfig::H265:
            eclipse_codec = video_h265;
            break;
        case UVAStreamConfig::VC1:
            eclipse_codec = video_vc1;
            break;
        case UVAStreamConfig::MPEG2:
            eclipse_codec = video_mpeg2;
            break;
        case UVAStreamConfig::MPEG4:
            eclipse_codec = video_mpeg4;
            break;
        case UVAStreamConfig::THEORA:
            eclipse_codec = video_theora;
            break;
        case UVAStreamConfig::VP8:
            eclipse_codec = video_vp8;
            break;
        case UVAStreamConfig::VP9:
            eclipse_codec = video_vp9;
            break;
        case UVAStreamConfig::END_VIDEO_TYPES:
            eclipse_codec = end_of_video_codecs;
            break;
        case UVAStreamConfig::WVTT:
            eclipse_codec = subtitle_wvtt;
            break;
        case UVAStreamConfig::TTML:
            eclipse_codec = subtitle_ttml;
            break;
        case UVAStreamConfig::END_TEXT_TYPES:
            eclipse_codec = end_of_subtitle_codecs;
            break;
        case UVAStreamConfig::UNKNOWN:
        default:
            eclipse_codec = unknown_codec;
            break;
        }
        return eclipse_codec;
    }

    /**
     * @brief Convert UVAStreamConfig::MatrixCoefficients type to HDRMatrixCoefficients
     * @return Return converted value
     */
    HDRMatrixCoefficients convertMatrixCoefficients(
                            UVAStreamConfig::MatrixCoefficients matrix_coefficients)
    {
        HDRMatrixCoefficients eclipse_matrix_coefficients;
        switch (matrix_coefficients)
        {
        case UVAStreamConfig::MatrixCoefficients::GBR:
            eclipse_matrix_coefficients = matrix_coefficients_gbr;
            break;
        case UVAStreamConfig::MatrixCoefficients::BT709:
            eclipse_matrix_coefficients = matrix_coefficients_bt709;
            break;
        case UVAStreamConfig::MatrixCoefficients::FCC:
            eclipse_matrix_coefficients = matrix_coefficients_fcc;
            break;
        case UVAStreamConfig::MatrixCoefficients::BT470BG:
            eclipse_matrix_coefficients = matrix_coefficients_bt470bg;
            break;
        case UVAStreamConfig::MatrixCoefficients::SMPTE170M:
            eclipse_matrix_coefficients = matrix_coefficients_smpte170m;
            break;
        case UVAStreamConfig::MatrixCoefficients::SMPTE240M:
            eclipse_matrix_coefficients = matrix_coefficients_smpte240m;
            break;
        case UVAStreamConfig::MatrixCoefficients::YCOCG:
            eclipse_matrix_coefficients = matrix_coefficients_ycocg;
            break;
        case UVAStreamConfig::MatrixCoefficients::BT2020_NON_CONSTANT_LUMINANCE:
            eclipse_matrix_coefficients = matrix_coefficients_bt2020_non_constant_luminance;
            break;
        case UVAStreamConfig::MatrixCoefficients::BT2020_CONSTANT_LUMINANCE:
            eclipse_matrix_coefficients = matrix_coefficients_bt2020_constant_luminance;
            break;
        case UVAStreamConfig::MatrixCoefficients::UNSPECIFIED:
        default:
            eclipse_matrix_coefficients = matrix_coefficients_unspecified;
            break;
        }
        return eclipse_matrix_coefficients;
    }

    /**
     * @brief Convert UVAStreamConfig::ChromaSiting type to HDRChromaSiting
     * @return Return converted value
     */
    HDRChromaSiting convertChromaSiting(UVAStreamConfig::ChromaSiting chroma_siting)
    {
        HDRChromaSiting eclipse_chroma_siting;
        switch (chroma_siting)
        {
        case UVAStreamConfig::ChromaSiting::TOP_COLLOCATED:
            eclipse_chroma_siting = chroma_siting_top_collocated;
            break;
        case UVAStreamConfig::ChromaSiting::HALF:
            eclipse_chroma_siting = chroma_siting_half;
            break;
        case UVAStreamConfig::ChromaSiting::UNSPECIFIED:
        default:
            eclipse_chroma_siting = chroma_siting_unspecified;
            break;
        }
        return eclipse_chroma_siting;
    }

    /**
     * @brief Convert UVAStreamConfig::ColorRange type to HDRColorRange
     * @return Return converted value
     */
    HDRColorRange convertColorRange(UVAStreamConfig::ColorRange color_range)
    {
        HDRColorRange eclipse_color_range;
        switch (color_range)
        {
        case UVAStreamConfig::ColorRange::LIMITED:
            eclipse_color_range = color_range_broadcast;
            break;
        case UVAStreamConfig::ColorRange::FULL_RANGE:
            eclipse_color_range = color_range_full_range;
            break;
        case UVAStreamConfig::ColorRange::DEFINED:
            eclipse_color_range = color_range_defined;
            break;
        case UVAStreamConfig::ColorRange::UNSPECIFIED:
        default:
            eclipse_color_range = color_range_unspecified;
            break;
        }
        return eclipse_color_range;
    }

    /**
     * @brief Convert UVAStreamConfig::TransferCharacteristics type to HDRTransferCharacteristics
     * @return Return converted value
     */
    HDRTransferCharacteristics convertTransferCharacteristics(
                                UVAStreamConfig::TransferCharacteristics transfer_characteristics)
    {
        HDRTransferCharacteristics eclipse_transfer_characteristics;
        switch (transfer_characteristics)
        {
        case UVAStreamConfig::TransferCharacteristics::ITURBT709:
           eclipse_transfer_characteristics = transfer_characteristics_iturbt709;
            break;
        case UVAStreamConfig::TransferCharacteristics::GAMMA2_2CURVE:
           eclipse_transfer_characteristics = transfer_characteristics_gamma2_2curve;
            break;
        case UVAStreamConfig::TransferCharacteristics::GAMMA2_8CURVE:
           eclipse_transfer_characteristics = transfer_characteristics_gamma2_8curve;
            break;
        case UVAStreamConfig::TransferCharacteristics::SMPTE170M:
           eclipse_transfer_characteristics = transfer_characteristics_smpte170m;
            break;
        case UVAStreamConfig::TransferCharacteristics::SMPTE240M:
           eclipse_transfer_characteristics = transfer_characteristics_smpte240m;
            break;
        case UVAStreamConfig::TransferCharacteristics::LINEAR:
           eclipse_transfer_characteristics = transfer_characteristics_linear;
            break;
        case UVAStreamConfig::TransferCharacteristics::LOG:
           eclipse_transfer_characteristics = transfer_characteristics_log;
            break;
        case UVAStreamConfig::TransferCharacteristics::LOG_SQRT:
           eclipse_transfer_characteristics = transfer_characteristics_log_sqrt;
            break;
        case UVAStreamConfig::TransferCharacteristics::IEC61966_2_4:
           eclipse_transfer_characteristics = transfer_characteristics_iec61966_2_4;
            break;
        case UVAStreamConfig::TransferCharacteristics::ITURBT1361:
           eclipse_transfer_characteristics = transfer_characteristics_iturbt1361;
            break;
        case UVAStreamConfig::TransferCharacteristics::IEC61966_2_1:
           eclipse_transfer_characteristics = transfer_characteristics_iec61966_2_1;
            break;
        case UVAStreamConfig::TransferCharacteristics::ITURBT2020_10:
           eclipse_transfer_characteristics = transfer_characteristics_iturbt2020_10;
            break;
        case UVAStreamConfig::TransferCharacteristics::ITURBT2020_12:
           eclipse_transfer_characteristics = transfer_characteristics_iturbt2020_12;
            break;
        case UVAStreamConfig::TransferCharacteristics::SMPTEST2084:
           eclipse_transfer_characteristics = transfer_characteristics_smptest2084;
            break;
        case UVAStreamConfig::TransferCharacteristics::SMPTEST428_1:
           eclipse_transfer_characteristics = transfer_characteristics_smptest428_1;
            break;
        case UVAStreamConfig::TransferCharacteristics::ARIBSTD_B67:
           eclipse_transfer_characteristics = transfer_characteristics_aribstd_b67;
            break;
        case UVAStreamConfig::TransferCharacteristics::UNSPECIFIED:
        default:
           eclipse_transfer_characteristics = transfer_characteristics_unspecified;
            break;
        }
        return eclipse_transfer_characteristics;
    }

    /**
     * @brief Convert UVAStreamConfig::ColorPrimaries type to HDRColorPrimaries
     * @return Return converted value
     */
    HDRColorPrimaries convertColorPrimaries(UVAStreamConfig::ColorPrimaries color_primaries)
    {
        HDRColorPrimaries eclipse_color_primaries;
        switch (color_primaries)
        {
        case UVAStreamConfig::ColorPrimaries::ITURBT709:
            eclipse_color_primaries = color_primaries_iturbt709;
            break;
        case UVAStreamConfig::ColorPrimaries::ITURBT470M:
            eclipse_color_primaries = color_primaries_iturbt470m;
            break;
        case UVAStreamConfig::ColorPrimaries::ITURBT470BG:
            eclipse_color_primaries = color_primaries_iturbt470bg;
            break;
        case UVAStreamConfig::ColorPrimaries::SMPTE170M:
            eclipse_color_primaries = color_primaries_smpte170m;
            break;
        case UVAStreamConfig::ColorPrimaries::SMPTE240M:
            eclipse_color_primaries = color_primaries_smpte240m;
            break;
        case UVAStreamConfig::ColorPrimaries::FILM:
            eclipse_color_primaries = color_primaries_film;
            break;
        case UVAStreamConfig::ColorPrimaries::ITURBT2020:
            eclipse_color_primaries = color_primaries_iturbt2020;
            break;
        case UVAStreamConfig::ColorPrimaries::SMPTEST428_1:
            eclipse_color_primaries = color_primaries_smptest428_1;
            break;
        case UVAStreamConfig::ColorPrimaries::JEDEC_P22:
            eclipse_color_primaries = color_primaries_jedec_p22;
            break;
        case UVAStreamConfig::ColorPrimaries::UNSPECIFIED:
        default:
            eclipse_color_primaries = color_primaries_unspecified;
            break;
        }
        return eclipse_color_primaries;
    }
};

OperaMseStreamData::OperaMseStreamData(UVADecoderBuffer* sample,
                                    UVADRMBackend* drm_backend,
                                    UVADRMBackend::UVADRMDecrypter* decrypter)
    : MseStreamData(sample->GetWritableData(),
                    sample->GetDataSize(),
                    sample->pts_sec,
                    sample->dts_sec,
                    sample->duration_sec)
    , m_native_sample(sample)
    , m_drm_backend(drm_backend)
    , m_decrypter(decrypter)
    , m_secure_decrypt(false)
{}

OperaMseStreamData::~OperaMseStreamData()
{
    TRACE_VERBOSE(("%s(): Removing sample\n", __FUNCTION__));

    if (m_encryption_info.m_key_id)
    {
        delete[] m_encryption_info.m_key_id;
    }
    
    if (m_encryption_info.m_iv)
    {
        delete[] m_encryption_info.m_iv;
    }
    
    std::vector<MseStreamData::SubSampleInfo *>::iterator it;   
    for (it = m_encryption_info.m_sub_samples.begin(); it != m_encryption_info.m_sub_samples.end(); ++it)
    {
        delete *it;
    }
    m_encryption_info.m_sub_samples.clear();

    delete m_native_sample;
}

OperaMseStreamData*
createOperaMseStreamData(UVADecoderBuffer* sample,
                         UVADRMBackend* drm_backend,
                         UVADRMBackend::UVADRMDecrypter* decrypter,
                         bool secure_decrypt)
{
    OperaMseStreamData* opera_mse_stream_data = 0;
    if (sample)
    {
        opera_mse_stream_data = new OperaMseStreamData(sample, drm_backend, decrypter);
        if (opera_mse_stream_data)
        {
            opera_mse_stream_data->setStreamConfigInfo();

            if (secure_decrypt)
            {
                opera_mse_stream_data->createEncryptionInfo();
            }
        }
    }
    return opera_mse_stream_data;
}

void
OperaMseStreamData::setStreamConfigInfo()
{
    UVAStreamConfig* stream_config = &(m_native_sample->config);

    m_stream_config.setStreamConfigCodecInfo((UVAStreamConfig::av(stream_config->codec) ==
                                                UVAStreamConfig::AUDIO_STREAM) ? audio_stream : video_stream,
                                              convertToEclipseCodec(stream_config->codec));

    if (stream_config->color_format)
    {
        m_stream_config.setHDRInfoExist(true);
        m_stream_config.setStreamConfigHDRInfo(
            stream_config->color_format->mastering_metadata->primary_r_chromaticity_x,
            stream_config->color_format->mastering_metadata->primary_r_chromaticity_y,
            stream_config->color_format->mastering_metadata->primary_g_chromaticity_x,
            stream_config->color_format->mastering_metadata->primary_g_chromaticity_y,
            stream_config->color_format->mastering_metadata->primary_b_chromaticity_x,
            stream_config->color_format->mastering_metadata->primary_b_chromaticity_y,
            stream_config->color_format->mastering_metadata->white_point_chromaticity_x,
            stream_config->color_format->mastering_metadata->white_point_chromaticity_y,
            stream_config->color_format->mastering_metadata->luminance_max,
            stream_config->color_format->mastering_metadata->luminance_min,
            convertMatrixCoefficients(stream_config->color_format->matrix_coefficients),
            static_cast<std::int64_t>(stream_config->color_format->bit_depth_luma),
            static_cast<std::int64_t>(stream_config->color_format->chroma_subsampling_horz),
            static_cast<std::int64_t>(stream_config->color_format->chroma_subsampling_vert),
            static_cast<std::int64_t>(stream_config->color_format->cb_subsampling_horz),
            static_cast<std::int64_t>(stream_config->color_format->cb_subsampling_vert),
            convertChromaSiting(stream_config->color_format->chroma_siting_horz),
            convertChromaSiting(stream_config->color_format->chroma_siting_vert),
            convertColorRange(stream_config->color_format->range),
            convertTransferCharacteristics(stream_config->color_format->transfer_characteristics),
            convertColorPrimaries(stream_config->color_format->primaries),
            static_cast<std::int64_t>(stream_config->color_format->max_cll),
            static_cast<std::int64_t>(stream_config->color_format->max_fall));
        TRACE_VERBOSE((" HDR info is set \n"));
    }
}

void
OperaMseStreamData::createEncryptionInfo()
{
    UVADecoderBuffer::SubSampleEncryptionInfo* data = NULL;
    UVADecoderBuffer::SubSampleEncryptionInfo* next = NULL;
    
    if (m_native_sample->GetKID())
    {
        m_encryption_info.m_key_id_length = 
            static_cast<std::uint32_t>(m_native_sample->GetKIDSize());
        m_encryption_info.m_key_id = new std::uint8_t[m_encryption_info.m_key_id_length];
        if (m_encryption_info.m_key_id)
        {
            memcpy((void*)m_encryption_info.m_key_id, (const void*)m_native_sample->GetKID(), m_encryption_info.m_key_id_length);
        }
        
        if (m_native_sample->GetIV())
        {
            m_encryption_info.m_iv_length =
                static_cast<std::uint32_t>(m_native_sample->GetIVSize());
            m_encryption_info.m_iv = new std::uint8_t[m_encryption_info.m_iv_length];
            if (m_encryption_info.m_iv)
            {
                memcpy((void*)m_encryption_info.m_iv, (const void*)m_native_sample->GetIV(), m_encryption_info.m_iv_length);
            }
        }

        size_t total = 0;
        data = m_native_sample->GetSubSampleDecryptionData();
        while (data)
        {
            next = data->next;
            
            SubSampleInfo *sample = new SubSampleInfo;
            sample->unencrypted_data_count =
                static_cast<std::uint32_t>(data->unencrypted_data_count);
            sample->encrypted_data_count =
                static_cast<std::uint32_t>(data->encrypted_data_count);
            total += data->unencrypted_data_count + data->encrypted_data_count;
            m_encryption_info.m_sub_samples.push_back(sample);
            
            delete data;
            data = next;
        }
        
        if (total != m_native_sample->GetDataSize())
        {
            TRACE_ERROR(("total[%zu] != m_native_sample->GetDataSize()[%zu]\n", total, m_native_sample->GetDataSize()));
        }
        
        m_secure_decrypt = true;  
    }
}

bool
OperaMseStreamData::decryptData()
{   
    bool decrypt_ok = false;

    // Data will be decrypted at low-level (e.g. TEE secure env.)
    if (m_secure_decrypt)
    {
        return false;
    }
    
    if (m_native_sample->GetKID() && m_drm_backend)
    {
        const unsigned char* iv = m_native_sample->GetIV();
        size_t iv_size = m_native_sample->GetIVSize();


        /* WARNING!
         *
         * This type of decryption is zero-copy, but is done on data cached in Opera.
         * Opera will not do anything with data in its decrypted nor encrypted form.
         * If ultra secure solution is required, do not do decryption on data pointed
         * to by GetWritableData(), instead make a copy.
         *
         */


        if (m_decrypter)
        {

            unsigned char* encrypted = m_native_sample->GetWritableData();
            UVADecoderBuffer::SubSampleEncryptionInfo* info = m_native_sample->GetSubSampleDecryptionData();
            UVADecoderBuffer::SubSampleEncryptionInfo* next = NULL;
            size_t total = 0;
            bool success = true;
            while (info)
            {
                next = info->next;
                encrypted += info->unencrypted_data_count;
                size_t decrypted = info->encrypted_data_count;
                if (decrypted)
                {
                    UVADRMBackend::UVADRMDecrypter::DECRYPT_STATUS decrypt_status = UVADRMBackend::UVADRMDecrypter::ERROR;

                    if (decrypt_status!= UVADRMBackend::UVADRMDecrypter::OK)
                    {
                        success = false;
                    }
                    else if (iv)
                    {
                        iv = NULL;
                        iv_size = 0;
                    }
                }

                if(info->encrypted_data_count != decrypted)
                {
                    TRACE_ERROR(("info->encrypted_data_count != decrypted"));
                    return decrypt_ok;
                }
                encrypted += info->encrypted_data_count;
                total += info->encrypted_data_count + info->unencrypted_data_count;
                delete info;
                info = next;
            }

            //If we do in-place decryption we might end up with half decrypted sample in case of errors
            //Since it will be invalid anyways, lets count on decoder to send us decode error

            if (success && total == m_native_sample->GetDataSize())
            {
                //Data was decrypted in place properly, notify Opera that buffer is no longer encrypted,
                //We might get same data later, this will prevent us from encoding it again as it will be treated as uncreated from now on.
                m_native_sample->SetUnencrypted();
            }
            // m_decrypter->decryptData(NULL, 0, <?>, <?>, NULL, 0); -- some decrypters might want finalization?
            if(total != m_native_sample->GetDataSize())
            {
                TRACE_ERROR(("total != m_native_sample->GetDataSize()\n"));
                return decrypt_ok;
            }
        }
        else
        {
            TRACE_ERROR(("NO DECRYPTOR!\n"));
            return decrypt_ok;
        }
    }
    else if (m_native_sample->GetKID())
    {
        TRACE_ERROR(("ENCRYPTED SAMPLE, NO DRM SET!\n"));
        return decrypt_ok;
    }
    else
    {
        TRACE_VERBOSE(("UNENCRYPTED SAMPLE!\n"));
    }
    decrypt_ok = true;
    return decrypt_ok;

}

void
OperaMseStreamData::setDrmType(int drm_type)
{
    m_drm_type = drm_type;
}


