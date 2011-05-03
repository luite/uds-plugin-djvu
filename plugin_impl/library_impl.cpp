/*
 * File Name: library_impl.cpp
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
#include <string.h>
#include "library_impl.h"

namespace pdf
{

/// @brief Internal file extension table.
/// dummy file is a index file which contains a list of file names.
static const std::string g_ext_table[] = 
{
    ".djvu", ".djv"
};
static const int EXT_TABLE_SIZE = 
                 sizeof(g_ext_table) / sizeof(g_ext_table[0]);

// Plugin library object implementation.
utils::ObjectTable<PluginLibraryImpl> 
PluginLibraryImpl::g_instances_table;

// Constructor
PluginLibraryImpl::PluginLibraryImpl()
{
    // g_thread_init(0);

    // Initialize all interface method pointers.
    query_interface         = query_interface_impl;
    release                 = release_impl;
    is_supported_document   = is_supported_document_impl;
    create_document         = create_document_impl;

    // record the instance.
    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginLibrary>(this);
}

PluginLibraryImpl::~PluginLibraryImpl()
{
    g_instances_table.remove(this);
}

PluginStatus 
PluginLibraryImpl::query_interface_impl(IPluginUnknown    *thiz,
                                        const UDSString   *id,
                                        void              **ptr)
{
    // Two steps to query the interface.
    // 1. Retrieve object instance from the thiz pointer. 
    // 2. Retrieve desired interface from given object.
    PluginLibraryImpl *instance = g_instances_table.get_object(thiz);
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int PluginLibraryImpl::release_impl( IPluginUnknown  *thiz )
{
    PluginLibraryImpl *instance = g_instances_table.get_object(thiz);
    delete instance;
    return 0;
}

PluginBool 
PluginLibraryImpl::is_supported_document_impl(IPluginUnknown    *thiz, 
                                              const UDSString   *path)
{
    // TODO. Refine this method
    // Estimate the first several bytes?
    if (path != 0)
    {
        const char * pos = strrchr(path->get_buffer(path), '.');
        for(int i = 0; i < EXT_TABLE_SIZE; ++i)
        {
            if (pos)
            {
                if (strncmp(pos, 
                            g_ext_table[i].c_str(), 
                            g_ext_table[i].length()) == 0)
                {
                    return PLUGIN_TRUE;
                }
            }
        }
    }
    return PLUGIN_FALSE;
}

IPluginUnknown * 
PluginLibraryImpl::create_document_impl(IPluginUnknown * thiz)
{
    PluginLibraryImpl *obj = g_instances_table.get_object(thiz);

    if (obj)
    {
        // Create a new document object.
        DocPtr doc = new PluginDocImpl;

        // Listen the release signal.
        doc->release_signal.add_slot(obj, 
            &PluginLibraryImpl::on_document_released);

        // Store the document.
        obj->documents.push_back(doc);
        return static_cast<IPluginUnknown *>(doc);
    }
    return 0;
}

void PluginLibraryImpl::on_document_released(PluginDocImpl * doc)
{
    DocumentsIter iter = std::find(documents.begin(), documents.end(), doc);
    if (iter != documents.end())
    {
        delete *iter;
        documents.erase(iter);
    }
}

}   // namespace pdf

