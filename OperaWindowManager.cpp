/*
 * Copyright 2014 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaWindowManager class.
 */
#include "config/include_hybrid_ui.h"
#include "config/include_opapp_support.h"
#include "config/nflx5t.h"
#include "config/include_avs_mmsdk.h"
#include "config/include_hoteltv.h"
#include "frost/frost_debug.h"

#include "OperaWindowManager.hpp"

#include "OperaWindow.hpp"
#include "OperaHbbTVWindow.hpp"
#include "OperaCspWindow.hpp"
#ifdef USE_MEDIA_PLAYER_WINDOW
#include "OperaMediaPlayerWindow.hpp"
#else
#include "OperaDashPlayerWindow.hpp"
#endif
#include "OperaBrowserWindow.hpp"
#include "OperaTVStoreWindow.hpp"
#ifdef INCLUDE_OPAPP_SUPPORT
#include "vewd_integration/opapp/OperaOpAppWindow.hpp"
#endif
#include "OperaOipfWindow.hpp"
#include "nebula/adaptation_layer/nebula_application_capability.h"
#include "nebula/adaptation_layer/nebula_picture_settings.h"
#include "OperaBrowserUIWindow.hpp"
#include "OperaLauncherWindow.hpp"
#include "../helper/OperaKeyHelper.hpp"
#include "vewd_common/bpi/bpi_number_key_handler.hpp"
#include "vewd_common/bpi/bpi_window_manager.hpp"
#include <omi/events/WindowFocusChanged.h>
#include <omi/messages/Blur.h>
#include <omi/messages/Focus.h>
#include <omi/messages/LoadURL.h>
#include <omi/messages/Show.h>
#include "utilities_public/Sort.hpp"
#include "utilities_public/UnderlyingType.hpp"
#include "utilities_public/UString.hpp"
#include "utilities_debug/cabot_assert.h"
#include "utilities_debug/trace.h"
#include "utilities_debug/unused.h"

#include "profile_helper/ProfileHelper.h"
#include <sstream>
#include <algorithm>

TRACE_IMPLEMENT("opera/window_man");

namespace
{

NamedComparer(ComparerOfOperaWindowPtr, OperaWindow*);
DEFINE_CABOT_SORT_VECTOR_NAMED(VectorOfOperaWindowPtr, ComparerOfOperaWindowPtr);

class PriorityComparer : public ComparerOfOperaWindowPtr
{
public:
    virtual bool less(OperaWindow* const &left, OperaWindow* const &right) const;
};

bool PriorityComparer::less(OperaWindow* const &left, OperaWindow* const &right) const
{
    return left->getEventDispatchPriority() < right->getEventDispatchPriority();
}

bool isTeFrontUrl(cabot::String const& url)
{
    return ((url.find("te-front-portal") != cabot::String::npos) ||
            (url.find("te-front-mgr") != cabot::String::npos) ||
            (url.find("te-front-epg") != cabot::String::npos));
}

} // namespace

OperaWindowManager* OperaWindowManager::m_the_window_manager = nullptr;

OperaWindowManager::OperaWindowManager(OperaEngine& opera_engine)
    : m_ui_window(nullptr)
    , m_opera_engine(opera_engine)
    , m_is_oipf_window_supported(false)
    , m_app_state_change_callback(nullptr)
    , m_is_number_key_sent_from_rc(false)
    , m_usb_keyboard_plugged(false)
    , m_current_url("")
    , m_is_apm_started(false)
    , m_is_start_url_opened(false)
    , m_is_repeat_key_active(false)
{
    CABOT_ASSERT(!m_the_window_manager);
    m_the_window_manager = this;
}

OperaWindowManager::~OperaWindowManager()
{
    m_the_window_manager = nullptr;

    destroyWindowList();
}

void
OperaWindowManager::destroyWindowList()
{
    //FrostMutexLock lock(m_list_mutex);
    WindowIterator it =  m_window_list.begin();
    for (; it != m_window_list.end(); ++it)
    {
        delete *it;
    }
    m_window_list.clear();
}

OperaWindowManager* OperaWindowManager::getWindowManager()
{
    return m_the_window_manager;
}

void
OperaWindowManager::initialise(int media_resource_id)
{
    /*if (!m_list_mutex.initialise())
    {
        TRACE_ERROR(("Failed to initialise mutex\n"));
    }*/

    InitialiseNativeOperaWindowManager(this);
    BPI_NumberKeyHandlerSetNumberKeyHandler(this);
    bpi::windowManagerInitialise();
    bpi::windowManagerSetWindowManager(this);
    
    m_ui_window = new OperaBrowserWindow(m_opera_engine,
            OperaWindow::window_name_ui, OperaWindow::WindowType::window_ui, 10);

#ifdef INCLUDE_HYBRID_UI
    NEBULA_ApplicationSharedCapability shared_capabilities;
    NEBULA_GetSharedCapability(&shared_capabilities);
    if (shared_capabilities.hybrid_ui_availability)
    {
        m_is_oipf_window_supported = true;
    }
    else
#endif
    {
        m_is_oipf_window_supported = false;
    }
    setKeySet(*m_ui_window, 0, std::vector<int>());
    insertToList(m_ui_window, m_window_list);
    OperaHbbTVWindow* hbbtv_window = new OperaHbbTVWindow(m_opera_engine, priority_max_value);
    insertToList(hbbtv_window, m_window_list);
    
    OperaCspWindow* csp_window = new OperaCspWindow(m_opera_engine, priority_max_value);
    insertToList(csp_window, m_window_list);
    
    auto* media_player_window = new OperaMediaPlayerWindow(m_opera_engine, priority_max_value, 
                                                           media_resource_id);
    insertToList(media_player_window, m_window_list);
    
    OperaTVStoreWindow* tv_store_window = new OperaTVStoreWindow(m_opera_engine, priority_max_value);
    insertToList(tv_store_window, m_window_list);
#ifdef INCLUDE_OPAPP_SUPPORT
    OperaOpAppWindow* opapp_window = new OperaOpAppWindow(m_opera_engine, priority_max_value);
    opapp_window->initialise();
    insertToList(opapp_window, m_window_list);
    opapp_window->attachHbbTvWindow(*hbbtv_window);
#endif
    OperaOipfWindow* oipf_window = new OperaOipfWindow(m_opera_engine,
                                                       OperaWindow::window_name_oipf,
                                                       OperaWindow::WindowType::window_oipf,
                                                       priority_max_value);
    insertToList(oipf_window, m_window_list);

    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow* launcher_window = new OperaLauncherWindow(m_opera_engine, priority_max_value);
        insertToList(launcher_window, m_window_list);
        launcher_window->bringToFront();
    }

    m_opera_engine.addEventHandler(&OperaWindowManager::onWindowFocusChanged, this);
}

