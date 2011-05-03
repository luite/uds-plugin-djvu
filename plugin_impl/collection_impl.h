/*
 * File Name: collection_impl.h
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

#ifndef DUMMY_PLUGIN_COLLECTION_IMPL_H_
#define DUMMY_PLUGIN_COLLECTION_IMPL_H_

#include <vector>
#include <cassert>
#include "plugin_inc.h"
#include "interfaces_utils.h"
#include "signal_slot.h"
#include "pdf_observer.h"
#include "pdf_collection.h"

using namespace utils;

namespace pdf
{

/// PluginCollectionImpl provides a template based collection container.
/// Through this class, caller is able to store data inside the collection.
/// If caller wants to strore pointe data in the collection, it's necessary
/// for caller to connect the release_signal, so that when collection object
/// is to be released, the caller can be notified.
class PluginCollectionImpl : public IPluginUnknown
                           , public IPluginCollection
{
public:
    PluginCollectionImpl(void)
    {
        query_interface = query_interface_impl;
        release = release_impl;
        get_data = get_data_impl;
        get_num_elements = get_num_elements_impl;

        g_instances_table.add_interface<IPluginUnknown>(this);
        g_instances_table.add_interface<IPluginCollection>(this);
    }

    ~PluginCollectionImpl(void)
    {
        // the collection data should be released here
        g_instances_table.remove(this);
        delete data;
        data = 0;
    }

    void set_data(PDFCollectionBase *coll)
    {
        data = coll;
    }

public:
    utils::Signal<PluginCollectionImpl *> release_signal;

private:
    // IPluginUnknown
    static PluginStatus query_interface_impl(IPluginUnknown      *thiz,
                                             const UDSString     *id, 
                                             void                **ptr )
    {
        // check object. 
        PluginCollectionImpl *instance = g_instances_table.get_object(thiz);
        if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
        {
            return PLUGIN_OK;
        }
        return PLUGIN_FAIL;
    }

    static int release_impl(IPluginUnknown      *thiz )
    {
        PluginCollectionImpl *instance = g_instances_table.get_object(thiz);
        if (instance->release_signal.count() > 0)
        {
            instance->release_signal.broadcast(instance);
        }
        else
        {
            delete instance;
        }
        return 0;
    }

    // IPluginCollection
    static PluginStatus get_data_impl(IPluginUnknown   *thiz,
                                      void             **data_ptr)
    {
        PluginCollectionImpl *instance = g_instances_table.get_object(thiz);
        instance->data->get_first_element(data_ptr);
        return PLUGIN_OK;
    }

    static int get_num_elements_impl(IPluginUnknown   *thiz )
    {
        PluginCollectionImpl *instance = g_instances_table.get_object(thiz);
        return instance->data->get_count();
    }

private:
    static ObjectTable<PluginCollectionImpl> g_instances_table;
    PDFCollectionBase *data;

};



};  // namespace dummy

#endif  // PLUGIN_COLLECTION_IMPL_H_

