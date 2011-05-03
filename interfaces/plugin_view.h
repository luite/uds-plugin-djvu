/*
 * File Name: plugin_view.h
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

#ifndef PLUGIN_VIEW_H_
#define PLUGIN_VIEW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"
#include "plugin_render_result.h"

/**
 * @brief Universal Document Shell Plugin View interface.
 */
typedef struct
{
    /**
     * @brief Get page number by a specified anchor.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param anchor Location of user specified content.
     * @return The page location of the specified anchor. The location is
     * represented by number.
     */
    int (* get_page_number)( IPluginUnknown              *thiz,
                             const UDSString             *anchor );

    /**
     * @brief Retrieve page name by a specified anchor.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param page_start_anchor The start anchor of the page.
     * @param name It serves as output parameter. The returned page name
     * is stored in the name.
     *
     * @return This function returns PLUGIN_OK if plugin can store the 
     * page name into the @name. Otherwise it returns false.
     * This function is useful for image plugin when UDS works in 
     * thumbnail mode. UDS can display the file name instead of page number.
     */
    PluginStatus (*get_page_name)( IPluginUnknown        *thiz, 
                                   const UDSString       *page_start_anchor,
                                   UDSString             *name);

    /**
     * @brief Returns an anchor pointing to the start of the rendered page
     * 
     * @param thiz         [in] IPluginUnknown pointer of view object.
     * @param anchor       [in] The anchor of the requested page
     * @param start_anchor [out] Anchor pointing to the start of the rendered page
     *
     * @return PLUGIN_OK If plugin can find the page that contain the object specified by range.
     *                   It returns other values if something wrong with
     */
    PluginStatus (* get_rendered_page_start)( IPluginUnknown    *thiz, 
                                           const UDSString      *anchor, 
                                           UDSString            *start_anchor );

    /**
     * @brief Returns an anchor poiting to the start of the physical page
     *
     * When the plugin renders only physical pages, this function doesn't have to be implementated. The
     * default implementation  willcall get_rendered_page_start()
     *
     * @param thiz IPluginUnknown pointer of view object.
     * @param start_anchor [out] Anchor pointing to the start of the physical page
     * @return PLUGIN_OK Iff plugin can find the page that contain the object specified by range.
     *         It returns other values if something wrong with
     */
    PluginStatus (* get_physical_page_start)( IPluginUnknown       *thiz, 
                                              const UDSString      *anchor, 
                                              UDSString            *start_anchor );


    /**
     * @brief Get the total number of the pages rendered by this view object.
     * @param thiz IPluginUnknown pointer of the view object.
     * @return The number of total pages rendered by this view object.
     */
    int (* get_number_of_pages) ( IPluginUnknown    *thiz );

    /**
     * @brief Get anchor position corresponding to the specified page.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param page The specified page number.
     * @param start_of_page_anchor The returned anchor position.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_anchor_by_page)( IPluginUnknown   *thiz,
                                         unsigned int     page,
                                         UDSString        *start_of_page_anchor );

    /**
     * @brief Get the anchor for the beginning position of the previous page.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param start_of_page_anchor The returned anchor position.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_prev_page)( IPluginUnknown  *thiz,
                                    UDSString       *start_of_page_anchor );

    /**
     * @brief Get the anchor for the beginning position of the next page.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param start_of_page_anchor The returned anchor position.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_next_page)( IPluginUnknown *thiz,
                                    UDSString       *start_of_page_anchor );

    /**
     * @brief Get the cover page of document.
     * @param thiz IPluginUnknown pointer of the view object.
     * @param width The desired cover page width.
     * @param height The desired cover page height.
     * @param cover_page The result image of cover page.
     * @return This function returns PLUGIN_OK if view object is able to
     * render the cover page successfully, otherwise it returns PLUGIN_FAIL.
     */
    PluginStatus (* get_cover_page)( IPluginUnknown *thiz,
                                     const int      width,
                                     const int      height,
                                     PluginBitmapAttributes *cover_page);

} IPluginView;

#ifdef __cplusplus
}
#endif 

#endif

