/*
 * File Name: plugin_doc_search.h
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

#ifndef PLUGIN_DOC_SEARCHER_H_
#define PLUGIN_DOC_SEARCHER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Document Search interafce. 
 * Through IPluginDocumentSearch, caller is able to do text search through 
 * in document. 
 */
typedef struct 
{
    /** 
     * @brief Create the search criteria object for next search.
     * @param thiz IPluginUnknown pointer of IPluginDocSearcher object.
     * @return Returns IPluginUnknown pointer of a search criteria object
     * if it can successfully create the object; otherwise returns NULL.
     */
    IPluginUnknown * (* create_search_criteria)( IPluginUnknown  *thiz );

    /**
     * @brief Send a request to search next/prev string according to the 
     * search criteria.
     * @param thiz IPluginUnknown pointer of document object.
     * @param criteria The IPluginUnknown pointer of document search criteria
     * object created by the document object.
     * @param from_anchor The start position anchor. The plugin searches from
     * this position if it's initialized by caller before the function is 
     * invoked. If this value is empty, plugin would search from the beginning
     * of the document. When this function returns successfully, the position
     * of cursor will be adjusted to the start anchor of first matched text.
     * @return The search status of executing this request.
     */
    PluginStatus (* request_search_next)( IPluginUnknown      *thiz,
                                          IPluginUnknown      *criteria,
                                          const UDSString     *from_anchor,
                                          const unsigned int  search_id );

    /**
     * @brief Send a request of searching the objective string in the whole
     * document. The plugin will return list of all matched anchors.
     * @param thiz IPluginUnknown pointer of document object.
     * @param criteria The IPluginUnknown pointer of Plugin document search
     * criteria object.
     * @return The search status of executing this request
     */
    PluginStatus (* request_search_all)( IPluginUnknown         *thiz,
                                         IPluginUnknown         *criteria,
                                         const unsigned int     search_id);

    /**
     * @brief Abort search request that is identified by search_id.
     * @param thiz IPluginUnknown pointer of document object.
     * @param search_id The search task id that has been sent.
     * @return This function returns PLUGIN_OK if plugin can abort the 
     * search task. It returns other values if something wrong with plugin.
     * The possible reason can be invalid search id.
     */
    PluginStatus (* abort_search)( IPluginUnknown               *thiz,
                                   const unsigned int           search_id);

} IPluginDocSearch;

#ifdef __cplusplus
}
#endif 

#endif

