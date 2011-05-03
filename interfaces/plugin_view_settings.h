/*
 * File Name: plugin_view_settings.h
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

#ifndef PLUGIN_VIEW_SETTINGS_H_
#define PLUGIN_VIEW_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin View Settings interface.
 * Through IPluginViewSettings, callers can set the viewing-related attributes
 * to plugin. The viewing attribute set in this interface would affect the 
 * rendering of all pages in current document. And this attribute would affect
 * all kinds of the document formats. IPluginViewSettings currently supports:
 * - Display Size
 * - Dot per Inch(DPI)
 */
typedef struct
{
    /**
     * @brief Set display size of the view
     * @param thiz IPluginUnknown pointer of the view object.
     * @param Width of the display size to be set.
     * @param Height of the display size to be set.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_display_size)( IPluginUnknown *thiz,
                                       const int       width,
                                       const int       height );


    /**
     * @brief Set DPI.
     * @param thiz IPluginUnknown pointer of the view object
     * @param DIP to be set
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_DPI)( IPluginUnknown        *thiz,
                              const unsigned int    dpi );

     /**
     * @brief Set Color Depth.
     * @param thiz IPluginUnknown pointer of the view object
     * @param Color depth(by bits) to be set.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_color_depth)( IPluginUnknown        *thiz,
                                      const unsigned int    color_depth );

} IPluginViewSettings;

#ifdef __cplusplus
}
#endif 

#endif

