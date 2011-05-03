/*
 * File Name: export_impl.cpp
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
#include "export_impl.h"
#include "library_impl.h"
#include "config.h"

/// @brief Return the highest API version this plugin supports.
const char* get_version() 
{
    // VERSION is automatically defined in config.h from the version 
    // number in configure.ac
	return VERSION;
}

/// @brief Create a new plugin library instance and return
/// the IPluginUnknown interface of the instance.
/// Currently, this function is not allowed to be called twice.
IPluginUnknown* create_plugin_library()
{
    using namespace pdf;
    static PluginLibraryImpl * library;
    if (library == NULL)
    {
        library = new PluginLibraryImpl;
        return static_cast<IPluginUnknown *>(library);
    }
    assert(false);
    return 0;
}