OperaWindow*
OperaWindowManager::createOipfWindow(cabot::String const& window_name, int event_dispatch_priority)
{
#ifdef INCLUDE_HYBRID_UI
    NEBULA_ApplicationSharedCapability shared_capabilities;
    NEBULA_GetSharedCapability(&shared_capabilities);
    if (shared_capabilities.hybrid_ui_availability)
    {
        OperaWindow* opera_window = getWindowByName(window_name);
        if (opera_window)
        {
            setEventDispatchPriority(*opera_window, event_dispatch_priority);
        }
        else
        {
            opera_window = new OperaBrowserWindow(m_opera_engine, window_name,
                    OperaWindow::WindowType::window_ui, event_dispatch_priority);
            insertToList(opera_window, m_window_list);
        }

        return opera_window;
    }
    else
#else
    CABOT_UNUSED_PARAM(window_name);
    CABOT_UNUSED_PARAM(event_dispatch_priority);
#endif
    {
        return nullptr;
    }
}

OperaWindow*
OperaWindowManager::createGenericWindow(cabot::String const& window_name, int event_dispatch_priority)
{
    OperaWindow* opera_window = getWindowByName(window_name);
    if (opera_window)
    {
        setEventDispatchPriority(*opera_window, event_dispatch_priority);
    }
    else
    {
        opera_window = new OperaBrowserWindow(m_opera_engine, window_name,
                OperaWindow::WindowType::window_ui, event_dispatch_priority);
        insertToList(opera_window, m_window_list);
    }
    return opera_window;
}

OperaWindow&
OperaWindowManager::uiWindow()
{
    CABOT_ASSERT(m_ui_window);
    return *m_ui_window;
}

OperaOipfWindow&
OperaWindowManager::oipfWindow()
{
    OperaWindow* oipf_window = getWindowByName(OperaWindow::window_name_oipf);
    CABOT_ASSERT(oipf_window);
    return *static_cast<OperaOipfWindow*>(oipf_window);
}

OperaLauncherWindow&
OperaWindowManager::launcherWindow()
{
    OperaWindow* launcher_window = getWindowByName(OperaWindow::window_name_launcher);
    CABOT_ASSERT(launcher_window);
    return *static_cast<OperaLauncherWindow*>(launcher_window);
}

OperaHbbTVWindow&
OperaWindowManager::hbbTVWindow()
{
    OperaWindow* hbbtv_window = getWindowByName(OperaWindow::window_name_hbbtv);
    CABOT_ASSERT(hbbtv_window);
    return *static_cast<OperaHbbTVWindow*>(hbbtv_window);
}

#ifdef INCLUDE_OPAPP_SUPPORT
OperaOpAppWindow&
OperaWindowManager::opAppWindow() const
{
    OperaWindow* opapp_window = getWindowByName(OperaWindow::window_name_opapp);
    CABOT_ASSERT(opapp_window);
    return *static_cast<OperaOpAppWindow*>(opapp_window);
}
#endif

OperaCspWindow&
OperaWindowManager::cspWindow()
{
    OperaWindow* csp_window = getWindowByName(OperaWindow::window_name_csp);
    CABOT_ASSERT(csp_window);
    return *static_cast<OperaCspWindow*>(csp_window);
}

OperaTVStoreWindow&
OperaWindowManager::tvStoreWindow()
{
    OperaWindow* tv_store_window = getWindowByName(OperaWindow::window_name_tv_store);
    CABOT_ASSERT(tv_store_window);
    return *static_cast<OperaTVStoreWindow*>(tv_store_window);
}

OperaBrowserUIWindow&
OperaWindowManager::getBrowserUIWindow(cabot::String const& start_url)
{
    OperaWindow* browser_ui_window = getWindowByName(OperaWindow::window_name_browser_ui);

    if (!browser_ui_window)
    {
        unsigned int plane_height = m_opera_engine.getPlaneHeight();
        browser_ui_window = new OperaBrowserUIWindow(m_opera_engine, start_url, priority_max_value, plane_height);
        insertToList(browser_ui_window, m_window_list);
    }
    CABOT_ASSERT(browser_ui_window);

    return *static_cast<OperaBrowserUIWindow*>(browser_ui_window);
}

OperaMediaPlayerWindow&
OperaWindowManager::mediaPlayerWindow()
{
    OperaWindow* media_player_window = getWindowByName(OperaWindow::window_name_media_player);
    CABOT_ASSERT(media_player_window);
    return *static_cast<OperaMediaPlayerWindow*>(media_player_window);
}

void
OperaWindowManager::destroyWindow(OperaWindow* window)
{
    CABOT_ASSERT(window);

    setActive(*window, false);

    if (window->type() == OperaWindow::WindowType::window_ui ||
        window->type() == OperaWindow::WindowType::window_browser_ui)
    {
        setVisible(*window, false);    
        notifyAppStateChange(window->handle(), ApplicationTerminated);
        removeFromList(window, m_window_list);
        delete window;
    }
    else if (window->type() == OperaWindow::WindowType::window_csp)
    {
        notifyAppStateChange(window->handle(), ApplicationTerminated);
        stopCspApp();
    }
    else if (window->type() == OperaWindow::WindowType::window_oipf)
    {
        notifyAppStateChange(window->handle(), ApplicationTerminated);
        stopOipfApp();
    }
    else
    {
        TRACE_ERROR(("Invalid window type : %d\n", window->type()));
    }
}

bool
OperaWindowManager::handleKeyInputWithDownUpKeys(nebula::KeyCode key_code,
                                   nebula::KeyPressHandlingReason keypress_handling_reason,
                                   bool key_released)
{
    //FrostMutexLock lock(m_list_mutex);

    TRACE_INFO(("handleKeyInputWithDownUpKeys(key_code = %d, reason = %s, key_released = %d)\n",
                key_code,
                toString(keypress_handling_reason),
                key_released));

    const OperaWindow* window = findWindowToHandleKeyInput(key_code, keypress_handling_reason);

    if (window)
    {
        TRACE_INFO(("key_code[%d] is handled by %s\n",
                        key_code,
                        window->name().charArray()));

        if (!key_released)
        {
            sendKeyDown(*window, key_code);
        }
        else
        {
            sendKeyUp(*window, key_code);
        }            
    }
    else
    {
        TRACE_INFO(("key_code[%d] is not handled by any window\n", key_code));
    }

    return window != nullptr;
}    

bool
OperaWindowManager::handleKeyInput(nebula::KeyCode key_code,
                                   nebula::KeyPressHandlingReason keypress_handling_reason)
{
    //FrostMutexLock lock(m_list_mutex);

    TRACE_ALWAYS(("handleKeyInput(key_code = %d, reason = %s)\n",
                key_code,
                toString(keypress_handling_reason)));

    const OperaWindow* window = findWindowToHandleKeyInput(key_code, keypress_handling_reason);
    OperaWindow* window2 = const_cast<OperaWindow*>(window);


    if (window)
    {
        TRACE_ALWAYS(("key_code[%d] is handled by %s\n",
                        key_code,
                        window->name().charArray()));
        setActive(*window2, true);
        sendKeyInput(*window, key_code);
    }
    else
    {
        TRACE_ALWAYS(("key_code[%d] is not handled by any window\n", key_code));
    }

    return window != nullptr;
}

