/*
 * File Name: document_impl.cpp
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
#include "document_impl.h"
#include "view_impl.h"
#include "collection_impl.h"
#include "string_impl.h"
#include "search_criteria_impl.h"
#include "pdf_anchor.h"

namespace pdf
{

utils::ObjectTable<PluginDocImpl> PluginDocImpl::g_instances_table;

PluginDocImpl::PluginDocImpl()
{
    // IPluginUnknown
    query_interface = query_interface_impl;
    release         = release_impl;

    // IPluginDocument
    open        = open_impl;
    is_open     = is_open_impl;
    close       = close_impl;
    create_view = create_view_impl;

    // IPluginDocNavigator
    get_initial_anchor      = get_initial_anchor_impl;
    get_object_from_anchor  = get_object_from_anchor_impl;
    get_type_of_object      = get_type_of_object_impl;
    get_words_from_range    = get_words_from_range_impl;
    get_text_from_range     = get_text_from_range_impl;
    is_anchor_in_current_document = is_anchor_in_current_document_impl;
    get_file_name_from_anchor = get_file_name_from_anchor_impl;
    get_file_position_from_anchor = get_file_position_from_anchor_impl;
    compare_anchor_location = compare_anchor_location_impl;

    // IPluginDocAttributes
    get_attribute = get_attribute_impl;
    set_attribute = set_attribute_impl;

    // IPluginEventBroadcaster
    add_event_receiver      = add_event_receiver_impl;
    remove_event_receiver   = remove_event_receiver_impl;

    // IPluginDocHyperlink
    is_hyperlink                = is_hyperlink_impl;
    get_target_from_hyperlink   = get_target_from_hyperlink_impl;
    get_hyperlinks_in_range     = get_hyperlinks_in_range_impl;

    // IPluginDocDictionary
    is_dictionary = is_dictionary_impl;

    // IPluginDocMarker
    get_supported_marker_types  = get_supported_marker_types_impl;
    request_marker_trees        = request_marker_trees_impl;
    has_toc                     = has_toc_impl;

    // IPluginDocSearch
    create_search_criteria  = create_search_criteria_impl;
    request_search_next     = request_search_next_impl;
    request_search_all      = request_search_all_impl;
    abort_search            = abort_search_impl;

    // Initialize interface and object table.
    g_instances_table.add_interface<IPluginUnknown>(this);
    g_instances_table.add_interface<IPluginDocument>(this);
    g_instances_table.add_interface<IPluginDocNavigator>(this);
    g_instances_table.add_interface<IPluginDocAttributes>(this);
    g_instances_table.add_interface<IPluginEventBroadcaster>(this);
    g_instances_table.add_interface<IPluginDocHyperlink>(this);
    g_instances_table.add_interface<IPluginDocDictionary>(this);
    g_instances_table.add_interface<IPluginDocMarker>(this);
    g_instances_table.add_interface<IPluginDocSearch>(this);

    // connect to the search ready signal
    doc_ctrl.sig_search_results_ready.add_slot(this
        , &PluginDocImpl::on_search_results_ready);

    // Initialize document attributes map.
    init_doc_attributes();
}

void PluginDocImpl::init_doc_attributes()
{
    doc_attr_map.insert(std::make_pair("fixed-page", "yes"));
	// TODO: set attributes "author", "title", "description"
}

PluginDocImpl::~PluginDocImpl()
{
    g_instances_table.remove(this);
}

unsigned int PluginDocImpl::get_page_count()
{
	WARNPRINTF("get_page_count(): %d", doc_ctrl.page_count());
	return doc_ctrl.page_count();
}

unsigned int PluginDocImpl::get_page_number(const std::string & anchor)
{
	WARNPRINTF("get_page_number(%s): %d", anchor.c_str(), doc_ctrl.get_page_number_of_anchor(anchor));
    return doc_ctrl.get_page_number_of_anchor(anchor);
}

bool PluginDocImpl::get_anchor_of_page(const unsigned int page_number,
                                       std::string & anchor)
{
	doc_ctrl.get_anchor_of_page(page_number, anchor); // DEBUG
	WARNPRINTF("get_anchor_of_page(%d) %s", page_number, anchor.c_str());
	return doc_ctrl.get_anchor_of_page(page_number, anchor);
}

bool PluginDocImpl::get_prev_page(std::string & anchor)
{
	WARNPRINTF("get_prev_page_page(%s)", anchor.c_str());
	bool res = doc_ctrl.get_prev_page_anchor(anchor);
	WARNPRINTF("result: %s   (%s)", anchor.c_str(), (res?"true":"false"));
	return res;
	// return doc_ctrl.get_prev_page_anchor(anchor);
}

bool PluginDocImpl::get_next_page(std::string & anchor)
{
	WARNPRINTF("get_next_page_page(%s)", anchor.c_str());
	bool res = doc_ctrl.get_prev_page_anchor(anchor);
	WARNPRINTF("result: %s   (%s)", anchor.c_str(), (res?"true":"false"));
	return res;
	// return doc_ctrl.get_next_page_anchor(anchor);
}

bool PluginDocImpl::get_original_size(const std::string &anchor,
                                      unsigned int &width,
                                      unsigned int &height)
{
    double w, h;
    if (doc_ctrl.get_page_crop_width(anchor, w) &&
        doc_ctrl.get_page_crop_height(anchor, h))
    {
        width = static_cast<unsigned int>(w);
        height = static_cast<unsigned int>(h);
        return true;
    }

    return false;
}

bool PluginDocImpl::get_content_area(const std::string &anchor,
                                     RenderArea &area)
{
    return doc_ctrl.get_page_content_area(anchor, area);
}

PluginRotationDegree PluginDocImpl::get_page_original_rotation(const std::string &anchor)
{
    return doc_ctrl.get_page_original_rotation(anchor);
}

PluginStatus 
PluginDocImpl::query_interface_impl(IPluginUnknown    *thiz,
                                    const UDSString   *id, 
                                    void              **ptr )
{
    // check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);

    // check interface.
    if (g_instances_table.query_interface(instance, id->get_buffer(id), ptr))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

int 
PluginDocImpl::release_impl(IPluginUnknown  *thiz )
{
    // Check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    instance->release_signal.safe_broadcast(instance);
    return 0;
}


PluginStatus
PluginDocImpl::open_impl(IPluginUnknown    *thiz, 
                         const UDSString   *path)
{
    // Check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    
    // Check document has been opened or not.
    if (instance->doc_ctrl.is_open())
    {
        return PLUGIN_ERROR_OPEN_FILE;
    }

    return instance->doc_ctrl.open(path->get_buffer(path));
}

PluginBool
PluginDocImpl::is_open_impl(IPluginUnknown    *thiz)
{
    // Check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    
    // Check document has been opened or not.
    if (instance->doc_ctrl.is_open())
    {
        return PLUGIN_TRUE;
    }
    return PLUGIN_FALSE;
}

PluginStatus
PluginDocImpl::close_impl(IPluginUnknown   *thiz)
{
    // Check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    
    // Check document has been opened or not.
    if (!instance->doc_ctrl.is_open())
    {
        return PLUGIN_FAIL;
    }

    if (instance->doc_ctrl.close())
    {
        return PLUGIN_OK;
    }

    return PLUGIN_FAIL;
}

IPluginUnknown *
PluginDocImpl::create_view_impl(IPluginUnknown   *thiz)
{
    // Check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    
    // Create the view.
    ViewPtr ptr = new PluginViewImpl(instance);
    if (ptr == 0)
    {
        return 0;
    }

    instance->views.push_back(ptr);
    ptr->release_signal.add_slot(instance, &PluginDocImpl::on_view_released);
    return static_cast<IPluginUnknown *>(ptr);
}

PluginStatus 
PluginDocImpl::get_initial_anchor_impl(IPluginUnknown   *thiz,
                                       UDSString        *anchor)
{
	WARNPRINTF("get_initial_anchor_impl(%s)", anchor->get_buffer(anchor));
	PluginDocImpl *instance = g_instances_table.get_object(thiz);

    // Use first page as the initial anchor.
    std::string temp;
    if (instance->doc_ctrl.get_anchor_of_page(1, temp))
    {
		anchor->assign(anchor, temp.c_str());
        return PLUGIN_OK;
    }

    return PLUGIN_FAIL;
}

PluginStatus 
PluginDocImpl::get_object_from_anchor_impl(IPluginUnknown  *thiz,
                                           const UDSString *anchor, 
                                           PluginRange     *range)
{
	WARNPRINTF("get_object_from_anchor_impl(%s)", anchor->get_buffer(anchor));
	PluginStatus ret = PLUGIN_FAIL;
    if (thiz && anchor && range)
    {
        PDFAnchor param(anchor->get_buffer(anchor));
        PluginDocImpl *instance = g_instances_table.get_object(thiz);
        PDFAnchor start, end;
		WARNPRINTF("page_num: %d", param.page_num);
        if (param.page_num > 0)
        {
            PagePtr page = instance->doc_ctrl.get_page(param.page_num);
            if (param.link_idx != -1)
            {
                // check 'anchor' is a link?
                // get the range anchor of this link.
                if (page && page->get_range_param_by_link_index(param.link_idx, start, end))
                {
                    ret = PLUGIN_OK;
                }
            }

            if (param.word_num != -1)
            {
                // check the word
                if (page && page->get_range_param_by_word_index(param.word_num, start, end))
                {
                    ret = PLUGIN_OK;
                }
            }

			// DEBUG
			ret = PLUGIN_OK;
			start.page_num = param.page_num;
			end.page_num = param.page_num;
			start.link_idx = param.link_idx;
			end.link_idx = param.link_idx;
			start.word_num = param.word_num;
			end.word_num = param.word_num;
			//ret = PLUGIN_OK;
			//start = end = param;
			// END DEBUG
		}

        if (param.toc_idx != -1)
        {
            // check 'anchor' is a TOC item?
            start.toc_idx = param.toc_idx;
            end.toc_idx = param.toc_idx;
            ret = PLUGIN_OK;
        }

        if (ret == PLUGIN_OK)
        {
            range->start_anchor->assign(range->start_anchor, 
                                        start.get_string().c_str());
            range->end_anchor->assign(range->end_anchor, 
                                      end.get_string().c_str());
        }
    }

    return ret;
}

PluginStatus
PluginDocImpl::get_file_name_from_anchor_impl(IPluginUnknown    *thiz,
                                            const UDSString   *anchor,
                                            UDSString         *file_name)
{
    PDFAnchor anchor_param(anchor->get_buffer(anchor));

    gchar * name = 0;
    if (!anchor_param.file_name.empty())
    {
        // if anchor contains the file name, use it
        name = g_path_get_basename(anchor_param.file_name.c_str());
    }
    else
    {
        // otherwise, use the file name stored in controller
        PluginDocImpl *instance = g_instances_table.get_object(thiz);
        name = g_path_get_basename(instance->doc_ctrl.name().c_str());
    }

    if (name != 0)
    {
		WARNPRINTF("get_file_name_from_anchor_impl(%s): %s", anchor->get_buffer(anchor), name);
		file_name->assign(file_name, name);
        return PLUGIN_OK;
    }
	WARNPRINTF("get_file_name_from_anchor_impl(%s): FAIL", anchor->get_buffer(anchor));
    return PLUGIN_FAIL;
}

PluginStatus 
PluginDocImpl::get_file_position_from_anchor_impl(IPluginUnknown    *thiz,
                                                const UDSString   *anchor,
                                                signed long long  *position)
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    PDFAnchor anchor_param(anchor->get_buffer(anchor));
    if (anchor_param.page_num <= 0 ||
        anchor_param.page_num > static_cast<int>(instance->get_page_count()))
    {
		WARNPRINTF("get_file_position_from_anchor_impl(%s): FAIL", anchor->get_buffer(anchor));
        return PLUGIN_FAIL;
    }
	WARNPRINTF("get_file_position_from_anchor_impl(%s): %d", anchor->get_buffer(anchor), (int)anchor_param.page_num);
    *position = anchor_param.page_num;
    return PLUGIN_OK;
}

PluginBool 
PluginDocImpl::is_anchor_in_current_document_impl(IPluginUnknown    *thiz,
                                                  const UDSString   *anchor)
{
    // check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    
    if (instance->doc_ctrl.is_anchor_in_current_document(
        anchor->get_buffer(anchor)))
    {
		WARNPRINTF("is_anchor_in_current_document_impl(%s): YES", anchor->get_buffer(anchor));
		return PLUGIN_TRUE;
    }
	WARNPRINTF("is_anchor_in_current_document_impl(%s): NO", anchor->get_buffer(anchor));
    return PLUGIN_FALSE;
}

int 
PluginDocImpl::compare_anchor_location_impl(IPluginUnknown   *thiz,
                                            const UDSString  *anchor_a,
                                            const UDSString  *anchor_b)
{
	// check object. 
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
   	WARNPRINTF("compare_anchor_location_impl(%s, %s): %d", anchor_a->get_buffer(anchor_a), anchor_b->get_buffer(anchor_b), instance->doc_ctrl.compare_anchor(anchor_a->get_buffer(anchor_a), anchor_a->get_buffer(anchor_b)));
	return instance->doc_ctrl.compare_anchor(anchor_a->get_buffer(anchor_a)
        , anchor_a->get_buffer(anchor_b));
}
    
PluginStatus 
PluginDocImpl::add_event_receiver_impl(IPluginUnknown     *thiz,
                                       const PluginEvent  plugin_event,
                                       EventFunc          callback,
                                       void               *user_data,
                                       unsigned long      *handler_id)
{
	WARNPRINTF("");
	PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (handler_id == 0)
    {
        return PLUGIN_FAIL;
    }

    *handler_id = instance->listeners.add_listener(plugin_event, callback, user_data);

    return PLUGIN_OK;
}

PluginStatus 
PluginDocImpl::remove_event_receiver_impl(IPluginUnknown  *thiz,
                                          unsigned long   handler_id )
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->listeners.remove_listener(handler_id))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginBool 
PluginDocImpl::is_hyperlink_impl(IPluginUnknown    *thiz,
                                 const PluginRange *range )
{
    if (range)
    {
        // Parse the range.
        UDSString * start = range->start_anchor;
        PDFAnchor start_param(start->get_buffer(start));

        UDSString * end = range->end_anchor;
        PDFAnchor end_param(end->get_buffer(end));
		
		WARNPRINTF("is_hyperlink_impl(%s, %s)", start->get_buffer(start), end->get_buffer(end));
		WARNPRINTF("start_pagenum: %d,  end_pagenum: %d", start_param.page_num, end_param.page_num);
		WARNPRINTF("start_linkidx: %d,  end_linkidx: %d", start_param.link_idx, end_param.link_idx);
        // Check the startpos and endpos is in the same link 
        // in the same page. If yes, it's a sensible hyperlink.
        if ( (start_param.page_num != 0) && (end_param.page_num != 0)
             && (start_param.page_num == end_param.page_num)
             && (start_param.link_idx != -1) && (end_param.link_idx != -1)
             && (start_param.link_idx == end_param.link_idx))
        {
			WARNPRINTF("YES (normal link)");
			return PLUGIN_TRUE;
        }
        //else if ((start_param.toc_idx = -1) && (end_param.toc_idx = -1)
		else if ((start_param.toc_idx != -1) && (end_param.toc_idx != -1)
                && (start_param.toc_idx == end_param.toc_idx))
        {
			WARNPRINTF("YES (toc)");
			return PLUGIN_TRUE;
        }
    }
	WARNPRINTF("NO (not a hyperlink)");
    return PLUGIN_FALSE;
}

PluginStatus 
PluginDocImpl::get_target_from_hyperlink_impl(IPluginUnknown     *thiz,
                                              const PluginRange  *range, 
                                              UDSString          *anchor )
{
    WARNPRINTF("get_target_from_hyperlink_impl()");
	if (is_hyperlink_impl(thiz, range))
    {
        // Parse the range.
        UDSString * start = range->start_anchor;
        PDFAnchor param(start->get_buffer(start));

        // Get the anchor of link's destination.
        PluginDocImpl *instance = g_instances_table.get_object(thiz);

        std::string temp;
        if (param.link_idx != -1)
        {
            PagePtr page = instance->doc_ctrl.get_page(param.page_num);

            if (page && page->get_goto_anchor_of_link(param.link_idx, temp))
            {
                anchor->assign(anchor, temp.c_str());
                return PLUGIN_OK;
            }
        }
        else if (param.toc_idx != -1)
        {
            PDFToc * toc = instance->doc_ctrl.get_toc();
            if (toc && toc->get_goto_anchor_of_toc_idx(param.toc_idx, temp))
            {
                anchor->assign(anchor, temp.c_str());
                return PLUGIN_OK;
            }
        }
		else if (param.page_num > 0) // added by luite
		{
			PagePtr page = instance->doc_ctrl.get_page(param.page_num);
			if(page) {
				anchor->assign(anchor, start->get_buffer(start));
				return PLUGIN_OK;
			}
		}
    }

    return PLUGIN_FAIL;
}

// The dummy document supports table of content.
IPluginUnknown* 
PluginDocImpl::get_supported_marker_types_impl(IPluginUnknown  *thiz )
{
	WARNPRINTF("get_supported_marker_types_impl()");
	// Not necessary to check thiz in this kind of document.
    // But if the document has different marker type it should
    // make necessary check.

    typedef PDFCollection<PluginMarkerType*> PDFMarkers;
    PDFMarkers *markers = new PDFMarkers;
    PluginMarkerType type = MARKER_TOC;
    markers->add(&type);
	
    PluginCollectionImpl *collection = new PluginCollectionImpl;
    
    collection->set_data(markers);

    // It's not necessary to connect the release signal, because
    // the collection can remove the memory itself. Document object
    // does not store any pointer data in the collection here.
    return static_cast<IPluginUnknown *>(collection);
}

void PluginDocImpl::add_toc_children(MarkerEntry * parent, 
                                     TocItem * item, 
                                     const unsigned int uds_private_size)
{
    while (item)
    {
        // Create a MarkerEntryImpl using item.
        MarkerEntry * entry = generate_marker_entry(item->anchor, 
                                                    item->text, 
                                                    item->goto_anchor,
                                                    uds_private_size);

        // Insert this item as the last child of the parent.
        if (!parent->first_child)
        {
            parent->first_child = entry;
        }
        else
        {
            MarkerEntry* cur = parent->first_child;
            while (cur->sibling)
            {
                cur = cur->sibling;
            }
            cur->sibling = entry;
        }
    
        // Recursively descend over children
        if (item->first_child)
        {
            add_toc_children(entry, item->first_child, uds_private_size);
        }

        // Next sibling
        item = item->sibling;
    }
}

// Steps to generate marker tree.
// 1. Generate the collection object.
// 2. Add marker tree nodes into the collection.
// 3. Invoke the callback function registered in listeners list.
// TODO. Remove the marker tree copy, use the MarkerEntryImpl in PDFToc instead.
PluginStatus 
PluginDocImpl::request_marker_trees_impl(IPluginUnknown     *thiz, 
                                         const unsigned int uds_private_size )
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);

    // Retrieve TOC.
    PDFToc * toc = instance->doc_ctrl.get_toc();
    TocItem * item = toc->get_toc();

    if (item == 0)
    {
        return PLUGIN_FAIL;
    }

    MarkerEntry* root_toc = marker_entry_new(uds_private_size);
    root_toc->type = MARKER_TOC;

    // Allocate collection object.
    typedef PDFCollection<MarkerEntry *> PDFMarkerEntries;
    PDFMarkerEntries * entries = new PDFMarkerEntries;

    // Traverse the whole TOC tree.
    instance->add_toc_children(root_toc, item, uds_private_size);
    entries->add(root_toc->first_child);

    // Root node is useless, disconnect it with other nodes
    root_toc->sibling = 0;
    root_toc->first_child = 0;
    marker_entry_free_recursive(root_toc);

    // Return the results as collection.
    PluginCollectionImpl *collection = new PluginCollectionImpl;
    collection->set_data(entries);

    // Notify all listeners.
    PluginEventAttrs event_data;
    event_data.marker_ready.result = static_cast<IPluginUnknown *>(collection);
    instance->listeners.broadcast(thiz, EVENT_MARKER_READY, &event_data);

    return PLUGIN_OK;
}

PluginBool PluginDocImpl::has_toc_impl(IPluginUnknown *thiz)
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->doc_ctrl.get_toc()->has_toc())
    {
        return PLUGIN_TRUE;
    }
    return PLUGIN_FALSE;
}

MarkerEntry * PluginDocImpl::generate_marker_entry(const std::string & anchor,
                                                   const std::string & text,
                                                   const std::string & goto_anchor,
                                                   const unsigned int uds_private_size)
{
	WARNPRINTF("generate_marker_entry(%s, %s, %s, %d)", anchor.c_str(), text.c_str(), goto_anchor.c_str(), uds_private_size);
	MarkerEntry * entry = marker_entry_new(uds_private_size);
    if (entry == 0)
    {
        return 0;
    }

    entry->type         = MARKER_TOC;
    entry->anchor       = new StringImpl(anchor);
    entry->text         = new StringImpl(text);
    entry->sibling      = 0;
    entry->first_child  = 0;
    entry->uds_private_size = uds_private_size;

    return entry;
}

IPluginUnknown* PluginDocImpl::get_hyperlinks_in_range_impl(IPluginUnknown      *thiz,
                                                            const PluginRange   *range)
{
	WARNPRINTF("get_hyperlinks_in_range_impl()");
	PluginDocImpl *instance = g_instances_table.get_object(thiz);

    // Parse the range.
    UDSString * start = range->start_anchor;
    if (start == 0)
    {
        // start anchor cannot be NULL
        return 0;
    }
    UDSString * end = range->end_anchor;
    std::string start_anchor = start->get_buffer(start);
    std::string end_anchor;
    if (end != 0)
    {
        end_anchor = end->get_buffer(end);
    }
    else
    {
        PDFAnchor param;
        param.set_end_anchor();
        end_anchor = param.get_string();
    }

    PDFCollectionBase * results = instance->doc_ctrl.get_hyperlinks_from_range(
        start_anchor, end_anchor);

    if (results == 0)
    {
        // no hyperlinks
        return 0;
    }

    if (results->get_count() <= 0)
    {
        // no hyperlinks
        delete results;
        results = 0;
        return 0;
    }

    // Return the results as collection.
    PluginCollectionImpl *collection = new PluginCollectionImpl;
    collection->set_data(results);
    return collection;
}

// Don't support search criteria.
IPluginUnknown *
PluginDocImpl::create_search_criteria_impl(IPluginUnknown  *thiz )
{
    return new PluginSearchCriteria;
}

PluginStatus
PluginDocImpl::request_search_next_impl(IPluginUnknown      *thiz,
                                        IPluginUnknown      *criteria,
                                        const UDSString     *from_anchor,
                                        const unsigned int  search_id)
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->doc_ctrl.search_next(
        (static_cast<PluginSearchCriteria*>(criteria))->get_data()
        , from_anchor->get_buffer(from_anchor)
        , search_id))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus
PluginDocImpl::request_search_all_impl(IPluginUnknown *thiz,
                                       IPluginUnknown *criteria,
                                       const unsigned int  search_id)
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->doc_ctrl.search_all(
        (static_cast<PluginSearchCriteria*>(criteria))->get_data()
        , search_id))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

PluginStatus
PluginDocImpl::abort_search_impl(IPluginUnknown *thiz,
                                 const unsigned int  search_id)
{
    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->doc_ctrl.abort_search(search_id))
    {
        return PLUGIN_OK;
    }
    return PLUGIN_FAIL;
}

void PluginDocImpl::on_view_released(ViewPtr view)
{
    ViewsIter iter = std::find(views.begin(), views.end(), view);
    if (iter != views.end())
    {
        delete *iter;
        views.erase(iter);
    }
}

void PluginDocImpl::on_search_results_ready(SearchResult res
                                            , PDFRangeCollection* coll
                                            , unsigned int search_id)
{
    PluginEventAttrs attrs;

    PluginCollectionImpl * results = new PluginCollectionImpl;
    results->set_data(coll);

    attrs.search_end.result = static_cast<IPluginUnknown *>(results);
    attrs.search_end.search_id = search_id;

    PluginEvent e = EVENT_SEARCH_END;
    if (res == RES_ABORTED)
    {
        e = EVENT_SEARCH_ABORTED;
    }

    listeners.broadcast(this, e, &attrs);
}

// Define pdf attribute.
PluginStatus
PluginDocImpl::get_attribute_impl(IPluginUnknown     *thiz,
                                  const UDSString    *key, 
                                  UDSString          *value )
{
	// WARNPRINTF("get_attribute_impl(\"%s\")", key->get_buffer(key));
	PluginDocImpl *instance = g_instances_table.get_object(thiz);

    // Check whether the key exists or not.
    if (instance->doc_attr_map.find(key->get_buffer(key)) 
            == instance->doc_attr_map.end())
    {
        return PLUGIN_NOT_SUPPORTED;
    }

    value->assign(value, instance->doc_attr_map[key->get_buffer(key)].c_str());
    return PLUGIN_OK;
}

PluginStatus 
PluginDocImpl::set_attribute_impl(IPluginUnknown     *thiz,
                                  const UDSString    *key,
                                  const UDSString    *value )
{
	// WARNPRINTF("set_attribute_impl(\"%s\",\"%s\")", key->get_buffer(key), value->get_buffer(value));
	PluginDocImpl *instance = g_instances_table.get_object(thiz);

    const char* key_cstr = key->get_buffer(key);
    DocAttrMapIter iter = instance->doc_attr_map.find(key_cstr);
    if (iter == instance->doc_attr_map.end())
    {
        return PLUGIN_NOT_SUPPORTED;
    }

    iter->second = value->get_buffer(value);
    return PLUGIN_OK;
}

PluginBool 
PluginDocImpl::is_dictionary_impl(IPluginUnknown  *thiz )
{
    return PLUGIN_FALSE;
}

PluginDocObjectType 
PluginDocImpl::get_type_of_object_impl(IPluginUnknown      *thiz,
                                       const PluginRange   *range)
{
	// Parse the range.
    UDSString * start = range->start_anchor;
    PDFAnchor start_param(start->get_buffer(start));

    UDSString * end = range->end_anchor;
    PDFAnchor end_param(end->get_buffer(end));

	WARNPRINTF("get_type_of_object_impl(%s, %s)", start->get_buffer(start), end->get_buffer(end));
	
    // TODO. Use bit to indicate a type, instead of integer
    if (start_param.link_idx >= 0 && end_param.link_idx >= 0)
    {
        if (start_param.word_num >= 0 && end_param.word_num >= 0)
        {
            return PLUGIN_DOC_OBJECT_TEXT_HYPERLINK;
        }
        return PLUGIN_DOC_OBJECT_HYPERLINK;
    }

    if (start_param.toc_idx >= 0 && end_param.toc_idx >= 0)
    {
        return PLUGIN_DOC_OBJECT_HYPERLINK;
    }

    if (start_param.word_num >= 0 && end_param.word_num >= 0)
    {
        return PLUGIN_DOC_OBJECT_TEXT;
    }
	return PLUGIN_DOC_OBJECT_TEXT; // DEBUG
    //return PLUGIN_DOC_OBJECT_NONE;
}

PluginStatus 
PluginDocImpl::get_words_from_range_impl(IPluginUnknown     *thiz, 
                                         const PluginRange  *chars_range, 
                                         PluginRange        *words_range )
{
    return PLUGIN_NOT_SUPPORTED;
}

PluginStatus 
PluginDocImpl::get_text_from_range_impl(IPluginUnknown        *thiz, 
                                        const PluginRange     *range, 
                                        UDSString             *result)
{
	// Parse the range.
    UDSString * start = range->start_anchor;
    PDFAnchor start_param(start->get_buffer(start));

    UDSString * end = range->end_anchor;
    PDFAnchor end_param(end->get_buffer(end));

	WARNPRINTF("get_text_from_range_impl(%s, %s)", start->get_buffer(start), end->get_buffer(end));	
	
    // Make the PDFAnchors valid for getting text.
    start_param.validate_for_getting_text();
    end_param.validate_for_getting_text();

    std::string result_str;

    PluginDocImpl *instance = g_instances_table.get_object(thiz);
    if (instance->doc_ctrl.get_text_from_range(start_param, end_param, result_str))
    {
		WARNPRINTF("result: %s", result_str.c_str());
        result->assign(result, result_str.c_str());
		return PLUGIN_OK;
    }
	WARNPRINTF("no text");
    return PLUGIN_FAIL;
}

}   // namespace pdf

