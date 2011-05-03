/*
 * File Name: plugin_render_result.h
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

#ifndef PLUGIN_RENDER_RESULT_H_
#define PLUGIN_RENDER_RESULT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Rendering bitmap structure used to communicate between plugin and UDS.
 * Struct derived from XImage. 
 * 
 */
typedef struct
{
    int width;                  /**< The width of image. */
    int height;                 /**< The height of image. */
    const unsigned char * data; /**< The image buffer data. */
    int row_stride;             /**< Number of bytes between the start of a row and the start of the next row.
                                     See also GdkPixBuf property rowstride. */ 
} PluginBitmapAttributes;

/**
 * @brief Render result interface, for querying bitmap inside render result
          get bounding rectangles for a specified range, and get the anchor
          for a specified point
 */
typedef struct
{
    /**
     * @brief Query bitmap attributes from render result
     * @param thiz IPluginUnknown interface pointer of the render result object
     * @param attributes output attributes
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_bitmap_attributes)( IPluginUnknown         *thiz, 
                                            PluginBitmapAttributes *attributes );

    /**
     * @brief Query content location for a specified point
     * @param thiz IPluginUnknown interface pointer of the render result object
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @param anchor output anchor
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_anchor_from_coordinates)( IPluginUnknown *thiz,
                                                  const int      x,
                                                  const int      y,
                                                  UDSString      *anchor );

    /**
     * @brief Get bounding rectangles for a specified range
     * @param thiz IPluginUnknown interface pointer of the render result object
     * @param range user specified range
     * @return IPluginUnknown interface of the collection object containing the array of bounding rectangles
     */
    IPluginUnknown* (* get_bounding_rectangles_from_range)( IPluginUnknown      *thiz,
                                                            const PluginRange   *range);

    /**
     * @brief Retrieve range from render result.
     * @param thiz The IPluginUnknown interface pointer of the render result object.
     * @param range The range of the render result.
     * @return This function returns PLUGIN_OK if it can retrieve the range of the
     * render result. Otherwise it returns the PLUGIN_FAIL.
     */
    PluginStatus (* get_rendered_range) ( IPluginUnknown  *thiz,
                                           PluginRange     *range );


} IPluginRenderResult;

#ifdef __cplusplus
}
#endif 

#endif
