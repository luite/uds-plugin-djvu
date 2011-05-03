/*
 * File Name: document_impl.h
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

#ifndef PDF_PLUGIN_DOCUMENT_IMPL_H_
#define PDF_PLUGIN_DOCUMENT_IMPL_H_

#include <string>
#include <vector>
#include "plugin_inc.h"
#include "interfaces_utils.h"
#include "signal_slot.h"
#include "listeners.h"
#include "marker_entry_impl.h"
#include "pdf_doc_controller.h"

using namespace utils;

namespace pdf
{

class PluginViewImpl;

/// @brief Implement all necessary interfaces for document object.
/// If the interface is not supported, the impl can remove 
/// them from parent class list.
class PluginDocImpl : public IPluginUnknown 
                    , public IPluginDocument 
                    , public IPluginDocNavigator
                    , public IPluginDocAttributes
                    , public IPluginEventBroadcaster
                    , public IPluginDocHyperlink
                    , public IPluginDocDictionary
                    , public IPluginDocMarker
                    , public IPluginDocSearch
{
public:
    PluginDocImpl();
    ~PluginDocImpl();

public:
    /// Retrieve total page number. This function is used by
    /// PluginViewImpl
    unsigned int get_page_count();

    /// Retrieve page number from anchor. This function is used
    /// by PluginViewImpl.
    unsigned int get_page_number(const std::string & anchor);

    /// Retrieve the anchor for given page number.
    bool get_anchor_of_page(const unsigned int page_number, 
                            std::string & anchor);

    /// Anchor iteraotr: retrieve the previous anchor.
    bool get_prev_page(std::string & anchor);

    /// Anchor iteraotr: retrieve the next anchor.
    bool get_next_page(std::string & anchor);

    bool get_original_size(const std::string &anchor,
                           unsigned int &width,
                           unsigned int &height);

    bool get_content_area(const std::string &anchor,
                          RenderArea &area);

    PluginRotationDegree get_page_original_rotation(const std::string &anchor);

    /// Get the renderer
    PDFRenderer* get_renderer() { return doc_ctrl.get_renderer(); }

    /// Get the document controller
    PDFController & get_doc_ctrl() { return doc_ctrl; }

    /// Get the file name
    const string& get_file_name() { return doc_ctrl.name(); }

public:
    /// The release signal.
    utils::Signal<PluginDocImpl *> release_signal;

private:
    // IPluginUnknown.
    static PluginStatus query_interface_impl(
        IPluginUnknown      *thiz,
        const UDSString     *id, 
        void                **ptr );

    static int release_impl(
        IPluginUnknown      *thiz );

    // IPluginDocument.
    static PluginStatus open_impl(
        IPluginUnknown      *thiz, 
        const UDSString     *path);

    static PluginBool is_open_impl(
        IPluginUnknown      *thiz);

    static PluginStatus close_impl(
        IPluginUnknown      *thiz);

    static IPluginUnknown * create_view_impl(
        IPluginUnknown      *thiz);

    // IPluginDocNavigator
    static PluginStatus get_initial_anchor_impl(
        IPluginUnknown      *thiz, 
        UDSString           *anchor);

    static PluginStatus get_object_from_anchor_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor, 
        PluginRange         *range);

    static PluginDocObjectType get_type_of_object_impl(
        IPluginUnknown      *thiz,
        const PluginRange   *range);
    
    static PluginStatus get_words_from_range_impl(
        IPluginUnknown      *thiz, 
        const PluginRange   *chars_range, 
        PluginRange         *words_range );

    static PluginStatus get_text_from_range_impl(
        IPluginUnknown      *thiz, 
        const PluginRange   *range, 
        UDSString           *result );

    static PluginBool is_anchor_in_current_document_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor );

    static PluginStatus get_file_name_from_anchor_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor,
        UDSString           *file_name);

    static PluginStatus get_file_position_from_anchor_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor,
        signed long long    *position);

    static int compare_anchor_location_impl(
        IPluginUnknown      *thiz,
        const UDSString     *anchor_a,
        const UDSString     *anchor_b );

    static PluginStatus get_attribute_impl(
        IPluginUnknown      *thiz,
        const UDSString     *key, 
        UDSString           *value );

    static PluginStatus set_attribute_impl(
        IPluginUnknown      *thiz,
        const UDSString     *key,
        const UDSString     *value );

    // IPluginEventBroadcaster
    static PluginStatus add_event_receiver_impl(
        IPluginUnknown      *thiz,
        const PluginEvent   plugin_event,
        EventFunc           callback,
        void                *user_data,
        unsigned long       *handler_id);

    static PluginStatus remove_event_receiver_impl(
        IPluginUnknown      *thiz,
        unsigned long       handler_id );

    // IPluginDocHyperlink
    static PluginBool is_hyperlink_impl(
        IPluginUnknown      *thiz,
        const PluginRange   *range );

    static PluginStatus get_target_from_hyperlink_impl(
        IPluginUnknown      *thiz,
        const PluginRange   *range, 
        UDSString           *anchor );

    static IPluginUnknown* get_hyperlinks_in_range_impl(
        IPluginUnknown      *thiz,
        const PluginRange   *range );

    // IPluginDocDictionary
    static PluginBool is_dictionary_impl( 
        IPluginUnknown      *thiz );

    // IPluginDocMarker
    static IPluginUnknown* get_supported_marker_types_impl(
        IPluginUnknown      *thiz );

    static PluginBool has_toc_impl(
        IPluginUnknown      *thiz );

    static PluginStatus request_marker_trees_impl(
        IPluginUnknown      *thiz, 
        const unsigned int  uds_private_size );

    // IPluginDocSearch
    static IPluginUnknown * create_search_criteria_impl(
        IPluginUnknown      *thiz );

    static PluginStatus request_search_next_impl(
        IPluginUnknown      *thiz,
        IPluginUnknown      *criteria,
        const UDSString     *from_anchor,
        const unsigned int  search_id);

    static PluginStatus request_search_all_impl( 
        IPluginUnknown *thiz,
        IPluginUnknown *criteria,
        const unsigned int  search_id);

    static PluginStatus abort_search_impl(
        IPluginUnknown *thiz,
        const unsigned int  search_id);

private:
    MarkerEntry* generate_marker_entry(const std::string & anchor,
                                       const std::string & text,
                                       const std::string & goto_anchor,
                                       const unsigned int uds_private_size);

    void add_toc_children(MarkerEntry * parent, 
                          TocItem * item,
                          const unsigned int uds_private_size);

private:
    typedef PluginViewImpl * ViewPtr;
    typedef std::vector<ViewPtr> Views;
    typedef std::vector<ViewPtr>::iterator ViewsIter;
    
private:
    // Handle the view release event
    void on_view_released(ViewPtr view);

    // Handle the search ready event
    void on_search_results_ready(SearchResult res, PDFRangeCollection* coll
        , unsigned int search_id);

private:
    static utils::ObjectTable<PluginDocImpl> g_instances_table;

private:
    utils::Listeners listeners;
    PDFController    doc_ctrl;
    Views            views;

    friend class PluginViewImpl;

private:
    void init_doc_attributes();
    typedef std::map<std::string, std::string> DocAttrMap;
    typedef DocAttrMap::iterator               DocAttrMapIter;
    DocAttrMap doc_attr_map;
};

};  // namespace pdf

#endif

