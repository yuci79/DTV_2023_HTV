/*
 * Copyright 2020 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the WindowManagerActiveList class.
 */

#include "vewd_integration/core/WindowManagerActiveList.hpp"
#include "vewd_integration/core/AnyOperaWindow.hpp"
#include "vewd_integration/core/OperaWindow.hpp"
#include "utilities_debug/trace.h"
#include <algorithm>
#include <sstream>

TRACE_IMPLEMENT("opera/window_man_active_list");

NameEqual::NameEqual(cabot::String const & name_to_check)
    : m_name_to_check(name_to_check)
{
}

bool
NameEqual::operator()(AnyOperaWindow * const & window) const
{
    return m_name_to_check == window->name();
}

void
WindowManagerActiveList::insert(AnyOperaWindow* window)
{
    bool exists = false;
    for (auto it =  m_active_window_list.begin(); it != m_active_window_list.end(); ++it)
    {
        if ((*it) == window)
        {
            exists = true;
            break;
        }
    }

    if (!exists)
    {
        m_active_window_list.push_back(window);
    }
}

const AnyOperaWindow*
WindowManagerActiveList::findWindowToHandleKeyInput(
                                nebula::KeyCode key_code,
                                nebula::KeyPressHandlingReason keypress_handling_reason) const
{
    const AnyOperaWindow* window = nullptr;

    for (int i = (m_active_window_list.size() - 1); i >= 0; --i)
    {
        if (m_active_window_list[i]->shouldHandleKey(key_code, keypress_handling_reason))
        {
            window = m_active_window_list[i];
            break;
        }
    }

    return window;
}

void
WindowManagerActiveList::sortActiveList()
{
    TRACE_INFO(("sortActiveList()\n"));

    // Ensure the relative order of the operator application and regular HbbTV
    // windows is correct.
    auto opapp_pos = std::find_if(m_active_window_list.begin(),
                                  m_active_window_list.end(),
                                  NameEqual(OperaWindow::window_name_opapp));
    auto hbbtv_pos = std::find_if(m_active_window_list.begin(),
                                  m_active_window_list.end(),
                                  NameEqual(OperaWindow::window_name_hbbtv));

    if (opapp_pos != m_active_window_list.end() &&
        hbbtv_pos != m_active_window_list.end())
    {
        bool const opapp_focusable = (*opapp_pos)->isFocusable();
        if ((opapp_focusable && opapp_pos < hbbtv_pos) ||
            (!opapp_focusable && opapp_pos > hbbtv_pos))
        {
            std::iter_swap(opapp_pos, hbbtv_pos);
        }
    }

    // Ensure the relative order of the Launcher and regular HbbTV
    // windows is correct after order done for opapp and HbbTv windows
    auto launcher_pos = std::find_if(m_active_window_list.begin(),
                                  m_active_window_list.end(),
                                  NameEqual(OperaWindow::window_name_launcher));

    // Get latest position of hbbtv window
    hbbtv_pos = std::find_if(m_active_window_list.begin(),
                                  m_active_window_list.end(),
                                  NameEqual(OperaWindow::window_name_hbbtv));

    if (launcher_pos  != m_active_window_list.end() &&
        hbbtv_pos != m_active_window_list.end())
    {
        bool const launcher_focusable = (*launcher_pos)->isFocusable();
        if ((launcher_focusable && launcher_pos < hbbtv_pos) ||
            (!launcher_focusable && launcher_pos > hbbtv_pos))
        {
            std::iter_swap(launcher_pos, hbbtv_pos);
        }
    }
}

void
WindowManagerActiveList::remove(AnyOperaWindow* window)
{
    for (auto it = m_active_window_list.begin(); it != m_active_window_list.end(); ++it)
    {
        if ((*it) == window)
        {
            m_active_window_list.erase(it);
            break;
        }
    }
}