const OperaWindow*
OperaWindowManager::findWindowToHandleKeyInput(nebula::KeyCode key_code,
                                        nebula::KeyPressHandlingReason keypress_handling_reason)
{
    TRACE_INFO(("findWindowToHandleKeyInput(key_code = %d, reason = %s)\n",
                key_code,
                toString(keypress_handling_reason)));

    auto any_window = m_active_window_list.findWindowToHandleKeyInput(key_code,
                                                                      keypress_handling_reason);
    const OperaWindow* window = static_cast<const OperaWindow*>(any_window);


    // Order for delivering events to a window. Lower values
    // mean higher priority: A window with event capture priority
    // of 0, will always get the events first.
#ifndef INCLUDE_AVS_MMSDK
    if (!window)
    {
        for (const auto& temp_window : m_window_list)
        {
            TRACE_INFO(("temp_window->type()-------->  %d\n",temp_window->type()));
            if (temp_window && !temp_window->isActive() &&
                temp_window->type() == OperaWindow::WindowType::window_ui &&
                temp_window->shouldHandleKey(key_code, keypress_handling_reason))
            {
                window = temp_window;
                TRACE_INFO(("Key is accepted by Window %s\n", window->name().charArray()));
                break;
            }
        }
    }
#endif

    return window;
}

void
OperaWindowManager::sendKeyInput(const OperaWindow& window, nebula::KeyCode key_code)
{
    using omi::msg::KeyInput;
    OperaWindowHandle handle = OperaWindowHandle(-1);

    // This is required for opapp support, where it is possible for the OpApp window
    // to not have focus and yet still process keys. Forcing the target window bypasses
    // browser focus, IME and virtual key handling.
    if (!window.isFocused() && window.type() == OperaWindow::WindowType::window_opapp)
    {
        if (Profile_IsOpAppVewdDemoEnabled() == 0)
        {
            handle = window.handle();
        }
    }

    // Youtube requires escape key instead of back
    if (window.getCurrentPageUrl().find("youtube.com/tv") != cabot::String::npos)
    {
        if (key_code == nebula::KeyBack)
        {
            key_code = nebula::Escape;
        }
    }

    if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (key_code == nebula::KeyProgramDown)
        {
            key_code = nebula::KeyPageDown;
        }
        else if (key_code == nebula::KeyProgramUp)
        {
            key_code = nebula::KeyPageUp;
        }
        else if (key_code == nebula::KeyRed)
        {
            key_code = nebula::KeyZoomIn;
        }
        else if (key_code == nebula::KeyGreen)
        {
            key_code = nebula::KeyZoomOut;
        }
        else if (key_code == nebula::KeyYellow)
        {
            key_code = nebula::KeyBrowserUrlBar;
        }
        else if (key_code == nebula::KeyBlue)
        {
            key_code = nebula::KeyBrowserSettings;
        }
    }

    if (key_code >= nebula::KeyZero &&
            key_code <= nebula::KeyNine)
    {
        m_is_number_key_sent_from_rc = true;
    }
    else
    {
        m_is_number_key_sent_from_rc = false;
    }

    // We do not receive separate key down and key up events from RC,
    // so we need to simulate down and up
    KeyInput key_input_down;
    KeyInput key_input_up;
    if (window.type() == OperaWindow::WindowType::window_ui)
    {
        key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle);
        key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
    }
    else if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (((key_code > nebula::KeyRight && key_code < nebula::KeyBack) ||
            (key_code > nebula::KeyNext && key_code < nebula::A) ||
            (key_code > nebula::Z && key_code < nebula::LittleA) ||
            (key_code > nebula::LittleZ && key_code < nebula::KeyChannelUp) ||
            (key_code > nebula::KeyFavourites && key_code < nebula::KeyExit) ||
            (key_code > nebula::KeyMedion)) && (m_usb_keyboard_plugged == false))
        {
            key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle);
            key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
        }
        else
        {
            key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down);
            key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up);
        }
    }
    else
    {
        key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle);
        key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
    }

    m_opera_engine.post(key_input_down);
    m_opera_engine.post(key_input_up);

    TRACE_INFO(("Key[%d] sent to browser\n", key_code));
}

void
OperaWindowManager::sendKeyDown(const OperaWindow& window, nebula::KeyCode key_code)
{
    using omi::msg::KeyInput;
    OperaWindowHandle handle = OperaWindowHandle(-1);

    // This is required for opapp support, where it is possible for the OpApp window
    // to not have focus and yet still process keys. Forcing the target window bypasses
    // browser focus, IME and virtual key handling.
    if (!window.isFocused() && window.type() == OperaWindow::WindowType::window_opapp)
    {
        if (Profile_IsOpAppVewdDemoEnabled() == 0)    
        {
            handle = window.handle();
        }
    }

    // Youtube requires escape key instead of back
    if (window.getCurrentPageUrl().find("youtube.com/tv") != cabot::String::npos)
    {
        if (key_code == nebula::KeyBack)
        {
            key_code = nebula::Escape;
        }
    }

    if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (key_code == nebula::KeyProgramDown)
        {
            key_code = nebula::KeyPageDown;
        }
        else if (key_code == nebula::KeyProgramUp)
        {
            key_code = nebula::KeyPageUp;
        }
        else if (key_code == nebula::KeyRed)
        {
            key_code = nebula::KeyZoomIn;
        }
        else if (key_code == nebula::KeyGreen)
        {
            key_code = nebula::KeyZoomOut;
        }
        else if (key_code == nebula::KeyYellow)
        {
            key_code = nebula::KeyBrowserUrlBar;
        }
        else if (key_code == nebula::KeyBlue)
        {
            key_code = nebula::KeyBrowserSettings;
        }
    }

    if (key_code >= nebula::KeyZero &&
            key_code <= nebula::KeyNine)
    {
        m_is_number_key_sent_from_rc = true;
    }
    else
    {
        m_is_number_key_sent_from_rc = false;
    }

    // We do not receive separate key down and key up events from RC,
    // so we need to simulate down in this function
    KeyInput key_input_down;
    if (window.type() == OperaWindow::WindowType::window_ui)
    {
        key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle);
    }
    else if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (((key_code > nebula::KeyRight && key_code < nebula::KeyBack) ||
            (key_code > nebula::KeyNext && key_code < nebula::A) ||
            (key_code > nebula::Z && key_code < nebula::LittleA) ||
            (key_code > nebula::LittleZ && key_code < nebula::KeyChannelUp) ||
            (key_code > nebula::KeyFavourites && key_code < nebula::KeyExit) ||
            (key_code > nebula::KeyMedion)) && (m_usb_keyboard_plugged == false))
        {
            key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle);
        }
        else
        {
            key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down);
        }
    }
    else
    {
        key_input_down = nebulaKeyToOperaKeyInput(key_code, key_down, handle, m_is_repeat_key_active);
        m_is_repeat_key_active = true;
    }

    m_opera_engine.post(key_input_down);

    TRACE_INFO(("Key[%d] sent to browser with key_down\n", key_code));
}

