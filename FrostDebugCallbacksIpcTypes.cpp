/**
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implementation for IPC function names and additional Nebula IPC Types for
 *        frost debug callbacks.
 */

#include "FrostDebugCallbacksIpcTypes.hpp"

MP_FROST_RegisterCommandCallbackData::MP_FROST_RegisterCommandCallbackData(
                            FROST_DebugParamType param_type, const void* param)
    :   m_param_type(param_type),
        m_param_null(param == nullptr),
        m_string_sub_param_null(false)
{
    if (!m_param_null)
    {
        switch (param_type)
        {
        default:
            break;
        case FROST_debug_param_int:
            m_int = *static_cast<const int*>(param);
            break;
        case FROST_debug_param_string_int:
            {
                const auto* p = static_cast<const FROST_DebugParamStringInt*>(param);
                m_int = static_cast<int>(p->int_part);
                m_string_sub_param_null = (p->string_part == nullptr);
                if (!m_string_sub_param_null)
                {
                    m_string = static_cast<const char*>(p->string_part);
                }
                break;
            }
        case FROST_debug_param_string:
        case FROST_debug_param_string_none:
            m_string = static_cast<const char*>(param);
            break;
        }
    }
}