/*
 * File Name: plugin_doc_hyperlink.h
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

#ifndef PLUGIN_HYPERLINK_H_
#define PLUGIN_HYPERLINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Document Hyperlink interafce. 
 * Through IPluginDocDictionary, UDS may be able to get explaination for 
 * certain word.
 */

typedef struct
{
    /**
     * @brief Check whether a range is a hyperlink.
     * @param thiz IPluginUnknown pointer of a document object.
     * @param range The range of the object.
     * @return PLUGIN_TRUE if range contains exactly a hyperlink;
     * otherwise return PLUGIN_FALSE.
     */
    PluginBool (* is_hyperlink)( IPluginUnknown    *thiz,
                                 const PluginRange *range );

    /**
     * @brief Get the anchor pointed by the hyperlink.
     * @param thiz IPluginUnknown pointer of the document object.
     * @param range The object range containing the hyperlink.
     * @param anchor The anchor of the target position pointed by the 
     * hyperlink.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_target_from_hyperlink)( IPluginUnknown     *thiz,
                                                const PluginRange  *range, 
                                                UDSString          *anchor );

    /**
     * @brief Retrieve hyper links inside specified range.
     * @param thiz IPluginUnknown pointer of document object.
     * @param range The range that caller wants to retrieve hyper links.
     * @return This function returns IPluginUnknown pointer of collection object.
     * The collection object contains all hyper links' range that inside the page
     * specified by the page_anchor. It returns NULL when the specified page does
     * not contain any hyper links.
     */
    IPluginUnknown * (* get_hyperlinks_in_range) ( IPluginUnknown      *thiz,
                                                   const PluginRange   *range );

} IPluginDocHyperlink;

#ifdef __cplusplus
}
#endif 

#endif