void
OperaWindowManager::sendKeyUp(const OperaWindow& window, nebula::KeyCode key_code)
{
    using omi::msg::KeyInput;
    OperaWindowHandle handle = OperaWindowHandle(-1);
    m_is_repeat_key_active = false;

    // This is required for opapp support, where it is possible for the OpApp window
    // to not have focus and yet still process keys. Forcing the target window bypasses
    // browser focus, IME and virtual key handling.
    if (!window.isFocused() && window.type() == OperaWindow::WindowType::window_opapp)
    {
        if (Profile_IsOpAppVewdDemoEnabled() == 0)    
        {
            handle = window.handle();
        }
    }

    // Youtube requires escape key instead of back
    if (window.getCurrentPageUrl().find("youtube.com/tv") != cabot::String::npos)
    {
        if (key_code == nebula::KeyBack)
        {
            key_code = nebula::Escape;
        }
    }

    if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (key_code == nebula::KeyProgramDown)
        {
            key_code = nebula::KeyPageDown;
        }
        else if (key_code == nebula::KeyProgramUp)
        {
            key_code = nebula::KeyPageUp;
        }
        else if (key_code == nebula::KeyRed)
        {
            key_code = nebula::KeyZoomIn;
        }
        else if (key_code == nebula::KeyGreen)
        {
            key_code = nebula::KeyZoomOut;
        }
        else if (key_code == nebula::KeyYellow)
        {
            key_code = nebula::KeyBrowserUrlBar;
        }
        else if (key_code == nebula::KeyBlue)
        {
            key_code = nebula::KeyBrowserSettings;
        }
    }

    if (key_code >= nebula::KeyZero &&
            key_code <= nebula::KeyNine)
    {
        m_is_number_key_sent_from_rc = true;
    }
    else
    {
        m_is_number_key_sent_from_rc = false;
    }

    // We do not receive separate key down and key up events from RC,
    // so we need to simulate up in this function when key is released
    KeyInput key_input_up;
    if (window.type() == OperaWindow::WindowType::window_ui)
    {
        key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
    }
    else if (window.type() == OperaWindow::WindowType::window_browser_ui)
    {
        if (((key_code > nebula::KeyRight && key_code < nebula::KeyBack) ||
            (key_code > nebula::KeyNext && key_code < nebula::A) ||
            (key_code > nebula::Z && key_code < nebula::LittleA) ||
            (key_code > nebula::LittleZ && key_code < nebula::KeyChannelUp) ||
            (key_code > nebula::KeyFavourites && key_code < nebula::KeyExit) ||
            (key_code > nebula::KeyMedion)) && (m_usb_keyboard_plugged == false))
        {
            key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
        }
        else
        {
            key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up);
        }
    }
    else
    {
        key_input_up = nebulaKeyToOperaKeyInput(key_code, key_up, handle);
    }

    m_opera_engine.post(key_input_up);

    TRACE_INFO(("Key[%d] sent to browser with key_up\n", key_code));
}

void
OperaWindowManager::setKeySet(OperaWindow& window, int keyset_value, std::vector<int> other_keys)
{
    window.setKeySet(keyset_value, other_keys);
}

void
OperaWindowManager::getKeySet(OperaWindow& window, int* keyset_value, std::vector<int>* other_keys) const
{
    window.getKeySet(keyset_value, other_keys);
}

void
OperaWindowManager::setEventDispatchPriority(OperaWindow& window, int event_dispatch_priority)
{
    window.setEventDispatchPriority(event_dispatch_priority);
    sortListByPriority();// update list since event dispatch priority of a window is changed.
}

int
OperaWindowManager::getEventDispatchPriority(OperaWindow& window) const
{
    return window.getEventDispatchPriority();
}

void
OperaWindowManager::setVisible(OperaWindow& window, bool visible)
{
    return window.setVisible(visible);
}

void
OperaWindowManager::setActive(OperaWindow& window, bool active)
{
    //FrostMutexLock lock(m_list_mutex);

    if (active)
    {
        //Activating an already active window should make it "first' active window
        //Therefore, just remove and re-insert it.
        if (window.isActive())
        {
            m_active_window_list.remove(&window);
        }
        m_active_window_list.insert(&window);
    }
    else
    {
        m_active_window_list.remove(&window);
    }
    window.setActive(active);
    updateActiveWindowsLocked();
    OperaWindow * active_window = getActiveWindowLocked();
    if (active_window)
    {
        CABOT_ASSERT(active_window->isActive());
        NEBULA_PictureSettingsSetOsdOnScreen(frost_true, NEBULA_OsdTypeBrowserOnScreen);
    }
    else
    {
        NEBULA_PictureSettingsSetOsdOnScreen(frost_false, NEBULA_OsdTypeBrowserOnScreen);
    }
}

void
OperaWindowManager::updateFocus()
{
    TRACE_INFO(("updateFocus()\n"));

    // Assumes m_list_mutex is locked.

    OperaWindow * active_window = getActiveWindowLocked();
    if (active_window && active_window->isFocusable())
    {
        TRACE_INFO(("Giving focus to %s window\n", active_window->name().c_str()));
#ifdef INCLUDE_HOTELTV
        if(active_window->name().find("HBBTV") != cabot::String::npos)
        {
            setVisible(hbbTVWindow(), true);
        }
#endif
        m_opera_engine.post(omi::msg::Focus(active_window->handle()));
    }
    else
    {
        TRACE_INFO(("Removing focus from all windows\n"));
        m_opera_engine.post(omi::msg::Blur());
    }
}

void
OperaWindowManager::setFocusable(OperaWindow& window, bool focusable)
{
    window.setFocusable(focusable);
    updateActiveWindows();
}

void
OperaWindowManager::reset(OperaWindow& window, bool clear_keyset)
{
    setActive(window, false);
    window.reset(clear_keyset);
}

bool
OperaWindowManager::loadUrl(OperaWindow& window, cabot::String const& url)
{
    return window.loadUrl(url);
}

bool
OperaWindowManager::setPreferenceValue(OperaWindow& window, UString const& key, UString const& value)
{
    return window.setPreferenceValue(key, value);
}

bool
OperaWindowManager::getPreferenceValue(OperaWindow& window, UString const& key, UString& value)
{
    return window.getPreferenceValue(key, value);
}

bool
OperaWindowManager::moveInHistory(OperaWindow& window, int steps)
{
    if (steps == -1 && window.type() == OperaWindow::WindowType::window_tv_store)
    {
        using nebula::KeyPressHandlingReason;
        return handleKeyInput(nebula::KeyBack, KeyPressHandlingReason::pre_terminal_ui_handling);
    }
    return window.moveInHistory(steps);
}

int
OperaWindowManager::getHistoryCount(OperaWindow& window)
{
    return window.getHistoryCount();
}

int
OperaWindowManager::getCurrentHistoryIndex(OperaWindow& window)
{
    return window.getCurrentHistoryIndex();
}

bool
OperaWindowManager::isVisible(OperaWindow& window) const
{
    return window.isVisible();
}

bool
OperaWindowManager::isActive(OperaWindow& window) const
{
    return window.isActive();
}

void
OperaWindowManager::getName(OperaWindow& window, cabot::String& name) const
{
    name = window.name();
}

void
OperaWindowManager::getUrl(OperaWindow& window, cabot::String& url) const
{
    url = window.getCurrentPageUrl();
}

VectorOfOperaWindowPtr const&
OperaWindowManager::getWindows() const
{
    return m_window_list;
}

OperaWindow*
OperaWindowManager::getActiveWindowLocked() const
{
    // Assumes m_list_mutex is locked.

    auto active_window = m_active_window_list.getActiveWindow();
    return static_cast<OperaWindow*>(active_window);
}

OperaWindow*
OperaWindowManager::getActiveWindow() const
{
    //FrostMutexLock lock(m_list_mutex);
    return getActiveWindowLocked();
}

