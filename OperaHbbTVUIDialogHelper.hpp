/*
 * Copyright 2019 Vestel Electronics.  All rights reserved.
 *
 * @file 
 * @brief Acts as a helper class for interaction of HbbTV applications and main UI
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVUIDIALOGHELPER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVUIDIALOGHELPER_HPP_INCLUDED

#include "utilities_public/ExplodingBoolean.hpp"

class AnyWebBrowserUIController;
enum class DialogType;

namespace cabot
{
    class String;
}

class OperaHbbTVUIDialogHelper
{
public:
    
    /**
     * @brief Ctor for OperaHbbTVUIDialogHelper
     *
     */
    OperaHbbTVUIDialogHelper();

    /**
     * @brief Dtor for OperaHbbTVUIDialogHelper
     */
    ~OperaHbbTVUIDialogHelper();

    /**
     * @brief Initialise the dialog helper
     *
     * @param ui_controller Specifies the UI controller the helper uses to create UI prompts
     *
     * @return true if the helper was initialised successfully
     */
    ExplodingBoolean initialise(AnyWebBrowserUIController* ui_controller);


    /**
     * @brief Get user confirmation by showing the modal dialog
     *
     * @param confirm_payload that the user will be asked to validate
     * @param type of Dialog, for example Confirm or PreApproval
     * @return true if user gives confirmation, false in any other condition
     */
    bool confirm(cabot::String const& confirm_payload,
                 DialogType const type);
 
private:
    AnyWebBrowserUIController* m_ui_controller;

};

#endif //CABOT_VEWD_INTEGRATION_CORE_OPERAHBBTVUIDIALOGHELPER_HPP_INCLUDED
