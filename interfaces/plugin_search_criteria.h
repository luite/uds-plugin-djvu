/*
 * File Name: plugin_search_criteria.h
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

#ifndef PLUGIN_SEARCH_CRITERIA_H_
#define PLUGIN_SEARCH_CRITERIA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Univeral Document Shell Plugin Document Search Criteria interface
 * Through IPluginSearchCriteria, caller can set search criteria on 
 * specified document object.
 */
typedef struct 
{
    /**
     * @brief Define the search text.
     * @param thiz The IPluginUnknown pointer of search criteria object.
     * @param text The string pattern. 
     * NOTE: This pattern MUST be UTF-8. 
     */
    PluginStatus (* set_search_text)( IPluginUnknown    *thiz, 
                                      const UDSString   *text ); 

    /** 
     * @brief Define whether the searching is case sensitive.
     * @param thiz The IPluginUnknown pointer to the IPluginSearchCriteria
     * object.
     * @param is_sensitive Specify whether the searching is case sensitive
     * or not. By default it is case insensitive.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_case_sensitive)( IPluginUnknown   *thiz, 
                                         const PluginBool is_sensitive );

    /** 
     * @brief Define whether searching matches the whole word or not.
     * @param thiz The IPluginUnknown pointer of the search criteria object.
     * @param do_match_word Specify whether match the whole word or not.
     * By default it should NOT match the whole word.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_match_whole_word)( IPluginUnknown   *thiz, 
                                           const PluginBool do_match_word );

    /** 
     * @brief Define whether search forward or not.
     * @param thiz The IPluginUnknown pointer of the search criteria object.
     * @param do_search_forward Specify whether support forward searching or
     * not.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* set_forward)( IPluginUnknown    *thiz, 
                                  const PluginBool  do_search_forward );

} IPluginSearchCriteria;

#ifdef __cplusplus
}
#endif 

#endif

