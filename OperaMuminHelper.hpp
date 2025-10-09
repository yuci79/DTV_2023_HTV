/**
 * Copyright 2018 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file defines some utility functions used in the mumin module
 */

#ifndef CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINHELPER_HPP_INCLUDED
#define CABOT_VEWD_INTEGRATION_MUMIN_OPERAMUMINHELPER_HPP_INCLUDED

#include "nebula/adaptation_layer/nebula_av_control.h"
#include "uva_backend.h"

#include "utilities_public/UString.hpp"

/**
 * @brief Convert from Opera coordinate space to Nebula space
 */
NEBULA_DisplayWindow operaRectToNebulaWindow(int x,
                                             int y,
                                             int w,
                                             int h,
                                             const NEBULA_VideoAspectRatio aspect_ratio);

/**
 * @brief Convert from Nebula coordinate space to Opera space
 */
void nebulaWindowToOperaRect(double x,
                             double y,
                             double width,
                             double height,
                             int& opera_x,
                             int& opera_y,
                             int& opera_width,
                             int& opera_height);
                             
/**
 * @brief Generate a string representation of a Nebula Display Window
 */
UString toStringNebulaWindow(NEBULA_DisplayWindow const& win);

/**
 * @brief Compare two NEBULA_DisplayWindows for equality
 */
bool operator==(NEBULA_DisplayWindow const& lhs, NEBULA_DisplayWindow const& rhs);

/**
 * @brief Compare two NEBULA_DisplayWindows for inequality
 */
bool operator!=(NEBULA_DisplayWindow const& lhs, NEBULA_DisplayWindow const& rhs);

#endif
