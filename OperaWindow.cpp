/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaWindow.
 */
#include "OperaWindow.hpp"
#include "OperaWindowManager.hpp"
#include "vewd_integration/helper/OperaSdk4Utilities.hpp"
#include "vewd_integration/mumin/OperaMuminHelper.hpp"
#include <omi/messages/CreateHbbTVWindow.h>
#include <omi/messages/CreateBrowserUIWindow.h>
#include <omi/messages/CreateWindow.h>
#include <omi/messages/DestroyWindow.h>
#include <omi/messages/LoadURL.h>
#include <omi/messages/Show.h>
#include <omi/messages/Hide.h>
#include <omi/messages/SetPref.h>
#include <omi/messages/GetPref.h>
#include <omi/messages/MoveInHistory.h>
#include <omi/messages/StackAtTop.h>
#include <omi/messages/StackAtBottom.h>
#include <omi/messages/StackAbove.h>
#include <omi/messages/StackBelow.h>
#include <omi/messages/GetHistoryCount.h>
#include <omi/messages/GetCurrentHistoryIndex.h>
#include <omi/messages/SetBounds.h>
#include "OperaEngine.hpp"
#include "../helper/OperaKeyHelper.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_public/UString.hpp"
#include "profile_helper/ProfileHelper.h"

#include <stdlib.h>

TRACE_IMPLEMENT("opera/opera_window");
#include "vewd_integration/helper/OperaSdk4Utilities.inl"

const char* OperaWindow::window_name_browser_ui = "BROWSERUI";
const char* OperaWindow::window_name_csp = "DEFAULT_CSP";
const char* OperaWindow::window_name_hbbtv = "HBBTV";
const char* OperaWindow::window_name_tv_store = "TVSTORE";
const char* OperaWindow::window_name_ui = "UI";
const char* OperaWindow::window_name_opapp = "OPAPP";
const char* OperaWindow::window_name_oipf = "OIPF";
const char* OperaWindow::window_name_launcher = "LAUNCHER";
const char* OperaWindow::window_name_overlay = "OVERLAY";
const char* OperaWindow::window_name_media_player = "MEDIA_PLAYER";

OperaWindow::OperaWindow(OperaEngine& opera_engine, cabot::String window_name, int event_dispatch_priority)
    : m_opera_engine(opera_engine)
    , m_current_url("")
    , m_window_handle(OperaWindowHandle(-1))
    , m_window_name(window_name)
    , m_active(false)
    , m_visible(false)
    , m_focused(false)
    , m_focusable(true)
    , m_event_dispatch_priority(event_dispatch_priority)
    , m_is_txt_app_running(false)
    , m_nebula_window_rect({0, 0, 0, 0})
    , m_nebula_window_rect_set(false)
{
}

OperaWindow::~OperaWindow()
{}

OperaWindowHandle
OperaWindow::handle() const
{
    return m_window_handle;
}

cabot::String
OperaWindow::name() const
{
    return m_window_name;
}

void
OperaWindow::setEventDispatchPriority(int event_dispatch_priority)
{
    // In order to push the window to the end of list (sorted according to the priorities);
    // set event dispatch priority to highest value, if negative or too lower of event dispatch
    // priority is set.
    if ((event_dispatch_priority < lowest_event_dispatch_priority_value) ||
        (event_dispatch_priority > highest_event_dispatch_priority_value))
    {
        m_event_dispatch_priority = highest_event_dispatch_priority_value;
    }
    else
    {
        m_event_dispatch_priority = event_dispatch_priority;
    }
}

int
OperaWindow::getEventDispatchPriority() const
{
    return m_event_dispatch_priority;
}

void
OperaWindow::setKeySet(int keyset_value, const std::vector<int>& other_keys)
{
    TRACE_INFO(("setKeySet for window[%s][%#x - %s]\n",
                    m_window_handle.str().c_str(), keyset_value,
                    keysetToString(keyset_value).c_str()));

    m_keyset.keyset_value = keyset_value;
    m_keyset.other_keys.clear();

    const bool other_keys_flag = m_keyset.keyset_value & keyset_other;
    const bool contains_other_keys = !other_keys.empty();

    if (other_keys_flag && !contains_other_keys)
    {
        TRACE_WARN(("OTHER bit set - other_keys empty\n"));
    }
    else if (!other_keys_flag && contains_other_keys)
    {
        TRACE_WARN(("OTHER bit not set - ignoring all other_keys\n"));
    }
    else if (other_keys_flag)
    {
        for (unsigned int i = 0; i < other_keys.size(); ++i)
        {
            const int key = other_keys[i];
            const omi::common::Key input_key = static_cast<omi::common::Key>(key);
            const nebula::KeyCode nebula_key_code = operaKeyToNebulaKey(input_key);

            if (nebula_key_code != nebula::KeyUnknown)
            {
                m_keyset.other_keys.push_back(nebula_key_code);
                TRACE_VERBOSE(("other_keys[%u] - %s[%d] -> nebula key[%d]\n",
                                i, omi::common::ToString(input_key).c_str(), static_cast<int>(input_key),
                                nebula_key_code));
            }
            else
            {
                TRACE_WARN(("other_keys[%u] - %s[%d] - ignoring unknown nebula key!\n",
                            i, omi::common::ToString(input_key).c_str(), static_cast<int>(input_key)));
            }
        }
    }
}

