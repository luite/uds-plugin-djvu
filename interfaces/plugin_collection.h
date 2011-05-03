/*
 * File Name: plugin_collection.h
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

#ifndef PLUGIN_COLLECTION_H_
#define PLUGIN_COLLECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Collection interface.
 * Through IPluginCollection, caller can access the elements of data collection
 * returned by plugin.
 */
typedef struct 
{
    /**
     * @brief Retrieve start address of the collection data.
     * @param thiz IPluginUnknown pointer of collection object.
     * @param data_ptr The output variable that contains the start address of 
     * the collection data.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_data)( IPluginUnknown   *thiz,
                               void             **data_ptr);

    /**
     * @brief Retrieve the number of elements.
     * @param thiz IPluginUnknown pointer of collection object .
     * @return This function returns the number of elements in the collection. 
     */
    int (* get_num_elements)( IPluginUnknown   *thiz );
                           
} IPluginCollection;

#ifdef __cplusplus
}
#endif 

#endif

