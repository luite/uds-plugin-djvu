/*
 * File Name: plugin_clone.h
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

#ifndef PLUGIN_CLONE_H_
#define PLUGIN_CLONE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Clone interface.
 * IPluginClone is used to clone the object.
 */
typedef struct
{

    /**
     * @brief Clone the object.
     * @param thiz The unknown interface of the source object.
     * @return Returns the interface IPluginUnknown of cloned object.
     */
    IPluginUnknown* (* create_clone_object)( IPluginUnknown *thiz );

} IPluginClone;

#ifdef __cplusplus
}
#endif 

#endif
