/*
 * Copyright 2019 Vestel Electronics.  All rights reserved.
 *
 * @file
 * @brief Acts as a helper class for interaction of HbbTV applications and main UI
 */

#include "OperaHbbTVUIDialogHelper.hpp"

#include "eclipse/core/AnyResultCallback.hpp"

#include "nebula/browser_interface/AnyWebBrowserUIController.hpp"
#include "nebula/browser_interface/WebBrowserUIControllerDialogTypes.hpp"

#include "frost/frost_semaphore.h"

#include "utilities_public/String.hpp"

#include "utilities_debug/unused.h"
#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/ui_dialog_helper");


// A simple success/failed blocking callback used to retrieve the 
// result of a confirm dialog modal operation
class ConfirmDialogResult : public AnyResultCallback
{
public:
    ConfirmDialogResult();
    ~ConfirmDialogResult();

    bool initialise();

    void failed() override;
    void success() override;
    void addParam(const char* param) override;
    void getParam(cabot::String& param) const override;
    bool getResult() const;

private: // copy operations disallowed
    ConfirmDialogResult(ConfirmDialogResult const&); // not to be implemented
    ConfirmDialogResult& operator=(ConfirmDialogResult const&); // not to be implemented

private:    
    bool m_result;
    FROST_Semaphore* m_sema;
    cabot::String m_string_param;
};

ConfirmDialogResult::ConfirmDialogResult()
    : AnyResultCallback()
    , m_result(false)
    , m_sema(0)
{
}

ConfirmDialogResult::~ConfirmDialogResult()
{
    if (m_sema)
    {
        FROST_SemaphoreDestroy(m_sema);
    }
}

bool ConfirmDialogResult::initialise()
{
    if (!m_sema)
    {
        m_sema = FROST_SemaphoreCreate(0);
    }

    return m_sema ? true : false;
}

void ConfirmDialogResult::success()
{
    m_result = true;
    FROST_SemaphoreSignal(m_sema);
}

void ConfirmDialogResult::failed()
{
    m_result = false;
    FROST_SemaphoreSignal(m_sema);
}

bool ConfirmDialogResult::getResult() const
{
    FROST_SemaphoreWait(m_sema);
    return m_result;
}

void ConfirmDialogResult::addParam(const char* param)
{
    m_string_param = param;
}

void ConfirmDialogResult::getParam(cabot::String& param) const
{
    param = m_string_param;
}

namespace
{
    bool getConfirmResult(AnyWebBrowserUIController& ui_controller, 
                          DialogType const type,
                          ConfirmDialogResult& dialog_result)
    {
        if (dialog_result.initialise())
        {
            ui_controller.doModalDialog(type, &dialog_result);
            // This call will block the current thread until the modal operation is finished
            return dialog_result.getResult();
        }

        return false;
    }
}

OperaHbbTVUIDialogHelper::OperaHbbTVUIDialogHelper()
    : m_ui_controller(nullptr)
{
}

OperaHbbTVUIDialogHelper::~OperaHbbTVUIDialogHelper()
{
}

ExplodingBoolean
OperaHbbTVUIDialogHelper::initialise(AnyWebBrowserUIController* ui_controller)
{
    m_ui_controller = ui_controller;
    
    return ExplodingBoolean(m_ui_controller != nullptr);
}

bool
OperaHbbTVUIDialogHelper::confirm(cabot::String const& confirm_payload, 
                                  DialogType const type)
{
    ConfirmDialogResult result;
    result.addParam(confirm_payload.charArray());
    return getConfirmResult(*m_ui_controller, type, result);
}

