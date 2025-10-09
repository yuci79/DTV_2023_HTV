/*
 * Copyright 2022 Vestel.  All rights reserved.
 *
 * @file
 * @brief Implements the OperaLinearIPServiceWindow class.
 */

#include "OperaLinearIPServiceWindow.hpp"
#include "vewd_integration/mumin/OperaMuminHelper.hpp"
#include "OperaEngine.hpp"

#include "utilities_debug/trace.h"

TRACE_IMPLEMENT("opera/core/operalinearipservicewindow");

OperaLinearIPServiceWindow::OperaLinearIPServiceWindow(
    OperaEngine& opera_engine, 
    cabot::String window_name, 
    int event_dispatch_priority)
: OperaWindow(opera_engine, window_name, event_dispatch_priority)
{
}

void 
OperaLinearIPServiceWindow::setLinearIPService(bool const linear_ip_service)
{
    TRACE_INFO(("Setting to '%s' - changing status : '%s' type = '%s'\n",
        (linear_ip_service ? "true" : "false"),
        (linear_ip_service == m_linear_ip_service ? "false" : "true"),
        name().c_str()));

    m_linear_ip_service = linear_ip_service;
}

bool 
OperaLinearIPServiceWindow::isLinearIPService() const
{
    TRACE_VERBOSE(("Linear IP Service is '%s' type = '%s'\n",
        (m_linear_ip_service ? "true" : "false"),
        name().c_str()));

    return m_linear_ip_service;
}

bool
OperaLinearIPServiceWindow::getVideoPosition(double& x,
                                             double& y,
                                             double& width,
                                             double& height) const
{
    TRACE_INFO(("%s()\n", __func__));

    return isLinearIPService() && OperaWindow::getPosition(x, y, width, height);
}

bool
OperaLinearIPServiceWindow::setVideoPosition(double x,
                                             double y,
                                             double width,
                                             double height)
{
    TRACE_INFO(("%s(): x=%f, y=%f, width=%f, height=%f\n", __func__, x, y, width, height));
    
    return isLinearIPService() && OperaWindow::setPosition(x, y, width, height);
}

void
OperaLinearIPServiceWindow::setInitialWindowPosition()
{
    const auto nebula_window = operaRectToNebulaWindow(0,
                                                       0,
                                                       m_opera_engine.getPlaneWidth(),
                                                       m_opera_engine.getPlaneHeight(),
                                                       NEBULA_aspect_ratio_unknown);

    OperaWindow::setPosition(nebula_window.x_position,
                             nebula_window.y_position,
                             nebula_window.width,
                             nebula_window.height);
}