VectorOfOperaWindowPtr 
OperaWindowManager::getActiveWindows() const
{
    //FrostMutexLock lock(m_list_mutex);
    return getActiveWindowsLocked();
}

VectorOfOperaWindowPtr 
OperaWindowManager::getActiveWindowsLocked() const
{
    auto windows = m_active_window_list.getActiveWindows();

    VectorOfOperaWindowPtr opera_windows;
    for (auto window : windows)
    {
        opera_windows.push_back(static_cast<OperaWindow*>(window));
    }
    return opera_windows;
}

OperaWindow*
OperaWindowManager::getActiveWindowForComponents() const
{
    //FrostMutexLock lock(m_list_mutex);

    auto active_window = m_active_window_list.getActiveWindowForComponents();
    return static_cast<OperaWindow*>(active_window);
}

OperaWindow*
OperaWindowManager::getActiveWindowByName(cabot::String const& name) const
{
    //FrostMutexLock lock(m_list_mutex);
    return static_cast<OperaWindow*>(m_active_window_list.getActiveWindowByName(name));
}

OperaWindow*
OperaWindowManager::getWindowByName(cabot::String const& name) const
{
    //FrostMutexLock lock(m_list_mutex);
    auto const pos = std::find_if(m_window_list.begin(), m_window_list.end(), NameEqual(name));
    return (pos != m_window_list.end()) ? *pos : nullptr;
}

void
OperaWindowManager::getWindowListAsName()
{
    if (m_window_list.size() != 0)
    {
        for (unsigned int i=0; i<m_window_list.size(); i++)
        {
            TRACE_ALWAYS(( "\x1b[34m************** %s, %s, %d, Window List[%d]: %s\n\x1b[0m", __FILE__, __FUNCTION__, __LINE__, i, m_window_list[i]->name().charArray() ));
        }
    }

    auto windows = m_active_window_list.getActiveWindows();

    for (unsigned int i=0; i<windows.size(); i++)
    {
        TRACE_ALWAYS(( "\x1b[34m************** %s, %s, %d, Active Window List[%d]: %s\n\x1b[0m", __FILE__, __FUNCTION__, __LINE__, i, windows[i]->name().charArray() ));
    }
}

OperaWindow*
OperaWindowManager::getWindowByHandleLocked(OperaWindowHandle const& handle) const
{
    // Assumes m_list_mutex is locked.

    for (unsigned int i=0; i<m_window_list.size(); i++)
    {
        if (m_window_list[i]->handle() == handle)
        {
            return m_window_list[i];
        }
    }
    return nullptr;
}

OperaWindow*
OperaWindowManager::getWindowByHandle(OperaWindowHandle const& handle) const
{
    //FrostMutexLock lock(m_list_mutex);
    return getWindowByHandleLocked(handle);
}

OperaWindowHandle
OperaWindowManager::getWindowHandle(OperaWindow& window)
{
    return window.handle();
}

void
OperaWindowManager::setApmStarted(bool apm_started)
{
    m_is_apm_started = apm_started;
}

bool
OperaWindowManager::getApmStarted()
{
    return m_is_apm_started;
}

void
OperaWindowManager::setVisibleAllWindows(bool visible)
{
    setVisible(hbbTVWindow(), visible);
    setVisible(cspWindow(), visible);
    setVisible(tvStoreWindow(), visible);
    setVisible(uiWindow(), visible);
#ifdef INCLUDE_OPAPP_SUPPORT
    setVisible(opAppWindow(), visible);
#endif
    setVisible(oipfWindow(), visible);
}

bool 
OperaWindowManager::terminateApp(WebBrowserAppType const app_type)
{
    switch (app_type)
    {
    case WebBrowserAppType::hbbtv_app:
    case WebBrowserAppType::hbbtv_txt_app:
    {
        TRACE_INFO(("Terminating HbbTV Window\n"));
        bool result = hbbTVWindow().terminateCurrentAppWithResult();
        setActive(hbbTVWindow(), false);
        return result;
    }
    case WebBrowserAppType::media_player:
        TRACE_INFO(("Terminating Media Player Window\n"));
        mediaPlayerWindow().terminateCurrentApp();
        setActive(mediaPlayerWindow(), false);
        return true;
    
    case WebBrowserAppType::user_app:
        if (cspWindow().isLinearIPService())
        {
            cspWindow().terminateCurrentApp();
            setActive(mediaPlayerWindow(), false);
            return true;
        }
        TRACE_ERROR(("App type '%d' not supported\n", convertToUnderlyingType(app_type)));
        SHOULD_NOT_BE_HERE();
        return false;
    
    case WebBrowserAppType::op_app:
        TRACE_ERROR(("App type '%d' not supported\n", convertToUnderlyingType(app_type)));
        SHOULD_NOT_BE_HERE();
        return false;
    }
    return false;
}

void
OperaWindowManager::terminateUserApps()
{
#ifdef NFLX5T
    if (getApmStarted())
    {
        setVisibleAllWindows(false);
        setApmStarted(false);
    }
#endif
    hbbTVWindow().terminateCurrentApp();
    cspWindow().terminateCurrentApp();
    tvStoreWindow().terminateCurrentApp();
    oipfWindow().terminateCurrentApp();
    mediaPlayerWindow().terminateCurrentApp();

    setActive(hbbTVWindow(), false);
    setActive(cspWindow(), false);
    setActive(tvStoreWindow(), false);
    setActive(oipfWindow(), false);
    setActive(mediaPlayerWindow(), false);

    OperaWindow* browser_ui_win = getWindowByName(OperaWindow::window_name_browser_ui);
    if (browser_ui_win)
    {
        browser_ui_win->terminateCurrentApp();
        destroyWindow(browser_ui_win);
    }
}

void
OperaWindowManager::notifyBrowserProcessStateChange(BrowserProcessState state)
{
    switch (state)
    {
    case browser_process_state_initialised:
        // Currently ignored.
        break;
    case browser_process_state_shutdown:
        terminateUserApps();
#ifdef INCLUDE_OPAPP_SUPPORT
        opAppWindow().terminate();
#endif
        break;
    case browser_process_state_restarted:
        recreateWindows();
        break;
    default:
        SHOULD_NOT_BE_HERE();
    }
}

void 
OperaWindowManager::recreateWindows()
{
    // Some windows have a permanent window handle that must be refreshed after
    // browser restart.
    hbbTVWindow().createWindow();
#ifdef INCLUDE_OPAPP_SUPPORT
    opAppWindow().initialise();
#endif
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        launcherWindow().createWindow();
        launcherWindow().bringToFront();
    }
}

bool
OperaWindowManager::startHbbTVApp(unsigned int org_id, unsigned short app_id)
{
    stopCspApp();
    stopOipfApp();
    stopBrowserUi();

    OperaHbbTVWindow& hbbtv_window = hbbTVWindow();
    return hbbtv_window.loadApp(org_id, app_id);
}

bool
OperaWindowManager::startHbbTVApp(cabot::String const& application_url)
{
    stopCspApp();
    stopOipfApp();
    stopBrowserUi();

    OperaHbbTVWindow& hbbtv_window = hbbTVWindow();
    return hbbtv_window.loadUrl(application_url);

}

