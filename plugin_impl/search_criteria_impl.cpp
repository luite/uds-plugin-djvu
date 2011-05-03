/*
 * File Name: search_criteria_impl.cpp
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

#include "search_criteria_impl.h"

namespace pdf
{

utils::ObjectTable<PluginSearchCriteria> 
    PluginSearchCriteria::g_instances_table;

PluginSearchCriteria::PluginSearchCriteria(void)
: data()
{
    // IPluginUnkown
    query_interface = query_interface_impl;
    release         = release_impl;

    // IPluginSearchCriteria
    set_search_text      = set_search_text_impl;
    set_case_sensitive   = set_case_sensitive_impl;
    set_match_whole_word = set_match_whole_word_impl;
    set_forward          = set_forward_impl;

    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginSearchCriteria>(this);
}

PluginSearchCriteria::~PluginSearchCriteria(void)
{
    g_instances_table.remove(this);
}

PluginStatus 
PluginSearchCriteria::query_interface_impl(IPluginUnknown    *thiz,
                                           const UDSString   *id, 
                                           void              **ptr )
{
    // check object. 
    PluginSearchCriteria *instance = g_instances_table.get_object(thiz);

    // check interface.
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int PluginSearchCriteria::release_impl(IPluginUnknown  *thiz )
{
    PluginSearchCriteria * instance = g_instances_table.get_object(thiz);

    delete instance;

    return PLUGIN_OK;
}

/// IPluginSearchCriteria
PluginStatus 
PluginSearchCriteria::set_search_text_impl(IPluginUnknown    *thiz, 
                                           const UDSString   *text )
{
    PluginSearchCriteria *instance = g_instances_table.get_object(thiz);
    instance->data.text = string(text->get_buffer(text));
    return PLUGIN_OK;
}

PluginStatus 
PluginSearchCriteria::set_case_sensitive_impl(IPluginUnknown   *thiz, 
                                              const PluginBool is_sensitive )
{
    PluginSearchCriteria *instance = g_instances_table.get_object(thiz);
    instance->data.case_sensitive = is_sensitive;
    return PLUGIN_OK;
}

PluginStatus
PluginSearchCriteria::set_match_whole_word_impl(IPluginUnknown   *thiz, 
                                                const PluginBool do_match_word )
{
    PluginSearchCriteria *instance = g_instances_table.get_object(thiz);
    instance->data.match_whole_word = do_match_word;
    return PLUGIN_OK;
}

PluginStatus 
PluginSearchCriteria::set_forward_impl(IPluginUnknown    *thiz, 
                                       const PluginBool  do_search_forward )
{
    PluginSearchCriteria *instance = g_instances_table.get_object(thiz);
    instance->data.forward = do_search_forward;
    return PLUGIN_OK;
}

}   // namespace pdf

