/*
 * File Name: render_settings_impl.h
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

#ifndef PDF_PLUGIN_RENDER_SETTINGS_IMPL_H_
#define PDF_PLUGIN_RENDER_SETTINGS_IMPL_H_

#include <vector>
#include "plugin_inc.h"
#include "signal_slot.h"
#include "interfaces_utils.h"
#include "pdf_page.h"

using namespace utils;

namespace pdf
{

class PluginRenderSettingsImpl : public IPluginUnknown
                               , public IPluginClone
                               , public IPluginRenderSettings
                               , public IPluginZoom
                               , public IPluginRotation
{
public:
    PluginRenderSettingsImpl(void);
    PluginRenderSettingsImpl(const PluginRenderSettingsImpl &ref);
    ~PluginRenderSettingsImpl(void);

    double zoom() const {return attr.get_zoom_setting();}
    
    double rotation() const {return attr.get_rotate();}
    
    const PDFRenderAttributes& get_render_attr() const {return attr;}

    static PluginRenderSettingsImpl * query_instance(IPluginUnknown * thiz);

public:
    utils::Signal<PluginRenderSettingsImpl *> release_signal;

private:
    static PluginStatus query_interface_impl(
        IPluginUnknown    *thiz,
        const UDSString   *id, 
        void              **ptr);

    static int release_impl(
        IPluginUnknown  *thiz);

    static IPluginUnknown* create_clone_object_impl(
        IPluginUnknown *thiz);

    static PluginStatus set_zoom_factor_impl(
        IPluginUnknown* thiz,
        const float zoom_factor );

    static float get_zoom_factor_impl(
        IPluginUnknown* thiz );
    
    static float get_max_zoom_factor_impl(
        IPluginUnknown* thiz );

    static float get_min_zoom_factor_impl(
        IPluginUnknown* thiz );

    static PluginStatus set_rotation_impl(
        IPluginUnknown               *thiz,
        const PluginRotationDegree   rotation);
    
    static PluginRotationDegree get_rotation_impl(
        IPluginUnknown  *thiz );

private:
    static utils::ObjectTable<PluginRenderSettingsImpl> g_instances_table;

private:
    // the render attributes
    PDFRenderAttributes attr;

};

};  // namespace pdf

#endif