void
OperaWindow::getKeySet(int* keyset_value, std::vector<int>* other_keys) const
{
    *keyset_value = m_keyset.keyset_value;
    other_keys->clear();
    for (unsigned int i = 0; i < m_keyset.other_keys.size(); i++)
    {
        other_keys->push_back(
            static_cast<int>(
                nebulaKeyToOperaKey(static_cast<nebula::KeyCode>(m_keyset.other_keys[i]))));
    }
}

bool
OperaWindow::shouldHandleKey(nebula::KeyCode key_code,
                             nebula::KeyPressHandlingReason keypress_handling_reason) const
{
    bool result = false;

    if (keypress_handling_reason == nebula::KeyPressHandlingReason::pre_terminal_ui_handling)
    {
        result = isKeyInKeySet(key_code);
    }

    TRACE_VERBOSE(("shouldHandleKey[%s](key_code = %d, reason = %s) = %d\n",
                    m_window_name.charArray(),
                    key_code, toString(keypress_handling_reason),
                    result));

    return result;
}

bool
OperaWindow::isKeyInKeySet(nebula::KeyCode key_code) const
{
    bool result = false;

    TRACE_INFO(("isKeyInKeySet[%s](key_code = %d)\n",
                m_window_name.charArray(),
                key_code));

    if (::isKeyInKeySet(key_code, m_keyset.keyset_value))
    {
        TRACE_VERBOSE(("key_code[%d] is in keyset\n", key_code));
        result = true;
    }

    if (!result &&
         (m_keyset.keyset_value & keyset_other) != 0 &&
         !(m_keyset.other_keys.empty()))
    {
        // check other keys
        for (unsigned int i=0; i < m_keyset.other_keys.size(); i++)
        {
            if (m_keyset.other_keys[i] == key_code)
            {
                TRACE_VERBOSE(("key_code[%d] found in other keys\n", key_code));
                result = true;
                break;
            }
        }
    }

    return result;
}

void
OperaWindow::setVisible(bool visible)
{
    TRACE_INFO(("Making window %s %svisible\n", handle().str().c_str(), visible ? "": "in"));
    if(m_window_handle.IsValid())
    {
         if (visible)
        {
            m_opera_engine.post(omi::msg::Show(m_window_handle));
        }
        else
        {
            m_opera_engine.post(omi::msg::Hide(m_window_handle));
        }
    }
    m_visible = visible;
}

void
OperaWindow::setFocus(bool focus)
{
    TRACE_INFO(("setFocus(%s: %d)\n", handle().str().c_str(), focus));
    m_focused = focus;
}

bool
OperaWindow::isFocused() const
{
    return m_focused;
}

void
OperaWindow::setFocusable(bool focus_allowed)
{
    TRACE_INFO(("setFocusable(%s: %d)\n", handle().str().c_str(), focus_allowed));
    m_focusable = focus_allowed;
}

bool
OperaWindow::isFocusable() const
{
    return m_focusable;
}

void
OperaWindow::reset(bool clear_keyset)
{
    TRACE_INFO(("reset(clear_keyset = %d)\n", clear_keyset));

    m_current_url.clear();
    m_active = false;
    m_visible = false;
    m_is_txt_app_running = false;
    if (clear_keyset)
    {
        clearKeyset();
    }
}

void
OperaWindow::setTeletextAppRunning(bool teletext_app)
{
    m_is_txt_app_running = teletext_app;
}

bool
OperaWindow::isTeletextAppRunning() const
{
    return m_is_txt_app_running;
}

void
OperaWindow::setActive(bool active)
{
    TRACE_INFO(("setActive(%s: %d)\n", handle().str().c_str(), active));
    //If an Hbbtv app is active, it should be focused
    if (active)
    {
        setFocus(true);
    }
    m_active = active;
}

bool
OperaWindow::sendToBack()
{
    using::omi::msg::StackAtBottom;
    //StackAtBottom::Result result = m_opera_engine.post(StackAtBottom(m_window_handle)).get();
    //return checkResult(result);
    m_opera_engine.post(StackAtBottom(m_window_handle));
    return true;
    
}