bool
OperaWindowManager::startTVStoreApp(cabot::String const& tv_store_url)
{
    stopCspApp();
    stopOipfApp();
    stopBrowserUi();

    if (tvStoreWindow().loadUrl(tv_store_url))
    {
        setVisible(tvStoreWindow(), true);
        setActive(tvStoreWindow(), true);
        return true;
    }
    return false;
}

bool
OperaWindowManager::startCspApp(cabot::String const& csp_url)
{
    stopCspApp();
    stopOipfApp();
    stopBrowserUi();
    stopTVStoreApp();

    if (cspWindow().loadUrl(csp_url))
    {
        if (csp_url.find("portaltv.tv/partner/infobar/") != cabot::String::npos)
        {
            return true;
        }

        setVisible(cspWindow(), true);
        setActive(cspWindow(), true);
        return true;
    }
    return false;
}

void
OperaWindowManager::stopCspApp()
{
    setActive(cspWindow(), false);
    cspWindow().stop();
}

void
OperaWindowManager::stopTVStoreApp()
{
    setActive(tvStoreWindow(), false);
    tvStoreWindow().stop();
}

void
OperaWindowManager::startBrowserUi(cabot::String const& start_url)
{
    stopCspApp();
    stopOipfApp();
    stopHbbTvApp();

    stopBrowserUi();
    OperaBrowserUIWindow& browser_ui_win = getBrowserUIWindow(start_url);

    setVisible(browser_ui_win, true);
    setActive(browser_ui_win, true);
}

void
OperaWindowManager::stopHbbTvApp(bool launch_autostart_app)
{
    OperaHbbTVWindow& hbbv_window = hbbTVWindow();
    hbbv_window.setRestartAfterTermination(launch_autostart_app);
    hbbv_window.terminateCurrentApp();
}

bool
OperaWindowManager::startOipfApp(cabot::String const& application_url)
{
    TRACE_INFO(("startOipfApp() [%s] \n", application_url.charArray()));
    stopOipfApp();
    stopCspApp();
    stopBrowserUi();

    OperaOipfWindow& oipf_window = oipfWindow();
    if (oipf_window.loadUrl(application_url))
    {
        setVisible(oipf_window, true);
        setActive(oipf_window, true);
        setFocusable(oipf_window, true);
        TRACE_INFO(("startOipfApp() [%s] - DONE\n", application_url.charArray()));
        return true;
    }
    TRACE_INFO(("startOipfApp() [%s] - FAILED \n", application_url.charArray()));
    return false;
}

void
OperaWindowManager::stopOipfApp()
{
    OperaOipfWindow& oipf_window = oipfWindow();
    oipf_window.terminateCurrentApp();
}

bool
OperaWindowManager::startBrowserLauncher(cabot::String const& application_url)
{
    TRACE_ALWAYS(("startBrowserLauncher() [%s] \n", application_url.charArray()));
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        stopOipfApp();
        stopCspApp();
        stopBrowserUi();

        OperaLauncherWindow& launcher_window = launcherWindow();
        if (launcher_window.loadUrl(application_url))
        {
            setVisible(launcher_window, false);
            setActive(launcher_window, false);
            setFocusable(launcher_window, false);
            TRACE_ALWAYS(("startBrowserLauncher() [%s] - DONE\n", application_url.charArray()));
            return true;
        }
    }
    TRACE_ALWAYS(("startBrowserLauncher() [%s] - FAILED \n", application_url.charArray()));
    return false;
}

void
OperaWindowManager::stopBrowserLauncher()
{
    TRACE_ALWAYS(("stopBrowserLauncher()\n"));
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        setVisible(launcher_window, false);
        setActive(launcher_window, false);
        setFocusable(launcher_window, false);
        launcher_window.terminateCurrentApp();
    }
}

void
OperaWindowManager::reloadBrowserLauncher()
{
    TRACE_ALWAYS(("reloadBrowserLauncher()\n"));
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        launcher_window.reloadCurrentApp();
    }
}

void
OperaWindowManager::setBrowserLauncherVisible(bool visible)
{
    TRACE_ALWAYS(("setBrowserLauncherVisible() %d\n", visible));
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        setVisible(launcher_window, visible);
        setActive(launcher_window, visible);
        setFocusable(launcher_window, visible);
    }
}

bool
OperaWindowManager::isBrowserLauncherFocused()
{
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        return launcher_window.isFocused();
    }
    else
    {
        return false;
    }
}

bool
OperaWindowManager::isBrowserLauncherVisible()
{
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        return launcher_window.isVisible();
    }
    else
    {
        return false;
    }
}

bool
OperaWindowManager::setFocusToBrowserLauncher(bool focus)
{
    TRACE_ALWAYS(("setFocusToLauncherWindow() %d\n", focus));
    if (Profile_IsTiVoBrowserLauncherEnabled() != tivo_browser_launcher_disabled)
    {
        OperaLauncherWindow& launcher_window = launcherWindow();
        setActive(launcher_window, focus);
        setFocusable(launcher_window, focus);
        if (focus)
        {
            if (!launcher_window.isVisible())
            {
                setVisible(launcher_window, true);
            }
            launcherWindow().bringToFront();
        }
        return true;
    }
    return false;
}

bool 
OperaWindowManager::startMediaPlayer(cabot::String const& url)
{
    TRACE_INFO(("%s() [%s] \n", __func__, url.charArray()));
    stopOipfApp();
    stopCspApp();
    stopBrowserUi();
    stopHbbTvApp();

    auto& media_player_window = mediaPlayerWindow();
    if (media_player_window.loadUrl(url))
    {
        setVisible(media_player_window, true);
        setActive(media_player_window, true);
        setFocusable(media_player_window, true);
        TRACE_INFO(("%s() [%s] - DONE\n", __func__, url.charArray()));
        return true;
    }
    return false;
}

void
OperaWindowManager::resetMediaPlayer()
{
    auto& media_player = mediaPlayerWindow();
    setVisible(media_player, false);
    // Set inactive and clear url.
    reset(media_player);
    setFocusable(media_player, false);
}

void
OperaWindowManager::stopBrowserUi()
{
    OperaWindow* browser_ui_win = getWindowByName(OperaWindow::window_name_browser_ui);
    if (browser_ui_win)
    {
        destroyWindow(browser_ui_win);
    }
}

void
OperaWindowManager::getUiApp(cabot::String& application_url)
{
    application_url = m_ui_window_url;
}

bool
OperaWindowManager::startUiApp(cabot::String const& application_url,
                               cabot::String const& overlay_application_url,
                               ApplicationResolution application_resolution)
{
    CABOT_UNUSED_PARAM(overlay_application_url);

    m_ui_window_url = application_url;

    OperaWindow::RenderingResolution rendering_resolution;

    switch (application_resolution)
    {
        case application_resolution_hd:
            rendering_resolution = OperaWindow::rendering_resolution_1280x720;
            break;

        case application_resolution_fhd:
            rendering_resolution = OperaWindow::rendering_resolution_1920x1080;
            break;

        default:
            rendering_resolution = OperaWindow::rendering_resolution_default;
            break;
    }

    if (uiWindow().loadUrl(m_ui_window_url, rendering_resolution))
    {
        return true;
    }

    return false;
}

