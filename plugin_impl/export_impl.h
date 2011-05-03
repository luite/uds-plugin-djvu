/*
 * File Name: export_impl.h
 */

/*
 * This file is part of uds-plugin-pdf.
 *
 * uds-plugin-pdf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-pdf is distributed in the hope that it will be useful,
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

#ifndef PLUGIN_EXPORT_IMPL_H_
#define PLUGIN_EXPORT_IMPL_H_

#include "plugin_unknown.h"

#ifdef GCC
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#else
#define PLUGIN_EXPORT __declspec( dllexport )
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Retrieve supported highest API version.
 */
PLUGIN_EXPORT  const char* get_version();

/**
 * @brief Construct plugin library instance and return the IPluginUnkonwn interface
 * pointer.
 */
PLUGIN_EXPORT  IPluginUnknown* create_plugin_library();

#ifdef __cplusplus
}
#endif 


#endif

