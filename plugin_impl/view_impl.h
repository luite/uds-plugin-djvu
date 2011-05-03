/*
 * File Name: view_impl.h
 */

/*
 * This file is part of uds-plugin-pdf.
 *
 * uds-plugin-pdf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-pdf is distributed in the hope that it will be useful,
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

#ifndef PDF_PLUGIN_VIEW_IMPL_H_
#define PDF_PLUGIN_VIEW_IMPL_H_

#include "plugin_inc.h"
#include "render_settings_impl.h"
#include "render_result_impl.h"
#include "interfaces_utils.h"
#include "listeners.h"

#include "pdf_page.h"
#include "pdf_renderer.h"

namespace pdf
{

class PluginDocImpl;

class PDFRenderAttributes;
/// @brief Implement all interfaces of view object. If view object
/// provider does not want to support optional interface, it may
/// remove the interface from parent class list.
class PluginViewImpl : public IPluginUnknown
                     , public IPluginView
                     , public IPluginViewSettings
                     , public IPluginRender
                     , public IPluginFont
                     , public IPluginEventBroadcaster
                     
{
public:
    explicit PluginViewImpl(PluginDocImpl *doc);
    ~PluginViewImpl(void);

public:
    utils::Signal<PluginViewImpl *> release_signal;

private:
    // IPluginUnknown
    static PluginStatus query_interface_impl(
        IPluginUnknown      *thiz,
        const UDSString     *id, 
        void                **ptr );

    static int release_impl(
        IPluginUnknown      *thiz );

    static int get_page_number_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor );

    static PluginStatus get_page_name_impl(
        IPluginUnknown        *thiz, 
        const UDSString       *page_start_anchor,
        UDSString             *name);

    static PluginStatus get_rendered_page_start_impl(
        IPluginUnknown       *thiz, 
        const UDSString      *anchor, 
        UDSString            *start_anchor );

    static PluginStatus get_physical_page_start_impl(
        IPluginUnknown       *thiz, 
        const UDSString      *anchor, 
        UDSString            *start_anchor );

    static int get_number_of_pages_impl(
        IPluginUnknown      *thiz);

    static PluginStatus get_anchor_by_page_impl(
        IPluginUnknown      *thiz,
        unsigned int        page,
        UDSString           *start_of_page_anchor);

    static PluginStatus get_prev_page_impl(
        IPluginUnknown      *thiz,
        UDSString           *start_of_page_anchor);

    static PluginStatus get_next_page_impl(
        IPluginUnknown *thiz,
        UDSString      *start_of_page_anchor);

    static PluginStatus get_cover_page_impl(
        IPluginUnknown *thiz,
        const int      width,
        const int      height,
        PluginBitmapAttributes *cover_page);

    static PluginStatus set_display_size_impl(
        IPluginUnknown      *thiz,
        const int           width,
        const int           height );

    static PluginStatus set_DPI_impl( 
        IPluginUnknown      *thiz,
        const unsigned int  dpi );

    static PluginStatus set_color_depth_impl(
        IPluginUnknown        *thiz,
        const unsigned int    color_depth );

    static PluginStatus render_impl(
        IPluginUnknown      *thiz,
        const UDSString     *start_of_page_anchor,
        const int           page_offset,
        IPluginUnknown      *settings,
        const RenderArea    *area, 
        const unsigned int  refId );

    static IPluginUnknown * create_render_settings_impl(
        IPluginUnknown      *thiz );

    static PluginStatus set_memory_limit_impl(
        IPluginUnknown       *thiz,   
        const unsigned int   bytes );

    static PluginStatus get_original_size_impl(
        IPluginUnknown       *thiz,
        const UDSString      *start_of_page_anchor,
        unsigned int         *width,
        unsigned int         *height );

    static PluginStatus get_page_content_area_impl(
        IPluginUnknown       *thiz,
        const UDSString      *start_of_page_anchor,
        RenderArea           *area );

    static PluginStatus get_original_rotation_impl(
        IPluginUnknown       *thiz,
        const UDSString      *start_of_page_anchor,
        PluginRotationDegree *rotation );
    
    static int get_font_size_impl(
        IPluginUnknown      *thiz );

    static PluginStatus set_font_size_impl(
        IPluginUnknown      *thiz, 
        const unsigned int  font_size );

    static PluginStatus get_font_family_impl(
        IPluginUnknown      *thiz, 
        UDSString           *font_family );

    static PluginStatus set_font_family_impl(
        IPluginUnknown      *thiz, 
        const UDSString     *font_family );

    static PluginStatus add_event_receiver_impl(
        IPluginUnknown      *thiz,
        const PluginEvent   plugin_event,
        EventFunc           callback,
        void                *user_data,
        unsigned long       *handler_id );

    static PluginStatus remove_event_receiver_impl(
        IPluginUnknown      *thiz,
        unsigned long       handler_id );

private:
    typedef PluginRenderSettingsImpl * RenderSettingsPtr;
    typedef std::vector<RenderSettingsPtr> RenderSettings;
    typedef std::vector<RenderSettingsPtr>::iterator RenderSettingsIter;

    typedef PluginRenderResultImpl * RenderResultPtr;
    typedef std::vector<RenderResultPtr> RenderResults;
    typedef std::vector<RenderResultPtr>::iterator RenderResultIter;

private:
    static utils::ObjectTable<PluginViewImpl> g_instances_table;

    // Reference to the document.
    PluginDocImpl    *document;
    RenderSettings   render_settings;
    RenderResults    render_results;
    utils::Listeners listeners;

    PDFRenderer      *renderer;

private:
    void send_render_request(int page_num,
                             const PDFRenderAttributes &page_attr,
                             const RenderArea *area,
                             const unsigned int  refId);

    // handle the "page ready" event
    // if cur_page is not null, it means this page has been rendered successfully
    // otherwise, the render task is aborted by someone error(out of memory)
    void handle_page_ready(RenderResultPtr result, RenderStatus stat);

    void on_render_settings_released(PluginRenderSettingsImpl * settings);
    void on_render_result_released(PluginRenderResultImpl * result);
};

};  // namespace pdf

#endif



