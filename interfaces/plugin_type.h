/*
 * File Name: plugin_type.h
 */

/*
 * This file is part of uds-plugin-common.
 *
 * uds-plugin-common is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-common is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Copyright (C) 2008 iRex Technologies B.V.
 * All rights reserved.
 */

#ifndef PLUGIN_TYPES_H_
#define PLUGIN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uds_string.h"

/**
 * @brief PluginBool is the basic plugin boolean type.
 */
typedef enum
{
    PLUGIN_FALSE = 0,
    PLUGIN_TRUE  = 1
} PluginBool;

/**
 * @brief PluginRange in [] format identifies a document object.
 */
typedef struct
{
    UDSString* start_anchor;
    UDSString* end_anchor;
} PluginRange;

typedef struct
{
    int x;          /**< The x coordinate of top left point */
    int y;          /**< The y coordinate of top left point */
    int width;      /**< The width of the rectangle         */
    int height;     /**< The height of the rectangle        */
} PluginRectangle;

/**
 * @brief Rotation direction definition
 */
typedef enum
{
    Clockwise_Degrees_0 = 0,        /**< Rotate 0 degree */
    Clockwise_Degrees_90 = 90,      /**< Rotate 90 degrees */
    Clockwise_Degrees_180 = 180,    /**< Rotate 180 degrees */
    Clockwise_Degrees_270 = 270,    /**< Rotate 270 degrees */
    Clockwise_Degrees_360 = 360     /**< Rotate 360 degrees */
} PluginRotationDegree;

#ifdef __cplusplus
}
#endif 

#endif

