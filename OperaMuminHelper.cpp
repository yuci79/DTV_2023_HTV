/**
 * Copyright 2018 Vestel.  All rights reserved.
 *
 * @file
 *
 * @brief This file contains the implementations for utility functions used in the Mumin module
 */

#include "OperaMuminHelper.hpp"
#include "vewd_common/bpi/bpi_graphics.hpp"

#include "utilities_debug/trace.h"
TRACE_IMPLEMENT("mumin/helper");

namespace
{

const float sd_video_width = 720.0;
const float sd_video_height = 576.0;

const frost_uint browser_max_video_width_default = 1920;
const frost_uint browser_max_video_height_default = 1080;

void
adjustVideoAspectRatio(NEBULA_VideoAspectRatio const aspect_ratio, int& x, int& y, int& w, int& h)
{
    double window_ratio = (double)w/h;

    double ar = 1.777777778;
    if (aspect_ratio == NEBULA_aspect_ratio_4_3)
    {
        ar = 1.333333333;
    }

    if (window_ratio > 1.8)
    {
        TRACE_INFO(("%s() ar: %f, window_ratio: %f\n", __FUNCTION__, ar, window_ratio));

        double new_w = h * ar;
        double new_x = x + double(w - new_w) / 2;

        x = static_cast<int>(new_x);
        w = static_cast<int>(new_w);

        TRACE_INFO(("%s() (%d, %d, %d, %d)\n", __FUNCTION__, x, y, w, h));
    }
    else if (window_ratio < 1.3)
    {
        TRACE_INFO(("%s() ar: %f, window_ratio: %f\n", __FUNCTION__, ar, window_ratio));

        double new_h = (double)w / ar;
        double new_y = y + double(h - new_h) / 2;

        y = static_cast<int>(new_y);
        h = static_cast<int>(new_h);
    }
}

}   // namespace

bool operator==(NEBULA_DisplayWindow const& lhs, NEBULA_DisplayWindow const& rhs)
{
    if ( (lhs.x_position == rhs.x_position) &&
         (lhs.y_position == rhs.y_position) &&
         (lhs.width      == rhs.width) &&
         (lhs.height     == rhs.height)
       )
    {
        return true;
    }
    return false;
}

bool operator!=(NEBULA_DisplayWindow const& lhs, NEBULA_DisplayWindow const& rhs)
{
    return !(lhs == rhs);
}

NEBULA_DisplayWindow operaRectToNebulaWindow(int x,
                                             int y,
                                             int w,
                                             int h,
                                             const NEBULA_VideoAspectRatio aspect_ratio)
{
    NEBULA_DisplayWindow nebula_window;

    frost_uint browser_max_video_width = browser_max_video_width_default;
    frost_uint browser_max_video_height = browser_max_video_height_default;
    //BPI_GraphicsGetNativeSize(&browser_max_video_width, &browser_max_video_height);

    ::adjustVideoAspectRatio(aspect_ratio, x, y, w, h);

    nebula_window.x_position = (float)x * sd_video_width / (float)browser_max_video_width;
    nebula_window.y_position = (float)y * sd_video_height / (float)browser_max_video_height;
    nebula_window.width      = (float)w * sd_video_width / (float)browser_max_video_width;
    nebula_window.height     = (float)h * sd_video_height / (float)browser_max_video_height;

    return nebula_window;
}

void nebulaWindowToOperaRect(double x,
                             double y,
                             double width,
                             double height,
                             int& opera_x,
                             int& opera_y,
                             int& opera_width,
                             int& opera_height)
{
    frost_uint browser_max_video_width = browser_max_video_width_default;
    frost_uint browser_max_video_height = browser_max_video_height_default;
    //BPI_GraphicsGetNativeSize(&browser_max_video_width, &browser_max_video_height);

    opera_x = static_cast<int>(x * (float)browser_max_video_width / sd_video_width);
    opera_y = static_cast<int>(y * (float)browser_max_video_height / sd_video_height);
    opera_width = static_cast<int>(width * (float)browser_max_video_width / sd_video_width);
    opera_height = static_cast<int>(height * (float)browser_max_video_height / sd_video_height);
}

UString toStringNebulaWindow(NEBULA_DisplayWindow const& win)
{
    UString s = "(";
    s.append(UString::fromInt(static_cast<int>(win.x_position)));
    s.append(", ");
    s.append(UString::fromInt(static_cast<int>(win.y_position)));
    s.append(", ");
    s.append(UString::fromInt(static_cast<int>(win.width)));
    s.append(", ");
    s.append(UString::fromInt(static_cast<int>(win.height)));
    s.append(")");

    return s;
}
