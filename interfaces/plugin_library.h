/*
 * File Name: plugin_library.h
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

#ifndef PLUGIN_LIBRARY_H_
#define PLUGIN_LIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Library interface.
 * Through IPluginLibrary, caller is able to:
 * - Check the library whether can it open the specified document or not.
 * - Create a new document object if the plug-in uses the low level interfaces.
 *    and retrieve the IPlugUnknown pointer of the library object.
 * - Create a new reader object if the plug-in uses the high level interfaces.
 */
typedef struct 
{

    /**
     * @brief Check this plugin library whether can it open specified document
     * or not.
     * @param thiz The IPluginUnknown pointer of plugin library.
     * @param path The document path.
     * @return Returns PLUGIN_TRUE if the specified document can be imported 
     * successfully, otherwise returns PLUGIN_FALSE.
     */
    PluginBool (* is_supported_document)( IPluginUnknown    *thiz, 
                                          const UDSString   *path );

    /**
     * @brief Create an empty document object it the plug-in uses the low level interfaces. 
     * It can import concrete document in succeed.
     * @param thiz The IPluginUnknown pointer of library object.
     * @return Returns IPluginUnknown pointer of document object creating 
     * successes, otherwise returns NULL.
     */
    IPluginUnknown * (* create_document)( IPluginUnknown * thiz );

} IPluginLibrary;

#ifdef __cplusplus
}
#endif 

#endif

