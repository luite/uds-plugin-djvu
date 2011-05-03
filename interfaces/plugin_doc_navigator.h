/*
 * File Name: plugin_doc_navigator.h
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

#ifndef PLUGIN_DOC_NAVIGATOR_H_
#define PLUGIN_DOC_NAVIGATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Document Object Type declaration.
 */
typedef enum 
{
    PLUGIN_DOC_OBJECT_NONE,             /**< No object. */
    PLUGIN_DOC_OBJECT_TEXT,             /**< Text object. */
    PLUGIN_DOC_OBJECT_IMAGE,            /**< Image object. */
    PLUGIN_DOC_OBJECT_HYPERLINK,        /**< Hyperlink object. */
    PLUGIN_DOC_OBJECT_TEXT_HYPERLINK,   /**< Text Hyperlink object. */
    PLUGIN_DOC_OBJECT_PAGE,             /**< The page object. */
} PluginDocObjectType;

/**
 * @brief Universal Document Shell Plugin Document Object Navigator interafce.
 * Through IPluginDocNavigator, caller is able to navigate among different 
 * objects inside a document. Particularly, callers can get object(range) by 
 * input anchor; get text from specified range; compare position of two 
 * anchors.
 */
typedef struct 
{
    /**
     * @brief Get initial anchor of current document. The initial anchor is
     * normally the first page of text, or the table of content.
     * @param thiz IPluginUnknown pinter of document object.
     * @param anchor The returned anchor.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_initial_anchor)(IPluginUnknown *thiz, 
                                        UDSString      *anchor);

    /**
     * @brief Get the object where the anchor resides.
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor Location of user specified content.
     * @param range The range for returned object.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_object_from_anchor)( IPluginUnknown      *thiz,
                                             const UDSString     *anchor, 
                                             PluginRange         *range);

    /**
     * @brief Get the object type for a specified object.
     * @param thiz IPluginUnknown pointer of document object.
     * @param range The range of the specified object.
     * @return Type of the specific object.
     */
    PluginDocObjectType (* get_type_of_object)( IPluginUnknown      *thiz,
                                                const PluginRange   *range );

    /**
     * @brief Extends the range from specified char to the word.
     * @param thiz IPluginUnknown pointer of document object.
     * @param char_range The range of the specified character.
     * @param words_range The range of extended word.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_words_from_range)( IPluginUnknown       *thiz, 
                                          const PluginRange    *char_range, 
                                          PluginRange          *words_range );

    /**
     * @brief Get the text within the specific range.
     * @param thiz IPluginUnknown pointer of document object.
     * @param range User specified range.
     * NOTE: The corresponding object within the range should be text.
     * @param result The result text.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_text_from_range)( IPluginUnknown        *thiz, 
                                          const PluginRange     *range, 
                                          UDSString             *result );

    /** 
     * @brief Check whether the specified anchor is in the current document.
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor Location of user specified content.
     * @return Returns PLUGIN_TRUE if the anchor is in current document;
     * otherwise returns PLUGIN_FALSE.
     */
    PluginBool (* is_anchor_in_current_document)( IPluginUnknown    *thiz,
                                                  const UDSString   *anchor );

    /**
     * @brief Retrieve filename for given anchor. When the document is made up of
     * several files, the plugin should return the file name according to the anchor.
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor Location of user specified content.
     * @param file_name. The output variable stores the file name that
     * contains the object specified by anchor. The file name must be in UTF-8 with
     * extension name.
     * @return Returns PLUGIN_OK if plugin is able to get the file name from anchor
     * otherwise returns PLUGIN_ERROR.
     */
    PluginStatus (* get_file_name_from_anchor)( IPluginUnknown    *thiz,
                                                const UDSString   *anchor,
                                                UDSString         *file_name );

    /**
     * @brief Retrieve object position specified by anchor inside the file.
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor Location of user specified content.
     * @param position. The object position inside the file. The position must be
     * able to be compared. Usually the position is the distance of the object from
     * file beginning.
     * @return Returns PLUGIN_OK if plugin is able to get the object position specified
     * by anchor otherwise returns PLUGIN_ERROR.
     */
    PluginStatus (* get_file_position_from_anchor)( IPluginUnknown    *thiz,
                                                    const UDSString   *anchor,
                                                    signed long long  *position );

    /**
     * @brief Retrieve the url pointed by an anchor
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor The anchor which contains or points to a URL.
     * @param url The URL retrieved.
     * @return Returns PLUGIN_OK if plugin is able to get the URL specified
     * by anchor otherwise returns PLUGIN_ERROR.
     */
    PluginStatus (* get_uri_from_anchor)( IPluginUnknown    *thiz,
                                          const UDSString   *anchor,
                                          UDSString         *url );

    /**
     * @brief Compare position of two anchor objects.
     * @param thiz IPluginUnknown pointer of document object.
     * @param anchor_a The first anchor.
     * @param anchor_b The second anchor.
     * @return The possible return values are:
     * - <0 when anchor_a's location in the document is before anchor_b's 
     * location
     * - =0 when anchor_a's location in the document is the same as anchor_b's
     * location
     * - >0 when anchor_a's location in the document is after anchor_b's 
     * location
     */
    int (* compare_anchor_location) ( IPluginUnknown            *thiz,
                                      const UDSString           *anchor_a,
                                      const UDSString           *anchor_b );

} IPluginDocNavigator;

#ifdef __cplusplus
}
#endif 

#endif

