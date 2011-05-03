/*
 * File Name: plugin_doc_dictionary.h
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

#ifndef PLUGIN_DOC_DICTIONARY_H_
#define PLUGIN_DOC_DICTIONARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Dictionary interafce. 
 * Through IPluginDocDictionary, UDS may be able to get explaination for 
 * certain word.
 */
typedef struct 
{
    /**
     * @brief Check whether the document is a dictionary.
     * @param thiz IPluginUnknown pointer of the document object.
     * @return Return PLUGIN_TRUE if this document is a dictionary;
     * otherwise return PLUGIN_FALSE.
     */
    PluginBool (* is_dictionary)( IPluginUnknown  *thiz );

} IPluginDocDictionary;

#ifdef __cplusplus
}
#endif 

#endif

