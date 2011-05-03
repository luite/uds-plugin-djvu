/*
 * File Name: listeners.h
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

#ifndef UTILS_LISTENER_H_
#define UTILS_LISTENER_H_


#include <vector>
#include <map>
#include "plugin_inc.h"

namespace utils
{

/// Implement a event listener container.
/// This class enables user to manage listeners for every
/// PluginEvent type. The handler_id will never be zero,
/// so listener can use zero as a special flag.
class Listeners
{
public:
    Listeners(void);
    ~Listeners(void);

public:
    /// Add listener. Repeated adding same callback function is
    /// allowed. So this function always returns the new added
    /// handler id.
    unsigned long add_listener(const PluginEvent event_type,
                               EventFunc callback,
                               void * user_data);
    
    /// Remove listener.
    bool remove_listener(unsigned long handler_id);

    /// Broadcast.
    void broadcast(IPluginUnknown *sender,
                   const PluginEvent event_type,
                   const PluginEventAttrs *plugin_data);

private:
    // For every listener, it's necessary to maintain
    // callback function pointer, user_data and handler id.
    struct ListenerData
    {
        EventFunc callback;
        void * user_data;
        unsigned long handler_id;
    };
    typedef ListenerData * DataPtr;
    typedef std::vector<DataPtr> Datas;
    typedef std::vector<DataPtr>::iterator DatasIter;
    typedef std::map<PluginEvent, Datas> ListenerMap;
    typedef std::map<PluginEvent, Datas>::iterator ListenerMapIter;
    ListenerMap listeners;
    unsigned long handler_id;

private:
    void broadcast_to_vector(Datas & listeners,
                             IPluginUnknown *sender,
                             const PluginEvent event_type,
                             const PluginEventAttrs *plugin_data);

    bool remove_from_vector(unsigned long handler_id,
                            Datas & listeners);

};

};  // namespace utils

#endif  // UTILS_LISTENER_H_

