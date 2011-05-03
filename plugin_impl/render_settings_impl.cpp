/*
 * File Name: render_settings_impl.cpp
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

#include "render_settings_impl.h"
#include "pdf_renderer.h"

namespace pdf
{

utils::ObjectTable<PluginRenderSettingsImpl> 
PluginRenderSettingsImpl::g_instances_table;

PluginRenderSettingsImpl::PluginRenderSettingsImpl(void)
: attr()
{
    // IPluginUnknown
    query_interface = query_interface_impl;
    release         = release_impl;

    // IPluginClone
    create_clone_object = create_clone_object_impl;
    
    // IPluginRenderSettings, no method yet.

    // IPluginZoom
    set_zoom_factor     = set_zoom_factor_impl;
    get_zoom_factor     = get_zoom_factor_impl;
    get_max_zoom_factor = get_max_zoom_factor_impl;
    get_min_zoom_factor = get_min_zoom_factor_impl;

    // IPluginRotation
    set_rotation = set_rotation_impl;
    get_rotation = get_rotation_impl;

    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginClone>(this);
    g_instances_table.add_interface<IPluginRenderSettings>(this);
    g_instances_table.add_interface<IPluginZoom>(this);
    g_instances_table.add_interface<IPluginRotation>(this);
}

PluginRenderSettingsImpl::PluginRenderSettingsImpl(const PluginRenderSettingsImpl &ref)
: attr(ref.attr)
{
    // IPluginUnknown
    query_interface = query_interface_impl;
    release         = release_impl;

    // IPluginClone
    create_clone_object = create_clone_object_impl;
    
    // IPluginRenderSettings, no method yet.

    // IPluginZoom
    set_zoom_factor     = set_zoom_factor_impl;
    get_zoom_factor     = get_zoom_factor_impl;
    get_max_zoom_factor = get_max_zoom_factor_impl;
    get_min_zoom_factor = get_min_zoom_factor_impl;

    // IPluginRotation
    set_rotation = set_rotation_impl;
    get_rotation = get_rotation_impl;

    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginClone>(this);
    g_instances_table.add_interface<IPluginRenderSettings>(this);
    g_instances_table.add_interface<IPluginZoom>(this);
    g_instances_table.add_interface<IPluginRotation>(this);
}

PluginRenderSettingsImpl::~PluginRenderSettingsImpl(void)
{
    g_instances_table.remove(this);
}

PluginRenderSettingsImpl * 
PluginRenderSettingsImpl::query_instance(IPluginUnknown * thiz)
{
    return g_instances_table.get_object(thiz);
}

PluginStatus 
PluginRenderSettingsImpl::query_interface_impl(IPluginUnknown    *thiz,
                                               const UDSString   *id, 
                                               void              **ptr)
{
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int 
PluginRenderSettingsImpl::release_impl(IPluginUnknown  *thiz)
{
    // Have to handle the object cloned and object created by view.
    // For cloned object, its life cycle is not limited.
    // For object created by view, view keeps a reference to it, so
    // it should notify view when it's to be released.
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    if (instance->release_signal.count())
    {
        instance->release_signal.safe_broadcast(instance);
    }
    else
    {
        delete instance;
    }
    return 0;
}

IPluginUnknown* 
PluginRenderSettingsImpl::create_clone_object_impl(IPluginUnknown *thiz)
{
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    PluginRenderSettingsImpl * another_instance 
        = new PluginRenderSettingsImpl(*instance);

    // DON'T need to connect the release signal.
    return static_cast<IPluginUnknown *>(another_instance);
}

PluginStatus 
PluginRenderSettingsImpl::set_zoom_factor_impl(IPluginUnknown *thiz,
                                               const float zoom_factor)
{
    PluginRenderSettingsImpl *instance = g_instances_table.get_object(thiz);

    // Should check the zoom factor boundary. TODO
    instance->attr.set_zoom_setting(static_cast<double>(zoom_factor));
    return PLUGIN_OK;
}

float 
PluginRenderSettingsImpl::get_zoom_factor_impl(IPluginUnknown *thiz)
{
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    return static_cast<float>(instance->attr.get_zoom_setting());
}
    
float 
PluginRenderSettingsImpl::get_max_zoom_factor_impl(IPluginUnknown *thiz)
{
    return static_cast<float>(PDFRenderer::get_max_zoom());
}

float 
PluginRenderSettingsImpl::get_min_zoom_factor_impl(IPluginUnknown *thiz)
{
    return static_cast<float>(PDFRenderer::get_min_zoom());
}

PluginStatus 
PluginRenderSettingsImpl::set_rotation_impl(IPluginUnknown  *thiz,
                                            const PluginRotationDegree rotation)
{
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    instance->attr.set_rotate(static_cast<int>(rotation));
    return PLUGIN_OK;
}
    
PluginRotationDegree 
PluginRenderSettingsImpl::get_rotation_impl(IPluginUnknown  *thiz)
{    
    PluginRenderSettingsImpl * instance = g_instances_table.get_object(thiz);
    return static_cast<PluginRotationDegree>(instance->attr.get_rotate());
}

} // namespace pdf

