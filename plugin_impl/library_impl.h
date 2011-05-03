/*
 * File Name: library_impl.h
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

#ifndef PDF_PLUGIN_LIBRARY_IMPL_H_
#define PDF_PLUGIN_LIBRARY_IMPL_H_

#include <vector>
#include "plugin_inc.h"
#include "document_impl.h"
#include "interfaces_utils.h"

namespace pdf
{

class PluginDocImpl;

/// @brief Implement Plugin Library object for pdf plugin.
class PluginLibraryImpl : public IPluginUnknown
                        , public IPluginLibrary
{
public:
    PluginLibraryImpl();
    ~PluginLibraryImpl();

private:
    /// @brief Implement query_interface.
    static PluginStatus query_interface_impl(
        IPluginUnknown      *thiz,
        const UDSString     *id, 
        void                **ptr );

    static int release_impl(
        IPluginUnknown      *thiz );

    /// @brief Implement is_supported_document.
    static PluginBool is_supported_document_impl(
        IPluginUnknown      *thiz, 
        const UDSString     *path );

    static IPluginUnknown * create_document_impl( 
        IPluginUnknown      *thiz );

private:
    void on_document_released(PluginDocImpl * doc);

private:
    typedef PluginLibraryImpl * LibraryPtr;
    typedef std::vector<LibraryPtr> Libraries;
    typedef std::vector<LibraryPtr>::iterator LibrariesIter;
    typedef PluginDocImpl * DocPtr;
    typedef std::vector<DocPtr> Documents;
    typedef std::vector<DocPtr>::iterator DocumentsIter;

    /// All constructured library instances.
    static utils::ObjectTable<PluginLibraryImpl> g_instances_table;
    Documents documents;            ///< All opened documents.
};

};  // namespace pdf

#endif

