/*
 * File Name: pdf_doc_controller.h
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

#ifndef PDF_DOC_CONTROLLER_H_
#define PDF_DOC_CONTROLLER_H_

#include "pdf_define.h"
#include "pdf_page.h"
#include "pdf_renderer.h"
#include "pdf_searcher.h"
#include "pdf_observer.h"
#include "pdf_toc.h"
#include "pdf_pages_cache.h"
#include "pdf_collection.h"

namespace pdf
{

/// The document class.
class PDFController
{
public:
    /// Page has been rendered signal
    Signal<void, RenderResultPtr, RenderStatus> sig_page_ready;

    /// Search results have been generated signal
    Signal<void, SearchResult, PDFRangeCollection*, unsigned int>
        sig_search_results_ready;

public:
    PDFController(void);
    ~PDFController(void);

    /// Open a pdf document
    /// TODO. Return error code : PluginStatus, take the password into account
    PluginStatus open(const string &path);

    /// Close and release all allocated resource.
    bool close();

    /// Check whether the document has been opened.
    bool is_open() { return (pdf_doc && pdf_doc->isOk()); }

    /// Get the TOC
    PDFToc * get_toc(void);

    /// Get the file name
    const string& name() const { return file_name; }

    /// The pages number
    unsigned int page_count();

    /// Get current page
    int  get_cur_page_num() const { return current_page_num; }

    /// Get the anchor by page number
    bool get_anchor_of_page(const unsigned int page_number, 
                            string &anchor);

    /// Is the anchor in current document
    bool is_anchor_in_current_document(const string &anchor);

    /// Get the page number by anchor
    unsigned int get_page_number_of_anchor(const string &anchor);

    /// Compare the anchor object.
    int compare_anchor(const string &first,
                       const string &second);

    /// Get all of the hyperlinks from a given range
    PDFCollectionBase* get_hyperlinks_from_range(const string &start,
                                                 const string &end);

    /// Get the text from a given range
    bool get_text_from_range(const PDFAnchor &start,
                             const PDFAnchor &end,
                             string &result);

    /// Anchor iteraotr: retrieve the previous anchor.
    bool get_prev_page_anchor(string &anchor);

    /// Anchor iteraotr: retrieve the next anchor.
    bool get_next_page_anchor(string &anchor);

    /// Get the crop width/height of a page by anchor
    bool get_page_crop_width(const string &anchor, double &width);
    bool get_page_crop_height(const string &anchor, double &height);

    /// Get the content area of a page by anchor
    bool get_page_content_area(const string &anchor, RenderArea &area);


    /// Get the crop width/height of a page by number
    double get_page_crop_width(const int page_num);
    double get_page_crop_height(const int page_num);

    /// Get current display size
    int get_display_width() { return renderer.get_view_attr().get_display_width(); }
    int get_display_height() { return renderer.get_view_attr().get_display_height(); }

    /// Get the original rotation of a page
    PluginRotationDegree get_page_original_rotation(const int page_num);
    PluginRotationDegree get_page_original_rotation(const string &anchor);

    /// Get the PDFPage by page number
    PagePtr get_page(const int page_num);

    /// Search for the next record
    bool search_next(const PDFSearchCriteria &criteria
                     , const string &from_anchor
                     , unsigned int search_id);

    /// Search all of the matched words
    bool search_all(const PDFSearchCriteria &criteria
                    , unsigned int search_id);

    /// Abort a search task
    bool abort_search(unsigned int search_id);

    /// Get the characters rectangle of the range
    bool get_bounding_rectangles(const string &start_anchor
                                 , const string &end_anchor
                                 , PDFRectangles &rects);

    /// Get the renderer
    PDFRenderer* get_renderer() { return &renderer; }

    /// Get the prerender policy
    PDFPrerenderPolicy* get_prerender_policy() { return prerender_policy; }

    // Get the poppler PDF document
    PDFDoc* get_pdf_doc() {return pdf_doc;}

    /// Set/get the memory limitation value to the cache
    bool set_memory_limit(const unsigned int bytes);
    unsigned int get_memory_limit();

    /// Clear all of the cached pages
    void clear_cache() { pages_cache.clear(); }

private:
    // Compare between two anchor parameters
    int compare_anchor_param(const PDFAnchor & first_param,
                             const PDFAnchor & second_param);

    // Get the hyperlinks in a page
    bool get_hyperlinks_in_page(int page_num,
                                PDFAnchor *start_param,
                                PDFAnchor *end_param,
                                PDFRangeCollection &results);

    // Remove the old pages in cache to make enough memory
    bool make_enough_memory(const int page_num, const int length);

    // Clear all cached pages but the locked one
    void clear_cached_bitmaps();

    // Update the memory usage by adding the length of page
    void update_memory_usage(const int length);

    // Set the current page number
    void set_cur_page_num(const int page_num) { current_page_num = page_num; }

    // Get the pages cache
    PagesCache & get_pages_cache() { return pages_cache; }

private:
    // The pages cache
    PagesCache pages_cache;

    // The PDFDoc instance
    PDFDoc *pdf_doc;

    // The TOC instance of PDFDoc
    PDFToc toc;

    // The PDF renderer
    // Shall the renderer be maitained in document?
    PDFRenderer renderer;

    // The current page number
    int current_page_num;

    // The pdf searcher
    PDFSearcher searcher;

    // File name
    string file_name;

    // prerender policy
    PDFPrerenderPolicy *prerender_policy;

    friend class PDFRenderer;
    friend class PDFSearcher;
    friend class PDFPage;
    friend class PDFLibrary;
};

};

#endif //PDF_DOC_CONTROLLER_H_