bool
OperaWindowManager::reloadUiApp()
{
    CABOT_ASSERT(m_ui_window);

    cabot::String url = uiWindow().getCurrentPageUrl();

    // current page URL of UI window will be empty
    // if page has not been reached for some reason

    if (url.isEmpty())
    {
        // in that case, use URL given in startUiApp()
        url = m_ui_window_url;
    }

    return uiWindow().loadUrl(url);
}

void
OperaWindowManager::stopUiApp()
{
    CABOT_ASSERT(m_ui_window);

    uiWindow().terminateCurrentApp();
}

void
OperaWindowManager::setVisibleUiApp(bool visible)
{
    CABOT_ASSERT(m_ui_window);
    setVisible(uiWindow(), visible);
    setActive(uiWindow(),visible);
    setFocusable(uiWindow(), visible);
    if (visible)
        bringToFront(uiWindow());
    else
        sendToBack(uiWindow());
}

void
OperaWindowManager::setFocusToOpApp(bool focus)
{
#ifdef INCLUDE_OPAPP_SUPPORT
    CABOT_ASSERT(m_ui_window);
    
    OperaOpAppWindow& opapp_window = opAppWindow();
    
    setVisible(opapp_window, focus);
    setActive(opapp_window,focus);
    setFocusable(opapp_window, focus);
    
    if (focus)
    {
        opapp_window.bringToFront();
    }
    else
    {
        opapp_window.sendToBack();
    }
#else
    CABOT_UNUSED_PARAM(focus);
#endif
    
}

bool
OperaWindowManager::isOpAppFocused()
{
#ifdef INCLUDE_OPAPP_SUPPORT
    if (Profile_GetAstraHDPlusOpAppSupportEnabled())
    {
        OperaOpAppWindow& opapp_window = opAppWindow();
        return opapp_window.isFocused();
    }
    else
#endif        
    {
        return false;
    }
}

bool
OperaWindowManager::isOipfWindowSupported()
{
    return m_is_oipf_window_supported;
}

bool
OperaWindowManager::sendToBack(OperaWindow& window)
{
    return window.sendToBack();
}

bool
OperaWindowManager::bringToFront(OperaWindow& window)
{
    return window.bringToFront();
}

bool
OperaWindowManager::moveAbove(OperaWindow& window, OperaWindow& reference_window)
{
    return window.moveAbove(reference_window.handle());
}

bool
OperaWindowManager::moveBelow(OperaWindow& window, OperaWindow& reference_window)
{
    return window.moveBelow(reference_window.handle());
}

void
OperaWindowManager::notifyAppStateChange(OperaWindowHandle const& handle, ApplicationState app_state)
{
    auto callback = m_app_state_change_callback.load();
    if (callback)
    {
        //FrostMutexLock lock(m_list_mutex);
        ApplicationStateData data;
        data.state = app_state;
        if (getWindowByHandleLocked(handle))
        {
            strncpy(data.window_name, getWindowByHandleLocked(handle)->name().charArray(), WINDOW_NAME_LENGTH);
            data.window_name[WINDOW_NAME_LENGTH - 1] = '\0';
            callback(&data);
        }
    }
}

bool 
OperaWindowManager::isCurrentActiveWindowType(OperaWindow::WindowType window_type) const
{
    //FrostMutexLock lock(m_list_mutex);
    auto active_window = getActiveWindowLocked();
    if (active_window)
    {
        return (active_window->type() == window_type);
    }
    return false;
}

WebBrowserAppType 
OperaWindowManager::getCurrentActiveWindowAppType() const
{
    //FrostMutexLock lock(m_list_mutex);
    auto active_window = getActiveWindowLocked();
    return getWebBrowserAppType(active_window);
}

WebBrowserAppType 
OperaWindowManager::getWebBrowserAppType(OperaWindow* active_win)
{
    /**
     * Logic moved/refactored from OperaEngine::getCurrentAppType
     * This now calls this method
     */
    WebBrowserAppType active_app_type = WebBrowserAppType::hbbtv_app;
    if (active_win)
    {
        if (active_win->type() == OperaWindow::WindowType::window_csp ||
            active_win->type() == OperaWindow::WindowType::window_browser_ui)
        {
            active_app_type = WebBrowserAppType::user_app;
        }
        else if (active_win->type() == OperaWindow::WindowType::window_opapp)
        {
            active_app_type = WebBrowserAppType::op_app;
        }
        else if (active_win->isTeletextAppRunning())
        {
            active_app_type = WebBrowserAppType::hbbtv_txt_app;
        }
        else if (active_win->type() == OperaWindow::WindowType::window_media_player)
        {
            active_app_type = WebBrowserAppType::media_player;
        }
    }
    return active_app_type;
}

bool 
OperaWindowManager::isAppRunning(WebBrowserAppType const app_type) const
{
    //FrostMutexLock lock(m_list_mutex);
    auto active_windows = getActiveWindowsLocked();
    return std::any_of(active_windows.begin(), active_windows.end(),
        [app_type](OperaWindow*& window)
        {
            return (app_type == OperaWindowManager::getWebBrowserAppType(window));
        });
}

bool
OperaWindowManager::isAppRunning() const
{
    //FrostMutexLock lock(m_list_mutex);
    auto active_windows = getActiveWindowsLocked();

    for (auto window : active_windows)
    {
        OperaWindow* active_win = static_cast<OperaWindow*>(window);
        if (isActiveWindowAppRunning(active_win))
        {
            return true;
        }
    }
    return false;
}

