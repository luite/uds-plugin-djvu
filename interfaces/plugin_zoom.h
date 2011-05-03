/*
 * File Name: plugin_zoom.h
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

#ifndef PLUGIN_ZOOM_H_
#define PLUGIN_ZOOM_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Zoom interface
 * Through IPluginZoom, caller can set/get zoom factors to/from plugin.
 * The zoom factor used here are the ratio related to the whole page.
 * That means the value zone of a zoom factor is [0,1].
 */
typedef struct
{
    /**
     * @brief Set zoom factor.
     * @param thiz IPluginUnknown pointer to the render settings object.
     * @param zoom_factor Zoom factor to be set.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_zoom_factor)( IPluginUnknown* thiz,
                                      const float zoom_factor );

    /**
     * @brief Query zoom factor.
     * @param thiz IPluginUnknown pointer to the render settings object.
     * @return Zoom factor in float type.
     */
    float (* get_zoom_factor)( IPluginUnknown* thiz );

    /**
     * @brief Query maximal zoom factor
     * @param thiz IPluginUnknown pointer to the render settings object.
     * @return Maximal zoom factor in float type 
     */
    float (* get_max_zoom_factor)( IPluginUnknown* thiz );

    /**
     * @brief Query minimal zoom factor.
     * @param thiz IPluginUnknown pointer to the render settings object.
     * @return Minimal zoom factor in float type.
     */
    float (* get_min_zoom_factor)( IPluginUnknown* thiz );

} IPluginZoom;

#ifdef __cplusplus
}
#endif 

#endif
