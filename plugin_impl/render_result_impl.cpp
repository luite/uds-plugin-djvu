/*
 * File Name: render_result_impl.cpp
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

#include "render_result_impl.h"
#include "render_settings_impl.h"
#include "collection_impl.h"

#include "pdf_renderer.h"
#include "pdf_doc_controller.h"
#include "pdf_page.h"

namespace pdf
{

utils::ObjectTable<PluginRenderResultImpl> PluginRenderResultImpl::g_instances_table;

PluginRenderResultImpl::PluginRenderResultImpl(const unsigned int page_num, const unsigned int id)
: page(0)
, page_number(page_num)
, ref_id(id)
, discard(false)
{
    // IPluginUnknown
    query_interface = query_interface_impl;
    release = release_impl;

    // IPluginRenderResult
    get_bitmap_attributes = get_bitmap_attributes_impl;
    get_anchor_from_coordinates = get_anchor_from_coordinates_impl;
    get_bounding_rectangles_from_range = get_bounding_rectangles_from_range_impl;
    get_rendered_range = get_rendered_range_impl;

    // IPluginRenderSettings, no method yet.

    // IPluginZoom
    set_zoom_factor = set_zoom_factor_impl;
    get_zoom_factor = get_zoom_factor_impl;
    get_max_zoom_factor = get_max_zoom_factor_impl;
    get_min_zoom_factor = get_min_zoom_factor_impl;

    // IPluginRotation
    set_rotation = set_rotation_impl;
    get_rotation = get_rotation_impl;

    // Initialize all supported interfaces. In render result, 
    // the render settings, zoom and rotation are read only.
    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginRenderResult>(this);
    g_instances_table.add_interface<IPluginRenderSettings>(this);
    g_instances_table.add_interface<IPluginZoom>(this);
    g_instances_table.add_interface<IPluginRotation>(this);

}

void PluginRenderResultImpl::set_page(PagePtr ptr)
{
    if (ptr != 0)
    {
        page = ptr;
        page_number = static_cast<unsigned int>(page->get_page_num());
        ref_id = page->get_ref_id();
        page->lock();
    }
}

PluginRenderResultImpl::~PluginRenderResultImpl(void)
{
    // unlock the pdf page but not delete it
    // the page would be deleted by cache manager
    if (page != 0)
    {
        page->unlock();
    }

    g_instances_table.remove(this);
}

PluginStatus
PluginRenderResultImpl::query_interface_impl(IPluginUnknown    *thiz,
                                             const UDSString   *id, 
                                             void              **ptr )
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int 
PluginRenderResultImpl::release_impl(IPluginUnknown  *thiz )
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);
    instance->release_signal.safe_broadcast(instance);
    return 0;
}

PluginStatus 
PluginRenderResultImpl::get_bitmap_attributes_impl(IPluginUnknown         *thiz, 
                                                   PluginBitmapAttributes *attributes)
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);

    if (instance->page == 0)
    {
        return PLUGIN_FAIL;
    }

    attributes->width = instance->page->get_bitmap_width();
    attributes->height = instance->page->get_bitmap_height();
    attributes->row_stride = instance->page->get_bitmap_row_stride();
    attributes->data = instance->page->get_bitmap_data();

    return PLUGIN_OK;
}

PluginStatus 
PluginRenderResultImpl::get_anchor_from_coordinates_impl(IPluginUnknown *thiz,
                                                 const int      x,
                                                 const int      y,
                                                 UDSString      *anchor )
{

    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);
    
    if (instance->page == 0)
    {
        return PLUGIN_FAIL;
    }

    double ux, uy;

    instance->page->coordinates_dev_to_user(x, y, &ux, &uy);

    PDFAnchor param;
    instance->page->get_anchor_param_from_coordinates(ux, uy, param);
    //param.file_name = instance->page->get_doc_controller()->name();
    anchor->assign(anchor, param.get_string().c_str());

    return PLUGIN_OK;
}

IPluginUnknown* 
PluginRenderResultImpl::get_bounding_rectangles_from_range_impl(IPluginUnknown      *thiz,
                                                                const PluginRange   *range)
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);

    if (instance->page == 0)
    {
        return 0;
    }

    PDFRectangles *rectangles = new PDFRectangles;

    // construct a start/end string
    string start(range->start_anchor->get_buffer(range->start_anchor));
    string end(range->end_anchor->get_buffer(range->end_anchor));
    if (instance->page->get_bounding_rectangles(start, end, *rectangles))
    {
        PluginCollectionImpl *collection = new PluginCollectionImpl;

        collection->set_data(rectangles);

        return static_cast<IPluginUnknown *>(collection);
    }

    return 0;
}

float 
PluginRenderResultImpl::get_zoom_factor_impl(IPluginUnknown* thiz )
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);

    if (instance->page == 0)
    {
        return static_cast<float>(PLUGIN_ZOOM_INVALID);
    }

    return static_cast<float>(instance->page->get_render_attr().get_zoom_setting());
}

float 
PluginRenderResultImpl::get_max_zoom_factor_impl(IPluginUnknown* thiz )
{
    // Even not necessary to get instance from thiz.
    // PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);
    return static_cast<float>(PDFRenderer::get_max_zoom());
}

float 
PluginRenderResultImpl::get_min_zoom_factor_impl(IPluginUnknown* thiz )
{
    // Even not necessary to get instance from thiz.
    // PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);
    return static_cast<float>(PDFRenderer::get_min_zoom());
}

PluginRotationDegree 
PluginRenderResultImpl::get_rotation_impl(IPluginUnknown  *thiz )
{
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);

    if (instance->page == 0)
    {
        return Clockwise_Degrees_0;
    }
    
    return static_cast<PluginRotationDegree>(
        instance->page->get_render_attr().get_rotate());
}

PluginStatus
PluginRenderResultImpl::get_rendered_range_impl(IPluginUnknown  *thiz,
                                                PluginRange     *range)
{
    // return the entire page now
    // TODO. return the accurate range in the future
    PluginRenderResultImpl *instance = g_instances_table.get_object(thiz);

    if (instance->page == 0)
    {
        return PLUGIN_FAIL;
    }

    int page_num = instance->page->get_page_num();

    PDFAnchor start_anchor;
    start_anchor.page_num = page_num;
    //start_anchor.file_name = instance->page->get_doc_controller()->name();

    PDFAnchor end_anchor;
    //end_anchor.file_name = instance->page->get_doc_controller()->name();
    end_anchor.set_end_anchor();

    range->start_anchor->assign( range->start_anchor, start_anchor.get_string().c_str() );
    range->end_anchor->assign( range->end_anchor, end_anchor.get_string().c_str() );

    return PLUGIN_OK;
}

// Not support
PluginStatus
PluginRenderResultImpl::set_zoom_factor_impl(IPluginUnknown* thiz,
                                             const float zoom_factor )
{
    return PLUGIN_FAIL;
}

PluginStatus 
PluginRenderResultImpl::set_rotation_impl(IPluginUnknown               *thiz,
                                          const PluginRotationDegree   rotation )
{
    return PLUGIN_FAIL;
}

} // namespace pdf

