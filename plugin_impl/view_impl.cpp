/*
 * File Name: view_impl.cpp
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

#include <cassert>
#include "view_impl.h"
#include "document_impl.h"

#include "pdf_library.h"

namespace pdf
{

utils::ObjectTable<PluginViewImpl> PluginViewImpl::g_instances_table;

PluginViewImpl::PluginViewImpl(PluginDocImpl *doc)
: document(doc)
, render_settings()
, render_results()
, listeners()
, renderer(doc->get_renderer())
{
    assert(document);

    // IPluginUnknown
    query_interface = query_interface_impl;
    release         = release_impl;

    // IPluginView
    get_page_number         = get_page_number_impl;
    get_page_name           = get_page_name_impl;
    get_rendered_page_start = get_rendered_page_start_impl;
    get_physical_page_start = get_physical_page_start_impl;
    get_number_of_pages     = get_number_of_pages_impl;
    get_anchor_by_page      = get_anchor_by_page_impl;
    get_prev_page           = get_prev_page_impl;
    get_next_page           = get_next_page_impl;
    get_cover_page          = get_cover_page_impl;

    // IPluginViewSettings
    set_display_size    = set_display_size_impl;
    set_DPI             = set_DPI_impl;
    set_color_depth     = set_color_depth_impl;

    // IPluginRender
    render                  = render_impl;
    create_render_settings  = create_render_settings_impl;
    set_memory_limit        = set_memory_limit_impl;
    get_original_size       = get_original_size_impl;
    get_page_content_area   = get_page_content_area_impl;
    get_original_rotation   = get_original_rotation_impl;

    // IPluginFont
    get_font_size   = get_font_size_impl;
    set_font_size   = set_font_size_impl;
    get_font_family = get_font_family_impl;
    set_font_family = set_font_family_impl;

    // IPluginEventBroadcaster
    add_event_receiver      = add_event_receiver_impl;
    remove_event_receiver   = remove_event_receiver_impl;

    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginView>(this);
    g_instances_table.add_interface<IPluginViewSettings>(this);
    g_instances_table.add_interface<IPluginRender>(this);
    g_instances_table.add_interface<IPluginEventBroadcaster>(this);

    // add the slots of handling render results onto the document
    document->doc_ctrl.sig_page_ready.add_slot(this
        , &PluginViewImpl::handle_page_ready);

}

PluginViewImpl::~PluginViewImpl(void)
{
    document->doc_ctrl.sig_page_ready.remove_slot(this
        , &PluginViewImpl::handle_page_ready);

    g_instances_table.remove(this);
}

PluginStatus 
PluginViewImpl::query_interface_impl(IPluginUnknown    *thiz,
                                     const UDSString   *id, 
                                     void              **ptr )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int 
PluginViewImpl::release_impl(IPluginUnknown  *thiz )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    instance->release_signal.safe_broadcast(instance);
    return 0;
}

int 
PluginViewImpl::get_page_number_impl(IPluginUnknown  *thiz,
                                     const UDSString *anchor )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    return instance->document->get_page_number(anchor->get_buffer(anchor));
}

int 
PluginViewImpl::get_number_of_pages_impl(IPluginUnknown *thiz)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    return instance->document->get_page_count();
}

PluginStatus 
PluginViewImpl::get_anchor_by_page_impl(IPluginUnknown *thiz,
                                        unsigned int page,
                                        UDSString *anchor)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    // Use first page as the initial anchor.
    std::string temp;
    if (instance->document->get_anchor_of_page(page, temp))
    {
        anchor->assign(anchor, temp.c_str());
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::get_prev_page_impl(IPluginUnknown *thiz,
                                   UDSString      *anchor)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    std::string temp(anchor->get_buffer(anchor));
    if (instance->document->get_prev_page(temp))
    {
        anchor->assign(anchor, temp.c_str());
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::get_next_page_impl(IPluginUnknown *thiz,
                                   UDSString      *anchor)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    std::string temp(anchor->get_buffer(anchor));
    if (instance->document->get_next_page(temp))
    {
        anchor->assign(anchor, temp.c_str());
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus
PluginViewImpl::get_cover_page_impl(IPluginUnknown *thiz,
                                    const int      width,
                                    const int      height,
                                    PluginBitmapAttributes *cover_page)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    if (instance->renderer->render_cover_page(width, height, cover_page))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::set_display_size_impl(IPluginUnknown *thiz,
                                      const int       width,
                                      const int       height )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    instance->renderer->get_view_attr().set_display_width(width);
    instance->renderer->get_view_attr().set_display_height(height);

    return PLUGIN_OK;
}

PluginStatus 
PluginViewImpl::set_DPI_impl(IPluginUnknown        *thiz,
                             const unsigned int    dpi )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    instance->renderer->get_view_attr().set_device_dpi_v(dpi);
    instance->renderer->get_view_attr().set_device_dpi_h(dpi);

    return PLUGIN_OK;
}

PluginStatus 
PluginViewImpl::set_color_depth_impl(IPluginUnknown *thiz,
                                     const unsigned int color_depth )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    instance->renderer->get_view_attr().set_color_depth(color_depth);

    return PLUGIN_OK;
}

PluginStatus 
PluginViewImpl::render_impl(IPluginUnknown      *thiz,
                            const UDSString     *start_of_page_anchor,
                            const int           page_offset,
                            IPluginUnknown      *settings,
                            const RenderArea    *area, 
                            const unsigned int  refId )
{
    // Must get proper page anchor, so no page offset allowed
    if (page_offset != 0)
    {
        return PLUGIN_NOT_SUPPORTED;
    }

    // support multi-thread
    // send render request
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    
    unsigned int page_num = instance->document->get_page_number(
        start_of_page_anchor->get_buffer(start_of_page_anchor));

    // construct the render setting attributes
    PluginRenderSettingsImpl * settings_obj = 
        PluginRenderSettingsImpl::query_instance(settings);
    
    // send render request
    instance->send_render_request(page_num, settings_obj->get_render_attr()
        , area, refId);
                              
    return PLUGIN_OK;
}

IPluginUnknown * 
PluginViewImpl::create_render_settings_impl(IPluginUnknown  *thiz)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    RenderSettingsPtr ptr = new PluginRenderSettingsImpl;
    if (ptr == 0)
    {
        return 0;
    }

    instance->render_settings.push_back(ptr);
    ptr->release_signal.add_slot(instance, 
        &PluginViewImpl::on_render_settings_released);
    return static_cast<IPluginUnknown *>(ptr);
}

PluginStatus 
PluginViewImpl::set_memory_limit_impl(IPluginUnknown       *thiz,
                                      const unsigned int   bytes)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (instance->document->get_doc_ctrl().set_memory_limit(bytes))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::get_original_size_impl(IPluginUnknown       *thiz,
                                       const UDSString      *start_of_page_anchor,
                                       unsigned int         *width,
                                       unsigned int         *height )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (instance->document->get_original_size(start_of_page_anchor->get_buffer(start_of_page_anchor),
                                              *width,
                                              *height))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus
PluginViewImpl::get_page_content_area_impl(IPluginUnknown       *thiz,
                                           const UDSString      *start_of_page_anchor,
                                           RenderArea           *area )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (instance->document->get_content_area(
        start_of_page_anchor->get_buffer(start_of_page_anchor),
        *area))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::add_event_receiver_impl(IPluginUnknown     *thiz,
                                        const PluginEvent  plugin_event,
                                        EventFunc          callback,
                                        void               *user_data,
                                        unsigned long      *handler_id )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (handler_id == 0)
    {
        return PLUGIN_FAIL;
    }

    *handler_id = 
        instance->listeners.add_listener(plugin_event, callback, user_data);
    return PLUGIN_OK;
}

PluginStatus 
PluginViewImpl::remove_event_receiver_impl(IPluginUnknown  *thiz,
                                           unsigned long   handler_id)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);
    if (instance->listeners.remove_listener(handler_id))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus
PluginViewImpl::get_original_rotation_impl(IPluginUnknown  *thiz,
                                           const UDSString *start_of_page_anchor,
                                           PluginRotationDegree *rotation)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    PluginRotationDegree res = instance->document->get_page_original_rotation(
        start_of_page_anchor->get_buffer(start_of_page_anchor) );

    if (res < 0)
    {
        return PLUGIN_FAIL;
    }

    *rotation = res;
    return PLUGIN_OK;
}

PluginStatus
PluginViewImpl::get_page_name_impl(IPluginUnknown        *thiz, 
                                   const UDSString       *page_start_anchor,
                                   UDSString             *name)
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    unsigned int page_num = instance->document->get_page_number(
        page_start_anchor->get_buffer(page_start_anchor));

    char buf[32] = {0};
    pdf_printf(buf, "%d", page_num);

    name->assign(name, buf);

    return PLUGIN_OK;
}

PluginStatus
PluginViewImpl::get_rendered_page_start_impl(IPluginUnknown    *thiz, 
                                          const UDSString      *anchor,
                                          UDSString            *start_anchor )
{
    PluginViewImpl *instance = g_instances_table.get_object(thiz);

    unsigned int page_num = instance->document->get_page_number(
        anchor->get_buffer(anchor));

    PDFAnchor param;
    param.page_num = page_num;
    //param.file_name = instance->document->get_file_name();
    start_anchor->assign(start_anchor, param.get_string().c_str());

    return PLUGIN_OK;
}


PluginStatus
PluginViewImpl::get_physical_page_start_impl(IPluginUnknown  *thiz,
                                             const UDSString *anchor,
                                             UDSString       *start_anchor)
{
    return get_rendered_page_start_impl(thiz, anchor, start_anchor);
}


void 
PluginViewImpl::on_render_settings_released(PluginRenderSettingsImpl *settings)
{
    RenderSettingsIter iter = std::find(render_settings.begin(),
                                        render_settings.end(),
                                        settings);
    if (iter != render_settings.end())
    {
        delete *iter;
        render_settings.erase(iter);
    }
}

void 
PluginViewImpl::on_render_result_released(PluginRenderResultImpl * result)
{
    RenderResultIter iter = std::find(render_results.begin(),
                                      render_results.end(),
                                      result);
    if (iter != render_results.end())
    {
        delete *iter;
        render_results.erase(iter);
    }
}

void PluginViewImpl::send_render_request(int page_num
                                         , const PDFRenderAttributes &page_attr
                                         , const RenderArea *area
                                         , const unsigned int  refId)
{
    // TODO. support clip rendering by input render area
    // 1. find the discarded render result object
    RenderResultPtr result = 0;
    RenderResultIter idx = render_results.begin();
    for (; idx != render_results.end(); ++idx)
    {
        RenderResultPtr p = *idx;
        if (p->is_discard())
        {
            result = p;
            break;
        }
    }

    // 2. reset the render result object
    if (result)
    {
        result->set_page_number(page_num);
        result->set_ref_id(refId);
        // reset the status to be "USABLE"
        result->set_discard(false);
    }
    else
    {
        result = new PluginRenderResultImpl(page_num, refId);

        // add the result into list
        render_results.push_back(result);
        result->release_signal.add_slot(this, &PluginViewImpl::on_render_result_released);
    }

    renderer->post_render_task(page_num, page_attr, result, refId);
}

void PluginViewImpl::handle_page_ready(RenderResultPtr result, RenderStatus stat)
{
    if (result == 0)
    {
        // the render result cannot be NULL
        return;
    }

    PluginEventAttrs attrs;

    // set reference id
    attrs.render_end.rid = result->get_ref_id();

    // set render result
    attrs.render_end.result = static_cast<IPluginUnknown *>(result);

    switch (stat)
    {
    case TASK_RENDER_DONE:
        {
            // render done
            attrs.render_end.status = RENDER_DONE;
        }
        break;
    case TASK_RENDER_OOM:
        {
            // out of memory
            attrs.render_end.status = RENDER_OUT_OF_MEMORY;

            WARNPRINTF("Tell UDS out of memory, task %ld is aborted", attrs.render_end.rid);
        }
        break;
    case TASK_RENDER_INVALID_PAGE:
        {
            // it is an invalid page
            attrs.render_end.status = RENDER_INVALID_PAGE;

            ERRORPRINTF("Cannot render an invalid page");
        }
    default:
        break;
    }

    listeners.broadcast(this, EVENT_RENDERING_END, &attrs);
}

// Functions that are not supported now
int 
PluginViewImpl::get_font_size_impl(IPluginUnknown   *thiz)
{
    return -1;
}

PluginStatus 
PluginViewImpl::set_font_size_impl(IPluginUnknown      *thiz,
                                   const unsigned int  font_size)
{
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::get_font_family_impl(IPluginUnknown    *thiz,
                                     UDSString         *font_family)
{
    return PLUGIN_FAIL;
}

PluginStatus 
PluginViewImpl::set_font_family_impl(IPluginUnknown    *thiz,
                                     const UDSString   *font_family)
{
    return PLUGIN_FAIL;
}

} // namespace pdf

