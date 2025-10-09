/*
 * Copyright 2020 Vestel.  All rights reserved.
 *
 * @file
 * @brief Active list management for OperaWindowManager
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_WINDOWMANAGERACTIVELIST_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_WINDOWMANAGERACTIVELIST_HPP_INCLUDED

#include "utilities_public/String.hpp"
#include "nebula/browser_interface/KeyPressHandlingReason.hpp"
#include "nebula/browser_interface/NebulaKeyCodes.hpp"
#include <string>
#include <vector>

class AnyOperaWindow;
typedef std::vector<AnyOperaWindow *> VectorOfAnyOperaWindowPtr;

/**
 * @brief Functor for matching window name for use with STL algorithms.
 */
class NameEqual
{
public:
    NameEqual(cabot::String const & name_to_check);
    bool operator()(AnyOperaWindow * const & window) const;
private:
    cabot::String const & m_name_to_check;
};

/**
 * @brief Active list management for OperaWindowManager. The tail is the most active.
 */
class WindowManagerActiveList
{
public:
    /**
     * @brief constructor
     */
    WindowManagerActiveList() = default;

    /**
     * @brief Add a window to the active list. If the window is already in the active list then
     *        leave in its current position otherwise append the window to the list
     *        to make it the most active. (Does not active the window)
     * @param window is the window to add.
     */
    void insert(AnyOperaWindow* window);

    /**
     * @brief Remove a window from the active list.
     * @param window is the window to remove.
     */
    void remove(AnyOperaWindow* window);

    /**
     * @brief Sort the active list.
     */    
    void sortActiveList();

    /**
     * @brief Get top level active window.
     * @return pointer to active AnyOperaWindow.
     */
    AnyOperaWindow* getActiveWindow() const;


    VectorOfAnyOperaWindowPtr getActiveWindows() const;
    /**
     * @brief Get active window by name.
     * @param name of active window to return.
     * @return pointer to active AnyOperaWindow.
     */
    AnyOperaWindow* getActiveWindowByName(cabot::String const& name) const;

    /**
     * @brief Get active window that will handle key.
     * @param key_code is the key code to be handled.
     * @param keypress_handling_reason is the reason to be handled.
     * @return pointer to active AnyOperaWindow.
     */
    const AnyOperaWindow* findWindowToHandleKeyInput(
                                nebula::KeyCode key_code,
                                nebula::KeyPressHandlingReason keypress_handling_reason) const;
    /**
     * @brief Get active window for media components.
     * @return pointer to active AnyOperaWindow for media components.
     */
    AnyOperaWindow *getActiveWindowForComponents() const;

    /**
     * @brief Provide a string representation of this object.
     * @return A string representing the object.
     */
    std::string toString() const;

private:
    AnyOperaWindow* nextInLineActiveWindow() const;
    
private:
    VectorOfAnyOperaWindowPtr m_active_window_list;
};

#endif