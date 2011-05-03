/*
 * File Name: plugin_doc_attributes.h
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

#ifndef PLUGIN_DOC_ATTRIBUTES_H_
#define PLUGIN_DOC_ATTRIBUTES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Document Attributes interface. 
 * Through IPluginDocAttributes, UDS can retrieve and change following
 * document attributes:
 * - The attribute that indicates whether this document is DRM protected
 * or not.
 * - The attribute that indicates whether this document is reflowable or 
 * fixed-size page-orientated.
 * - The attribute indicating that this document needs to specfiy correct
 * encoding when it's opened.
 * - The attribte that inidcaties this document needs to specify PASSWORD.
 */

typedef struct 
{
    /**
     * @brief Get document attributes.
     * @param thiz IPluginUnknown pointer of the current document object.
     * @param key Name of document attribute.
     * @param value Output variable of document attribute.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_attribute)( IPluginUnknown     *thiz,
                                    const UDSString    *key, 
                                    UDSString          *value );

    /**
     * @brief Set document attributes.
     * @param thiz IPluginUnknown pointer of the current document object
     * @param key Name of document attribute.
     * @param value Document attribute to be set.
     * @return TODO. Add return code here.
     */
    PluginStatus (* set_attribute)( IPluginUnknown     *thiz,
                                    const UDSString    *key,
                                    const UDSString    *value );


} IPluginDocAttributes;

#ifdef __cplusplus
}
#endif 

#endif