AnyOperaWindow*
WindowManagerActiveList::getActiveWindow() const
{
    if (!m_active_window_list.empty() && m_active_window_list.back()->isValid())
    {
        TRACE_VERBOSE(("%s(): Returning active window: %s\n",
                       __func__,
                       m_active_window_list.back()->name().charArray()));
        return m_active_window_list.back();
    }
    else
    {
        return nullptr;
    }
}

VectorOfAnyOperaWindowPtr
WindowManagerActiveList::getActiveWindows() const
{
    VectorOfAnyOperaWindowPtr active_windows;

    std::copy_if(
        m_active_window_list.begin(), m_active_window_list.end(),
             std::back_inserter(active_windows),
             [](AnyOperaWindow* opera_window)
             { 
                 return (opera_window && opera_window->isValid()); 
             });

    return active_windows;
}

AnyOperaWindow*
WindowManagerActiveList::getActiveWindowByName(cabot::String const& name) const
{
    if (!m_active_window_list.empty())
    {
        for (unsigned int i = 0; i<m_active_window_list.size(); i++)
        {
            if ((m_active_window_list[i]->name() == name) && m_active_window_list[i]->isValid())
            {
                return m_active_window_list[i];
            }
        }
    }
    return nullptr;
}

AnyOperaWindow*
WindowManagerActiveList::nextInLineActiveWindow() const
{
    AnyOperaWindow* next_in_line_active_window = nullptr;
    if (m_active_window_list.size() > 1)
    {
        auto window = m_active_window_list.at(m_active_window_list.size() - 2);
        if (window->isValid())
        {
            next_in_line_active_window = window;
        }
    }
    return next_in_line_active_window;
}

AnyOperaWindow *
WindowManagerActiveList::getActiveWindowForComponents() const
{
    bool broadband_media_allowed = true;
    const auto active_window = getActiveWindow();
    AnyOperaWindow* active_window_for_components = active_window;
    if (active_window)
    {
        auto active_opapp_window = getActiveWindowByName(OperaWindow::window_name_opapp);
        if (active_opapp_window)
        {
            auto active_hbbtv_window = getActiveWindowByName(OperaWindow::window_name_hbbtv);
            broadband_media_allowed = active_opapp_window->broadbandMediaAllowed();
            if (active_opapp_window == active_window)
            {
                /* [TS 103 606, 9.1.3]
                 * "While the operator application is in the background, transient or
                 *  overlaid transient state, any attempts by the operator application
                 *  to start presentation of broadband delivered media shall fail."
                 * 
                 * So if the opapp is the current active window and is in one of those states
                 * it is not considered the active window for component queries. Instead use
                 * either active hbbTV window or the next most active window in line.
                 */
                if (active_opapp_window->isFocused() && !broadband_media_allowed)
                {
                    if (active_hbbtv_window)
                    {
                        active_window_for_components = active_hbbtv_window;
                    }
                    else
                    {
                        active_window_for_components = nextInLineActiveWindow();
                    }
                }
            }
            else if (active_hbbtv_window && (active_hbbtv_window == active_window))
            {
                /* [TS 103 606, 9.1.3]
                 * "While the operator application is in the foreground or overlaid foreground
                 *  state, it shall be able to control media decoders..."
                 * 
                 * So if the opapp is not the currently active window but is in one of those
                 * states then it should be the active window for component queries.
                 */
                if (!active_opapp_window->isFocused() && broadband_media_allowed)
                {
                    active_window_for_components = active_opapp_window;
                }
            }
        }
    }
    
    TRACE_INFO(("%s(): Active window: %s, "
            "Active window for components: %s, "
            "Broadband media allowed: %s\n",
            __func__,
            active_window ? active_window->name().charArray() : "",
            active_window_for_components ? active_window_for_components->name().charArray() : "",
            broadband_media_allowed ? "true" : "false"));

    return active_window_for_components;
}

std::string
WindowManagerActiveList::toString() const
{
    std::stringstream ss;
    for (auto const & window : m_active_window_list)
    {
        ss << window->name().c_str() << ", ";
    }
    return ss.str();
}
