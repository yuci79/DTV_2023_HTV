/*
 * Copyright 2021 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines Dash PLayer Windows
 */

#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERADASHPLAYERWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERADASHPLAYERWINDOW_HPP_INCLUDED

#include "OperaLinearIPServiceWindow.hpp"

class OperaEngine;

/**
 * @brief used to launch a MPD file, it will generate a web page, and inject into VEWD using a data URLs
 * see  http://rdwiki/confluence/display/SWBINT/2021/03/15/MPD+playback
 * 
 */
class OperaDashPlayerWindow : public OperaLinearIPServiceWindow
{
public:
    /**
     * @brief Construct a new Opera Dash Player Window object
     * 
     * @param opera_engine Used to post messages / signal events.
     * @param event_dispatch_priority Priority of the window.
     * @param resource_id A unique id that represents the resource to use for playing media.
     */
    OperaDashPlayerWindow(OperaEngine& opera_engine, int event_dispatch_priority,
                          int resource_id);
    virtual ~OperaDashPlayerWindow();

    /**
     * @brief Check whether the supplied encoded url matches the internal unencoded url when encoded, and if it does populate
     * and return true else false
     * 
     * @param encoded_url to check
     * @param unencoded_url to populate 
     * @return whether a match found
     */
    bool checkUnEncodedUrl(cabot::String const encoded_url, cabot::String& unencoded_url) const;

public: // OperaWindow
    virtual bool loadUrl(cabot::String const& url,
                         RenderingResolution resolution = rendering_resolution_default);

    virtual void terminateCurrentApp();
    virtual OperaWindow::WindowType type() const;
    
private:
    /**
     * @brief Called from terminateCurrentApp, Destroys window and resets member variables
     */
    void stop();

    /**
     * @brief Generate data url to forward to the browser 
     * 
     * @param mpd_url to be part of the generated web page
     * @param data_url converted web page.
     * @param check_mpd_valid checks the mpd_url is valid (can block for limited time)
     * @return whether successful
     */
    bool generateDataUrl(cabot::String const mpd_url, cabot::String& data_url, bool check_mpd_valid) const;
    
    const OperaWindow::WindowType m_window_type;

    cabot::String m_unencoded_url;
};
#endif
