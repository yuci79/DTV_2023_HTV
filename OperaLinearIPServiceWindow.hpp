/*
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Defines an helper class for Opera Windows specific to Linear IP Services.
 */
#ifndef CABOT_VEWD_INTEGRATION_CORE_OPERALINEARIPSERVICEWINDOW_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_CORE_OPERALINEARIPSERVICEWINDOW_HPP_INCLUDED

#include "OperaWindow.hpp"

/**
 * @brief Derived class of OperaWindow containing boolean holding whether or not a Linear IP Service
 */
class OperaLinearIPServiceWindow : public OperaWindow
{
public:
    /**
     * @brief Construct a new Opera Linear IP Service Window, params forwarded to OperaWindow
     * 
     * @param opera_engine 
     * @param window_name 
     * @param event_dispatch_priority 
     */
    OperaLinearIPServiceWindow(
        OperaEngine& opera_engine, 
        cabot::String window_name, 
        int event_dispatch_priority);

    ~OperaLinearIPServiceWindow() = default;

    /**
     * @brief Specifies whether the window is associated with an Linear IP Service
     * 
     * @param linear_ip_service whether of type linear ip service
     */
    void setLinearIPService(bool const linear_ip_service);

    /**
     * @brief Returns whether it is a linear IP Service
     */
    bool isLinearIPService() const;

    /**
     * @brief Gets the rectangular region of the video.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the video has a rectangular region set, false otherwise.
     */
    bool getVideoPosition(double &x,
                          double &y,
                          double &width,
                          double &height) const;

    /**
     * @brief Sets the rectangular region of the video.
     *
     * @param x Left of rectangle.
     * @param y Top of rectangle.
     * @param width Width of rectangle.
     * @param height Height of rectangle.
     *
     * @return true if the video rectangular region is set, false otherwise.
     */
    bool setVideoPosition(double x,
                          double y,
                          double width,
                          double height);

protected:
    void setInitialWindowPosition();
    
private:
    bool m_linear_ip_service = false;
};

#endif