bool
OperaWindowManager::isActiveWindowAppRunning(OperaWindow *active_win) const
{
    if (active_win && (active_win->type() != OperaWindow::WindowType::window_launcher))
    {
        cabot::String active_win_url = active_win->getCurrentPageUrl();
        if (active_win->type() != OperaWindow::WindowType::window_ui)
        {
            return true;
        }
        else
        {
            if (active_win->isVisible() && isTeFrontUrl(active_win_url))
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<WebBrowserAppType>
OperaWindowManager::runningAppTypes(bool first_match_only) const
{
    std::vector<WebBrowserAppType> running_app_types;

    auto windows = getActiveWindows();

    for (auto window : windows)
    {
        OperaWindow* active_win = static_cast<OperaWindow*>(window);

        if (active_win && (active_win->type() != OperaWindow::WindowType::window_launcher))
        {
            cabot::String active_win_url = active_win->getCurrentPageUrl();
            if (active_win->type() != OperaWindow::WindowType::window_ui)
            {
                running_app_types.push_back(getWebBrowserAppType(active_win));
                if (first_match_only)
                {
                    break;
                }
            }
            else
            {
                if (active_win->isVisible() && isTeFrontUrl(active_win_url))
                {
                    running_app_types.push_back(getWebBrowserAppType(active_win));
                    if (first_match_only)
                    {
                        break;
                    }
                }
            }
        }
    }
    return running_app_types;
}

bool
OperaWindowManager::isNumberSentFromRC()
{
    return m_is_number_key_sent_from_rc;
}

void
OperaWindowManager::setApplicationStateChangeCallback(ApplicationStateChangeCallback* callback)
{
    TRACE_INFO(("%s()\n", __func__));
    m_app_state_change_callback.store(callback);
}

AnyWebBrowserWindowManager::WindowType
OperaWindowManager::windowType(AnyWebBrowserWindowManager::WindowId window_id) const
{
    //FrostMutexLock lock(m_list_mutex);
    AnyWebBrowserWindowManager::WindowType type = AnyWebBrowserWindowManager::window_non_existent;
    OperaWindow* win = getWindowByHandleLocked(OperaWindowHandle(window_id));

    if (win)
    {
        switch(win->type())
        {
        case OperaWindow::WindowType::window_hbbtv:
            type = AnyWebBrowserWindowManager::hbbtv;
            break;

        case OperaWindow::WindowType::window_opapp:
            type = AnyWebBrowserWindowManager::opapp;
            break;

        default:
            type = AnyWebBrowserWindowManager::non_hbbtv;
            break;
        }
    }
    return type;
}

cabot::String
OperaWindowManager::pageUrl(WindowId window_id) const
{
    //FrostMutexLock lock(m_list_mutex);
    cabot::String url;

    OperaWindow* win = getWindowByHandleLocked(OperaWindowHandle(window_id));
    if (win)
    {
        url = win->getCurrentPageUrl();
    }

    return url;
}

bool
OperaWindowManager::isApplicationBroadcastRelated(AnyWebBrowserWindowManager::WindowId window_id) const
{
    //FrostMutexLock lock(m_list_mutex);
    OperaWindow* win = getWindowByHandleLocked(OperaWindowHandle(window_id));

    bool is_broadcast_related = false;
    if (win)
    {
        if (win->type() == OperaWindow::WindowType::window_hbbtv)
        {
            OperaHbbTVWindow* hbbtv_win = static_cast<OperaHbbTVWindow*>(win);
            is_broadcast_related = hbbtv_win->broadcastRelated();
        }
    }

    return is_broadcast_related;
}

void
OperaWindowManager::insertToListLocked(
        OperaWindow* window,
        VectorOfOperaWindowPtr& window_list,
        bool sort_by_priority_after_insertion)
{
    // Assumes m_list_mutex is locked.

    bool exists = false;
    for (WindowIterator it =  window_list.begin(); it != window_list.end(); ++it)
    {
        if ((*it) == window)
        {
            exists = true;
            break;
        }
    }

    if (!exists)
    {
        window_list.push_back(window);
        if (sort_by_priority_after_insertion)
        {
            sortListByPriority();
        }
    }
}

void
OperaWindowManager::insertToList(
        OperaWindow* window,
        VectorOfOperaWindowPtr& window_list,
        bool sort_by_priority_after_insertion)
{
    //FrostMutexLock lock(m_list_mutex);
    insertToListLocked(window, window_list, sort_by_priority_after_insertion);
}

bool
OperaWindowManager::removeFromListLocked(
        OperaWindow* window,
        VectorOfOperaWindowPtr& window_list)
{
    // Assumes m_list_mutex is locked.

    WindowIterator it =  window_list.begin();
    for (; it != window_list.end(); ++it)
    {
        if ((*it) == window)
        {
            window_list.erase(it);
            return true;
        }
    }
    return false;
}

bool
OperaWindowManager::removeFromList(OperaWindow* window, VectorOfOperaWindowPtr& window_list)
{
    //FrostMutexLock lock(m_list_mutex);
    return removeFromListLocked(window, window_list);
}

void
OperaWindowManager::sortListByPriority()
{
    PriorityComparer comparer;

    if (m_window_list.size() > 1)
    {
        cabotSort(m_window_list, comparer);
        for (unsigned int i = 0; i < m_window_list.size(); i++)
        {
            TRACE_INFO(("window[%d] name[%s], priority[%d]\n", i,
                        m_window_list[i]->name().charArray(),
                        m_window_list[i]->getEventDispatchPriority()));
        }
    }
}

bool
OperaWindowManager::isInWindowList(OperaWindow const* window)
{
    VectorOfOperaWindowPtr::iterator it =  m_window_list.begin();

    for(; it != m_window_list.end(); ++it)
    {
        if((*it) == window)
        {
            return true;
        }
    }
    return false;
}

void OperaWindowManager::setUsbKeyboardStateAttached(bool attached)
{
    m_usb_keyboard_plugged = attached;
    if(getActiveWindow())
    {
        getActiveWindow()->updateOperaInputMethod();
    }
}

bool OperaWindowManager::isUsbKeyboardPlugged() const
{
    return m_usb_keyboard_plugged;
}

void OperaWindowManager::setCurrentUrl(cabot::String url)
{
    m_current_url = url;
}

cabot::String OperaWindowManager::getCurrentUrl() const
{
    return m_current_url;
}

void OperaWindowManager::setStarturlOpened(bool value)
{
    m_is_start_url_opened = value;
}

bool OperaWindowManager::isStarturlOpened() const
{
    return m_is_start_url_opened;
}

void
OperaWindowManager::setAppVisibilityWithGivenName(cabot::String const& window, bool visible)
{
    OperaWindow* browser_win = getWindowByName(window);

    if (browser_win)
    {
        browser_win->setVisible(visible);
    }
}

void
OperaWindowManager::updateActiveWindowsLocked()
{
    // Note: Trace deliberately uses not-locked function name.
    TRACE_INFO(("updateActiveWindows()\n"));

    // Assumes m_list_mutex is locked.

    m_active_window_list.sortActiveList();
    updateFocus();
    TRACE_INFO(("Active window list: %s\n", m_active_window_list.toString().c_str()));
}

void
OperaWindowManager::updateActiveWindows()
{
    //FrostMutexLock lock(m_list_mutex);
    updateActiveWindowsLocked();
}

void
OperaWindowManager::updateUserAgentPreference()
{
    OperaWindow* hbbtv_window = static_cast<OperaHbbTVWindow*>(getWindowByName(OperaWindow::window_name_hbbtv));
    if (hbbtv_window != nullptr)
    {
        hbbtv_window->updateUserAgentPreference();
    }
#ifdef INCLUDE_OPAPP_SUPPORT
    auto opapp_window = getWindowByName(OperaWindow::window_name_opapp);
    if (opapp_window != nullptr)
    {
        opapp_window->updateUserAgentPreference();
    }
#endif
}

void
OperaWindowManager::onWindowFocusChanged(omi::evt::WindowFocusChanged window_focus_changed)
{
    TRACE_INFO(("Window focus changed: focus_lost %s, focus_gained %s\n",
                window_focus_changed.focus_lost.str().c_str(),
                window_focus_changed.focus_gained.str().c_str()));

    //FrostMutexLock lock(m_list_mutex);
    if (window_focus_changed.focus_lost.IsValid())
    {
        OperaWindow* window = getWindowByHandleLocked(window_focus_changed.focus_lost);
        if (window)
        {
            window->setFocus(false);
        }
    }
    if (window_focus_changed.focus_gained.IsValid())
    {
        OperaWindow* window = getWindowByHandleLocked(window_focus_changed.focus_gained);
        if (window)
        {
            window->setFocus(true);
        }
    }
}

bool
OperaWindowManager::startAdvertisementBrowser(cabot::String const& ad_url)
{
    stopCspApp();

    if (cspWindow().loadUrl(ad_url))
    {
        setVisible(cspWindow(), true);
        setActive(cspWindow(), true);
        return true;
    }
    return false;
}

void
OperaWindowManager::stopAdvertisementBrowser()
{
    cspWindow().terminateCurrentApp();
    setActive(cspWindow(), false);
}
