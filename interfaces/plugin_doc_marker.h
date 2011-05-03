/*
 * File Name: plugin_doc_marker.h
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

#ifndef PLUGIN_DOC_MARKER_H_
#define PLUGIN_DOC_MARKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Marker Types declaration.
 */
typedef enum
{
    MARKER_UDS_PRIVATE = 0,     /**< The private marker defined by UDS. */
    MARKER_TOC,                 /**< The marker of Table-Of-Content. */
    MARKER_BUILTIN_BOOKMARK     /**< The built-in bookmark. */
} PluginMarkerType;

/**
 * @brief Universal Document Shell Plugin Marker Entry declaration. 
 */
typedef struct _MarkerEntry
{
    PluginMarkerType    type;               /**< The marker type. */
    UDSString           *anchor;            /**< The target anchor. */
    UDSString           *text;              /**< The display text. */
    struct _MarkerEntry *sibling;           /**< The next sibling entry. */
    struct _MarkerEntry *first_child;       /**< The first child entry. */
    unsigned int        uds_private_size;   /**< uds_private size in bytes. */
    unsigned char       uds_private[1];     /**< uds-private data. */
} MarkerEntry;

/**
 * @brief Universal Document Shell Plugin Marker Interface.
 * Through IPluginDocMarker, caller can request marker tree of document to 
 * plugin, and get plugin-supported marker types.
 */
typedef struct 
{
    /**
     * @brief Retrieve all supported marker types. 
     * @param thiz IPluginUnknown pointer of document object.
     * @return Returns IPluginUnknown pointer of collection object that 
     * contains all supported marker types if the document object contains
     * any markers. Otherwise this function returns NULL.
     */
    IPluginUnknown* (* get_supported_marker_types)( IPluginUnknown  *thiz );

    /**
     * @brief Check the document contains table of content or not.
     * @param thiz IPluginUnknown pointer of document object.
     * @return Returns PLUGIN_TRUE if the document contains the table of content.
     * Otherwise this function returns PLUGIN_FALSE.
     */
    PluginBool (* has_toc)( IPluginUnknown  *thiz );

    /**
     * @brief Request the marker trees. The marker tree won't be returned 
     * immediately after calling this function, but returned asynchronously
     * by some callback functions of UDS.
     * @param thiz IPluginUnknown pointer of document object.
     * @param uds_private_size The size of extra data buffer that plugin has to 
     * allocate for every marker tree node.
     * @return TODO. Add return code here.
     */
    PluginStatus (* request_marker_trees)( IPluginUnknown       *thiz, 
                                           const unsigned int   uds_private_size );

} IPluginDocMarker;

#ifdef __cplusplus
}
#endif 

#endif

