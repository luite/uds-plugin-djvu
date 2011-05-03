/*
 * File Name: plugin_export.h
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

#ifndef PLUGIN_EXPORT_H_
#define PLUGIN_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Global export functions from plugin shared library
 * These functions are invoked by Universal Document Shell.
 */

/**
 * @brief Function prototype for caller to retrieve API version that Plugin
 * provides.
 * @return The version string.
 */
typedef const char* (* GetVersionFunc)();

/**
 * @brief Function prototype to create plugin object.
 * Through this export function, caller is able to construct the plugin 
 * library.
 */
typedef IPluginUnknown* (* CreateLibFunc)();

#ifdef __cplusplus
}
#endif 

#endif