bool
OperaWindow::bringToFront()
{
    using::omi::msg::StackAtTop;
    //StackAtTop::Result result = m_opera_engine.post(StackAtTop(m_window_handle)).get();
    //return checkResult(result);
    m_opera_engine.post(StackAtTop(m_window_handle));
    return true;
    
}

bool
OperaWindow::moveAbove(OperaWindowHandle const& reference_window_handle) const
{
    using::omi::msg::StackAbove;
    //StackAbove::Result result = m_opera_engine.post(StackAbove(m_window_handle, reference_window_handle)).get();
    //return checkResult(result);
    m_opera_engine.post(StackAbove(m_window_handle, reference_window_handle));
    return true;    
}

bool
OperaWindow::moveBelow(OperaWindowHandle const& reference_window_handle) const
{
    using::omi::msg::StackBelow;
    //StackBelow::Result result = m_opera_engine.post(StackBelow(m_window_handle, reference_window_handle)).get();
    //return checkResult(result);
    m_opera_engine.post(StackBelow(m_window_handle, reference_window_handle));
    return true;
    
}

bool
OperaWindow::setPreferenceValue(UString const& key, UString const& value)
{
    using omi::msg::SetPref;

    bool retval = false;
    TRACE_INFO(("setPreferenceValue(handle=%p, key=[%s], value=[%s]\n", m_window_handle.str().c_str(), key.charArray(), value.charArray()));
    SetPref::Result result = m_opera_engine.post(
        SetPref(m_window_handle, key.charArray(), value.charArray())).get();

    switch (result.error)
    {
    case SetPref::KEY_NOT_FOUND:
        TRACE_ERROR(("%s pref does not exist\n", key.charArray()));
        break;
    case SetPref::INVALID_VALUE:
        TRACE_ERROR(("Preference value %s invalid for %s\n", value.charArray(), key.charArray()));
        break;
    case SetPref::INVALID_WINDOW:
        TRACE_ERROR(("Invalid window handle for %s preference and window %s\n", key.charArray(), m_window_name.charArray()));
        break;
    case SetPref::READ_ONLY:
        TRACE_ERROR(("The preference %s is read only\n", key.charArray()));
        break;
    case SetPref::NO_ERR:
        TRACE_INFO(("Preference %s is set to %s\n", key.charArray(), value.charArray()));
        retval = true;
        break;
    default:
        TRACE_ERROR(("Unknown result in setPreference for %s, error %d (%s)\n", key.charArray(),
                        result.error, result.errorString().c_str()));
        break;
    }
    return retval;
}

bool
OperaWindow::getPreferenceValue(UString const& key, UString& value)
{
    using omi::msg::GetPref;
    GetPref::Result result = m_opera_engine.post(GetPref(m_window_handle, key.charArray())).get();
    bool retval = false;
    if (checkResult(result))
    {
        value = result.value.c_str();
        retval = true;
    }
    return retval;
}

bool
OperaWindow::moveInHistory(int steps)
{
    using omi::msg::MoveInHistory;

    unsigned step_count = steps < 0 ? -steps : steps;
    MoveInHistory::Direction direction = steps < 0 ?
            MoveInHistory::HISTORY_BACK : MoveInHistory::HISTORY_FORWARD;

    TRACE_INFO(("Moving %d steps in history\n", steps));

    // move in history for current app, we might need to check app type
    //MoveInHistory::Result result =  m_opera_engine.post(MoveInHistory(m_window_handle, direction, step_count)).get();

    //return checkResult(result);
    m_opera_engine.post(MoveInHistory(m_window_handle, direction, step_count));

    return true;    
}

int
OperaWindow::getHistoryCount()
{
    using omi::msg::GetHistoryCount;

    GetHistoryCount::Result result =  m_opera_engine.post(GetHistoryCount(m_window_handle)).get();
    int count = 0;
    if (checkResult(result))
    {
        count = result.count;
    }

    return count;
}

int
OperaWindow::getCurrentHistoryIndex()
{
    using omi::msg::GetCurrentHistoryIndex;

    GetCurrentHistoryIndex::Result result =  m_opera_engine.post(GetCurrentHistoryIndex(m_window_handle)).get();
    int index = 0;
    if (checkResult(result))
    {
        index = result.index;
    }

    return index;
}

bool
OperaWindow::isVisible() const
{
    return m_visible;
}

bool
OperaWindow::isActive() const
{
    return m_active;
}

bool
OperaWindow::isValid() const
{
    return m_window_handle.IsValid();
}

void
OperaWindow::setRenderingResolution(RenderingResolution resolution)
{
    UString resolution_str = "1280x720";
    if (resolution == rendering_resolution_1920x1080)
    {
        resolution_str = "1920x1080";
    }
    if (Profile_GetB2BProduct() != Profile_NormalTV)
    {
        char content[11] = {'\0'};
        if (getFileContent("/conf/browser/content_resolution.txt", content))
        {
            resolution_str = content;
        }
        else if (Profile_GetContentResolution() == 1)
        {
            resolution_str = "1920x1080";
        }

        if (this->name() == "HBBTV")
        {
            resolution_str = "1920x1080";
            
        }
    }
    setPreferenceValue("content_resolution", resolution_str);
}

