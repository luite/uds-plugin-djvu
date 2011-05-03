/*
 * File Name: plugin_unknown.h
 */

/*
 * This file is part of uds-plugin-common.
 *
 * uds-plugin-common is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-common is distributed in the hope that it will be useful,
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

#ifndef PLUGIN_UNKNOWN_H_
#define PLUGIN_UNKNOWN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_error.h"
#include "plugin_type.h"
#include "uds_string.h"

typedef struct _IPluginUnknown IPluginUnknown;

/**
 * @brief Universal Document Shell plugin unknown interface.
 * Through IPluginUnknown, caller is able to access the other interfaces
 * by given object. 
 */

struct _IPluginUnknown
{
    // Define a virtual desctuctor to avoid memory leaks in derived classes.
    // Note: IPluginUnknown is intended as a plain C struct,
    //       but uds and/or plugins use it as a C++ base class.
    //       This may cause memory leaks because the derived class destructor
    //       is not called when deleting the base type.
    virtual ~_IPluginUnknown() {}

    /**
     * @brief Query specified interface according to the interface id.
     * @param thiz The IPluginUnknown pointer of a specifed object.
     * @param id The interface id. 
     * @param ptr The variable to store the returned interface pointer.
     * @return TODO. Add return codes here.
     */
    PluginStatus (* query_interface)( IPluginUnknown    *thiz,
                                      const UDSString   *id, 
                                      void              **ptr );

    /**
     * @brief Release the object.
     * @param thiz The IPluginUnknown pointer of a specifed object.
     */
    int (* release)( IPluginUnknown  *thiz );

};

#ifdef __cplusplus
}
#endif 

#endif

