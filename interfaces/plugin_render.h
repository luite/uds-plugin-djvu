/*
 * File Name: plugin_render.h
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

#ifndef PLUGIN_RENDER_H_
#define PLUGIN_RENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Render area type definition.
 */
typedef struct
{
    float x_offset;     /**< Offset in X axis, represented by the ratio 
                        relatingto the width of the whole page */
    float y_offset;     /**< Offset in Y axis, represented by the ratio 
                        relating to the height of the whole page */
    float width;        /**< width of the render area, represented by the ratio
                        relating to the width of the whole page */
    float height;       /**< height of the render area, represented by the 
                        ratio relating to the height of the whole page */
} RenderArea;

/**
 * @brief Universal Document Shell Plugin Renderer interface.
 * Through IPluginRender, caller can:
 * - Render content according to anchor and renderer settings
 * - Create rendering settings
 * - Set memory limit
 */
typedef struct
{
    /**
     * @brief Render content. 
     * The plugin would check whether the content has been rendered when caller
     * call this function. The plugin can abort other job if necessary. Once 
     * the content has been successfully rendered, the plugin would notify the
     * caller through the listeners.
     * @param thiz     IPluginUnknown pointer of the current view object.
     * @param anchor   The content location where rendering starts.
     * @param offset   Number of pages to navigate away from the specified anchor
     * @param          before rendering can start.
     * @param settings The rendering settings associated with this render 
     * @param          behavior.
     * @param area     For fixed-page oriented document, this variable 
     * @param          specifies which part of page need to be rendered.
     * @param refId    A ID used to refer the rendering result, including
     * @param          rendered image, paragraph text and something else. It is
     * @param          generated by caller
     * @return PLUGIN_OK            : render request accepted
     * @return PLUGIN_OUT_OF_MEMORY : insufficient memory
     * @return PLUGIN_NOT_SUPPORTED : invalid request for this plugin,
     * @return                        e.g. plugin cannot handle page_offset != 0
     * @return PLUGIN_FAIL          : other problem
     */
    PluginStatus (* render)( IPluginUnknown             *thiz,
                             const UDSString            *start_of_page_anchor,
                             const int                  page_offset,
                             IPluginUnknown             *settings,
                             const RenderArea           *area, 
                             const unsigned int         render_id );

    /**
     * @brief Create rendering settings.
     * @param thiz IPluginUnknown pointer of the current view object.
     * @return Returns IPluginUnknown pointer of the created render settings 
     * object, returns NULL if it fails.
     */
    IPluginUnknown * (* create_render_settings)( IPluginUnknown  *thiz );

    /**
     * @brief Set memory limit for plugin renderer
     * @param thiz IPluginUnknown pointer of the current view object.
     * @param bytes The memory budget in bytes.
     * @return TODO. Add return code here.
     */
    PluginStatus (* set_memory_limit)( IPluginUnknown       *thiz,   
                                       const unsigned int   bytes );

    /**
     * @brief Get the original size in pixel of the page (zoom = 100%)
     * @param thiz IPluginUnknown pointer of the current view object.
     * @param start_of_page_anchor Anchor of the requested page.
     * @param width The returned width.
     * @param height The returned height.
     * @return TODO. Add return code here
     */
    PluginStatus (* get_original_size)( IPluginUnknown       *thiz,
                                        const UDSString      *start_of_page_anchor,
                                        unsigned int         *width,
                                        unsigned int         *height);

    /**
     * @brief Get the content area of a page.
     * @param thiz IPluginUnknown pointer of the current view object.
     * @param start_of_page_anchor Start anchor of the page.
     * @param area The content area of a page
     * @return Returns PLUGIN_OK if this function succeeds, otherwise returns PLUGIN_FAIL.
     */
    PluginStatus (* get_page_content_area)( IPluginUnknown       *thiz,
                                            const UDSString      *start_of_page_anchor,
                                            RenderArea           *area);

    /**
     * @brief Get the original orientation of the page.
     * @param thiz IPluginUnknown pointer of the current view object.
     * @param start_of_page_anchor Start anchor of the page.
     * @param rotation How the page was rotated before it was stored in the file,
     *                 e.g. Clockwise_Degrees_90 means the page was rotated 90 degrees
     *                 before it was stored in the file, so for properly displaying it
     *                 the page must be rotated (360 - 90) degrees clockwise
     * @return This function returns PLUGIN_OK if plugin can retrieve 
     *         the original rotation. Otherwise, it returns the PLUGIN_FAIL.
     */
    PluginStatus (* get_original_rotation)( IPluginUnknown       *thiz,
                                            const UDSString      *start_of_page_anchor,
                                            PluginRotationDegree *rotation );

} IPluginRender;

#ifdef __cplusplus
}
#endif 

#endif
