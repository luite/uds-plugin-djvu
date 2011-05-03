/*
 * File Name: search_criteria_impl.h
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

#ifndef PDF_PLUGIN_SEARCH_CRITERIA_H_
#define PDF_PLUGIN_SEARCH_CRITERIA_H_

#include "interfaces_utils.h"
#include "plugin_inc.h"

#include "pdf_search_criteria.h"

namespace pdf
{

struct PDFSearchCriteria;
class PluginSearchCriteria : public IPluginUnknown
                           , public IPluginSearchCriteria
{
public:
    PluginSearchCriteria(void);
    ~PluginSearchCriteria(void);

    PDFSearchCriteria& get_data() {return data;}

private:
    /// IPluginUnknown
    static PluginStatus query_interface_impl(
        IPluginUnknown    *thiz,
        const UDSString   *id, 
        void              **ptr );

    static int release_impl(
        IPluginUnknown  *thiz );

    /// IPluginSearchCriteria
    static PluginStatus set_search_text_impl(
        IPluginUnknown    *thiz, 
        const UDSString   *text ); 

    static PluginStatus set_case_sensitive_impl(
        IPluginUnknown   *thiz, 
        const PluginBool is_sensitive );

    static PluginStatus set_match_whole_word_impl(
        IPluginUnknown   *thiz, 
        const PluginBool do_match_word );

    static PluginStatus set_forward_impl(
        IPluginUnknown    *thiz, 
        const PluginBool  do_search_forward );

private:
    /// all constructured search criteria instances.
    static utils::ObjectTable<PluginSearchCriteria> g_instances_table;

private:
    PDFSearchCriteria data;
};

};  // namespace pdf

#endif


