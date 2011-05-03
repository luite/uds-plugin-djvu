/*
 * File Name: listeners.cpp
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

#include "listeners.h"

namespace utils
{

Listeners::Listeners(void)
: handler_id(0)
{
}

Listeners::~Listeners(void)
{
}

/// Add listener. Repeated adding same callback function is
/// allowed. So this function always returns the new added
/// handler id.
unsigned long Listeners::add_listener(const PluginEvent event_type,
                                      EventFunc callback,
                                      void * user_data)
{
    ++handler_id;
    ListenerData * data = new ListenerData;
    data->callback = callback;
    data->user_data = user_data;
    data->handler_id = handler_id;
    listeners[event_type].push_back(data);
    return handler_id;
}


bool Listeners::remove_listener(unsigned long handler_id)
{
    // Have to search the whole map.
    ListenerMapIter begin = listeners.begin();
    ListenerMapIter end   = listeners.end();
    for(ListenerMapIter it = begin; it != end; ++it)
    {
        if (remove_from_vector(handler_id, it->second))
        {
            return true;
        }
    }
    return false;
}


void Listeners::broadcast(IPluginUnknown *sender,
                          const PluginEvent event_type,
                          const PluginEventAttrs *plugin_data)
{
    broadcast_to_vector(listeners[event_type], sender, 
                        event_type, plugin_data);
}

 void Listeners::broadcast_to_vector(Datas & listeners,
                                     IPluginUnknown *sender,
                                     const PluginEvent event_type,
                                     const PluginEventAttrs *plugin_data)
 {
     DatasIter begin = listeners.begin();
     DatasIter end   = listeners.end();
     for(DatasIter it = begin; it != end; ++it)
     {
         (*(*it)->callback)( sender, 
                             (*it)->handler_id, 
                             event_type, 
                             (*it)->user_data,
                             plugin_data );
     }
 }

bool Listeners::remove_from_vector(unsigned long handler_id,
                                   Datas & listeners)
{
    DatasIter begin = listeners.begin();
    DatasIter end   = listeners.end();
    for(DatasIter it = begin; it != end; ++it)
    {
        if ((*it)->handler_id == handler_id)
        {
            delete *it;
            listeners.erase(it);
            return true;
        }
    }
    return false;
}

}

