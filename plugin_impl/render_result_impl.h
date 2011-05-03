/*
 * File Name: render_result_impl.h
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

#ifndef PDF_PLUGIN_RENDER_RESULT_IMPL_H_
#define PDF_PLUGIN_RENDER_RESULT_IMPL_H_

#include "plugin_inc.h"
#include "interfaces_utils.h"
#include "signal_slot.h"
#include "pdf_page.h"

using namespace utils;

namespace pdf
{

class PluginRenderSettingsImpl;

class PluginRenderResultImpl : public IPluginUnknown
                             , public IPluginRenderResult
                             , public IPluginRenderSettings
                             , public IPluginZoom
                             , public IPluginRotation
{
public:
    PluginRenderResultImpl(const unsigned int page_num, const unsigned int id);
    ~PluginRenderResultImpl(void);

public:
    utils::Signal<PluginRenderResultImpl *> release_signal;

public:
    void set_page(PagePtr ptr);
    PagePtr get_page() {return page;}

    void set_discard(bool s) {discard = s;}
    bool is_discard() {return discard;}

    void set_page_number(const unsigned int n) {page_number = n;}
    unsigned int get_page_number() {return page_number;}
    void set_ref_id(const unsigned int r) {ref_id = r;}
    unsigned int get_ref_id() {return ref_id;}

private:
    // IPluginUnknown
    static PluginStatus query_interface_impl(
        IPluginUnknown    *thiz,
        const UDSString   *id, 
        void              **ptr );

    static int release_impl(
        IPluginUnknown  *thiz );

    // IPluginRenderResult 
    static PluginStatus get_bitmap_attributes_impl(
        IPluginUnknown         *thiz, 
        PluginBitmapAttributes *attributes);

    static PluginStatus get_anchor_from_coordinates_impl(
        IPluginUnknown *thiz,
        const int      x,
        const int      y,
        UDSString      *anchor );

    static IPluginUnknown* get_bounding_rectangles_from_range_impl(
        IPluginUnknown      *thiz,
        const PluginRange   *range);

    static PluginStatus get_rendered_range_impl(
        IPluginUnknown  *thiz,
        PluginRange     *range);

    // IPluginZoom
    static PluginStatus set_zoom_factor_impl(
        IPluginUnknown* thiz,
        const float zoom_factor );

    static float get_zoom_factor_impl(
        IPluginUnknown* thiz );

    static float get_max_zoom_factor_impl(
        IPluginUnknown* thiz );

    static float get_min_zoom_factor_impl(
        IPluginUnknown* thiz );

    // IPluginRotation
    static PluginStatus set_rotation_impl(
        IPluginUnknown               *thiz,
        const PluginRotationDegree   rotation );
    
    static PluginRotationDegree get_rotation_impl(
        IPluginUnknown  *thiz );

private:
    static utils::ObjectTable<PluginRenderResultImpl> g_instances_table;

    // reference to a PDFPage, it can be null
    PagePtr page;

    // page number
    unsigned int page_number;

    // reference id
    unsigned int ref_id;

    // flag indicating whether the render result is discarded or not
    bool discard;

    friend class PDFPage;
};

typedef PluginRenderResultImpl * RenderResultPtr;

};  // namespace pdf

#endif