void
OperaWindow::setRenderingResolution(int width, int height)
{
    const UString resolution_str = UString::fromInt(width) + UString("x") + UString::fromInt(height);
    setPreferenceValue("content_resolution", resolution_str);
}

cabot::String const&
OperaWindow::getCurrentPageUrl() const
{
    return m_current_url;
}

void
OperaWindow::setCurrentPageUrl(cabot::String const& url)
{
    m_current_url = url;
}

void OperaWindow::clearKeyset()
{
    TRACE_INFO(("clearKeyset()\n"));

    m_keyset.keyset_value = 0;
    m_keyset.other_keys.clear();
}

void
OperaWindow::updateUserAgentPreference()
{
    setPreferenceValue("user_agent", m_opera_engine.userAgentString().charArray());
}

void OperaWindow::setDefaultUserAgentString()
{
    TRACE_INFO(("User agent is %s\n", m_opera_engine.userAgentString().charArray()));
    m_opera_engine.updateUserAgent();
    updateUserAgentPreference();
}

void OperaWindow::setTikTokUserAgentString()
{
    m_opera_engine.updateUserAgent();
    cabot::String ua = m_opera_engine.userAgentString();
    ua.replaceAll("Linux","Linux; Tizen 5.5");
    setPreferenceValue("user_agent", ua.charArray());
}

void OperaWindow::updateOperaInputMethod()
{
    bool is_usb_keyboard_plugged = m_opera_engine.getWindowManager()->isUsbKeyboardPlugged();

    UString input_method;
    if (!is_usb_keyboard_plugged)
    {
        TRACE_INFO(("Setting Opera Keyboard Input Method to Virtual Keyboard.\n"));
        input_method = "virtual_keyboard";
    }
    else
    {
        TRACE_INFO(("Setting Opera Keyboard Input Method to Omi Events.\n"));
        input_method = "omi_events";
    }
    setPreferenceValue("input_method", input_method);
}

bool
OperaWindow::getFileContent(const char* file_path, char *content)
{
    int length;
    FILE* local_file;
    if ((local_file = fopen(file_path, "r")) != NULL)
    {
        if (fgets(content, 255, local_file) == NULL)
        {
            TRACE_ERROR(("OperaWindow : %s file could not be retrieved\n", file_path));
            fclose(local_file);
            return false;
        }

        length = strlen(content) - 1;

        if (content[length] == '\n')
        {
            content[length]='\0';
        }

        fclose(local_file);
        return true;
    }
    else
    {
        TRACE_ERROR(("OperaWindow : %s file could not be accessed\n", file_path));
        return false;
    }
}

bool
OperaWindow::broadbandMediaAllowed() const
{
    return true;
}

bool
OperaWindow::getPosition(double& x,
                         double& y,
                         double& width,
                         double& height) const
{
    TRACE_INFO(("%s()\n", __func__));

    if (m_nebula_window_rect_set)
    {
        x = m_nebula_window_rect.x_position;
        y = m_nebula_window_rect.y_position;
        width = m_nebula_window_rect.width;
        height = m_nebula_window_rect.height;

        return true;
    }

    return false;
}

bool
OperaWindow::setPosition(double x,
                         double y,
                         double width,
                         double height)
{
    TRACE_INFO(("%s(): nebula: x=%f, y=%f, width=%f, height=%f\n", __func__, x, y, width, height));

    int opera_x = 0;
    int opera_y = 0;
    int opera_width = 0;
    int opera_height = 0;

    nebulaWindowToOperaRect(x,
                            y,
                            width,
                            height,
                            opera_x,
                            opera_y,
                            opera_width,
                            opera_height);
                               
    TRACE_INFO(("%s(): opera: x=%d, y=%d, width=%d, height=%d\n",
                __func__,
                opera_x,
                opera_y,
                opera_width,
                opera_height));

    const omi::Rect opera_bounds(opera_x,
                                opera_y,
                                opera_width,
                                opera_height);

    const auto result = m_opera_engine.post(omi::msg::SetBounds(handle(),
                                            opera_bounds)).get();
    if (checkResult(result))
    {
        TRACE_VERBOSE(("Bounds set\n"));

        m_nebula_window_rect.x_position = x;
        m_nebula_window_rect.y_position = y;
        m_nebula_window_rect.width = width;
        m_nebula_window_rect.height = height;
        m_nebula_window_rect_set = true;
            
        setRenderingResolution(opera_width, opera_height);

        return true;
    }

    TRACE_ERROR(("Unable to set bounds\n"));

    return false;
}