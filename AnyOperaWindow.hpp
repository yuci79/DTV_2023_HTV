/*
 * Copyright 2020 Vestel.  All rights reserved.
 *
 * @file
 * @brief This file defines the AnyOperaWindow interface.
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_ANYOPERAWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_ANYOPERAWINDOW_HPP_INCLUDED

#include "nebula/browser_interface/KeyPressHandlingReason.hpp"
#include "nebula/browser_interface/NebulaKeyCodes.hpp"
#include "utilities_public/String.hpp"

/**
 * @brief Interface common to all browser windows.
 * Initially only contains functions required for unit testing.
 */
class AnyOperaWindow
{
public:
    /**
     * @brief Indicates whether the window has focus.
     * @return true if the window has focus.
     */
    virtual bool isFocused() const = 0;

    /**
     * @brief Indicates whether the window is focusable.
     * @return true if the window is focusable.
     */
    virtual bool isFocusable() const = 0;

    /**
     * @brief Indicates whether the window is valid.
     * @return true if the window is valid.
     */    
    virtual bool isValid() const = 0;

    /**
     * @brief The window name.
     * @return the window name.
     */    
    virtual cabot::String name() const = 0;

    /**
     * @brief Indicates whether the window should handle the key.
     * @param key_code is the key code to be handled.
     * @param keypress_handling_reason is the reason to be handled.
     * @return true if the window should handle the key.
     */    
    virtual bool shouldHandleKey(
                        nebula::KeyCode key_code,
                        nebula::KeyPressHandlingReason keypress_handling_reason) const = 0;

    /**
     * @brief Indicates whether the window allows broadcast media.
     * @return true if the window allows broadcast media.
     */    
    virtual bool broadbandMediaAllowed() const = 0;
};

#endif
