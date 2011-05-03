/*
 * File Name: plugin_rotation.h
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

#ifndef PLUGIN_ROTATION_H_
#define PLUGIN_ROTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Univeral Document Shell Plugin Rotation interface
 * Through IPluginRotation, caller can set/get rotation value to/from 
 * plugin.
 */
typedef struct
{
    /**
     * @brief Set rotation direction
     * @param thiz IPluginUnknown pointer to the object that implements 
     * IPluginRotation.
     * @param rotation Rotation direction to be set.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_rotation)( IPluginUnknown               *thiz,
                                   const PluginRotationDegree   rotation );
    
    /**
     * @brief Get rotation status
     * @param thiz IPluginUnknown pointer to the object that implements 
     * IPluginRotation
     * @return Return current rotation value.
     */
    PluginRotationDegree (* get_rotation)( IPluginUnknown  *thiz );

} IPluginRotation;

#ifdef __cplusplus
}
#endif 

#endif

