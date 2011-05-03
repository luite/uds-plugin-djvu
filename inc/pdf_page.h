/*
 * File Name: pdf_page.h
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

#ifndef PDF_PAGE_H_
#define PDF_PAGE_H_

#include "pdf_define.h"
#include "pdf_anchor.h"
#include "pdf_searcher.h"
#include "pdf_observer.h"

namespace pdf
{
typedef PDFInstanceCollection<PluginRectangle> PDFRectangles;

class PDFRenderAttributes
{
public:
    PDFRenderAttributes()
        : zoom_setting(PLUGIN_ZOOM_DEFAULT)
        , real_zoom_value(PLUGIN_ZOOM_DEFAULT)
        , rotate(0)
    {}

    PDFRenderAttributes(const PDFRenderAttributes &attr)
        : zoom_setting(attr.zoom_setting)
        , real_zoom_value(attr.real_zoom_value)
        , rotate(attr.rotate)
    {}

    ~PDFRenderAttributes() {}

    PDFRenderAttributes& operator=(const PDFRenderAttributes& right)
    {
        if (*this == right)
        {
            return *this;
        }

        zoom_setting = right.zoom_setting;
        real_zoom_value = right.real_zoom_value;
        rotate = right.rotate;
        return *this;
    }

    bool operator==(const PDFRenderAttributes &right) const
    {
        return ((fabs(this->zoom_setting - right.zoom_setting) < ZERO_RANGE) &&
                (fabs(this->real_zoom_value - right.real_zoom_value) < ZERO_RANGE) &&
                this->rotate == right.rotate);
    }

    void set_zoom_setting(double z) {zoom_setting = z;}
    double get_zoom_setting() const {return zoom_setting;}

    void set_rotate(int r) {rotate = r;}
    int get_rotate() const {return rotate;}

    void set_real_zoom_value(double z) {real_zoom_value = z;}
    double get_real_zoom_value() const {return real_zoom_value;}

private:
    // the zoom setting
    double zoom_setting;

    // the real zoom value
    double real_zoom_value;

    // the rotation degree
    int    rotate;

};

struct SearchWordRecord
{
    int word_index;
    int start_char_index;
    int end_char_index;

    SearchWordRecord()
        : word_index(0)
        , start_char_index(0)
        , end_char_index(0)
    {}

    SearchWordRecord(int word_idx, int start_idx, int end_idx)
        : word_index(word_idx)
        , start_char_index(start_idx)
        , end_char_index(end_idx)
    {}

    ~SearchWordRecord() 
    {}
};

typedef PDFInstanceCollection<SearchWordRecord> SearchWords;

class PDFController;
class PDFRenderer;
class TextWordQueue;
/// The page information.
class PDFPage
{
public:
    /// Render status, 
    /// it would be used when submitting a new render task
    typedef enum
    {
        RENDER_DONE = 0,
        RENDER_RUNNING,
        RENDER_STOP
    }RenderStatus;

public:
    PDFPage(int page_num, const PDFRenderAttributes &attr);

    ~PDFPage(void);

    ///  Return the size_t value based on the pagenumber
    size_t operator()();

    ///  Comparing
    bool operator == (const PDFPage &right);
    bool operator == (const PDFRenderAttributes &right);

    /// Destroy the resource of the page
    unsigned int destroy();

    ///  Lock the page so that it has the highest priority(cannot
    /// be removed until UDS unlocks it)
    void lock() { b_lock = true; }

    ///  Unlock the page
    void unlock() { b_lock = false; }

    ///  Is current page be locked
    bool locked() const { return b_lock; }

    ///  Set the render attributes
    ///  return true means setting succeed, or there is no change
    ///  return false means the page is in rendering, cannot change
    ///  the setting now
    void set_render_attr(const PDFRenderAttributes &attr);

    ///  Get the render attributes
    const PDFRenderAttributes& get_render_attr() const { return render_attr; }

    ///  Get/Set the reference id
    int get_ref_id() { return ref_id; }
    void set_ref_id(int id) { ref_id = id; }

    int get_page_num() const { return page_number; }
    int get_bitmap_width();
    int get_bitmap_height();
    int get_bitmap_row_stride();
    SplashBitmap* get_bitmap() const { return bitmap; }
    Links* get_links() const { return links; }
    TextPage* get_text() const { return text; }
    TextWordList* get_words_list();
    PDFController* get_doc_controller() const { return doc_controller; }
    void set_doc_controller(PDFController* doc) { doc_controller = doc; }

    ///  Get the data length of the bitmap
    unsigned int length();

    ///  Get the data of the bitmap
    const unsigned char* get_bitmap_data();

    /// Estimate whether the input is hyperlink page of current page
    bool is_hyper_linked_page(int dst_page_num);

    // Render functions
    ///  Render a splash page by render attributes passed in
    bool render_splash_map(PDFRenderer *renderer, void *abort_data);

    ///  Render a text page by render attributes passed in
    bool render_text(PDFRenderer *renderer, bool use_defalt_setting = false);

    // Search functions
    ///  Search in the current PDFPage. Make sure the text page is
    /// rendered before searching
    SearchResult search(SearchContext &ctx, PDFSearchPage &results);

    ///  Get the bounding rectangles in the passed-in range
    bool get_bounding_rectangles(const string &start_anchor
                                 , const string &end_anchor
                                 , PDFRectangles &rects);

    ///  Get rendering status
    RenderStatus get_render_status() {return render_status;}

    ///  Get the anchor of (x, y).
    void get_anchor_param_from_coordinates(double x, double y, PDFAnchor &param);

    ///  Get the destination's page number of the link
    int  get_goto_page_of_link(int link_index);

    ///  Get the destination's anchor of the link.
    bool get_goto_anchor_of_link(int link_index, std::string & anchor);

    /// Get the range of link by link index
    bool get_range_param_by_link_index(const int link_index,
                                       PDFAnchor & start_param,
                                       PDFAnchor & end_param);

    /// Get the range of word by word index
    bool get_range_param_by_word_index(const int link_index,
                                       PDFAnchor & start_param,
                                       PDFAnchor & end_param);

    /// Get the text by range
    bool get_text_by_range(const PDFAnchor & start_param,
                           const PDFAnchor & end_param,
                           std::string &result);

    /// Get content area of a page.
    /// The content area should be caculated by the result of a thumbnail rendering
    bool get_content_area(PDFRenderer *renderer, RenderArea &area);
    const RenderArea & get_content_area() const { return content_area; }

    ///  Translate point from device coordination to user's
    void coordinates_dev_to_user(const double dx, const double dy, 
                                 double *ux, double *uy);

    void coordinates_user_to_dev(const double ux, const double uy, 
                                 int *dx, int *dy);

private:
    // Data structures for searching
    typedef enum
    {
        STATUS_HEADER = 0, //for searching words list
        STATUS_BODY, //for searching words list
        STATUS_TAIL, //for searching words list
    }MatchStatus;

private:
    //Initialize the page
    void init();

    // Set render status
    void set_render_status(RenderStatus s);

    // Update render results
    void update_text(TextPage *t);
    void update_links(Links *l);
    void update_bitmap(SplashBitmap *m);

    // Destroy render results
    void destroy_text();
    unsigned int destroy_bitmap();
    void destroy_links();

    // Search destination string by forward order
    // return the index of searching position
    PluginRangeImpl* search_string_forward(SearchContext &ctx,
                                           TextWordList *words);

    // Search destination string by backward order
    // return the index of searching position
    PluginRangeImpl* search_string_backward(SearchContext &ctx,
                                            TextWordList *words);

    // Compare the single string, it is used when searching single word
    bool compare_string(const string &dst          // pattern string
                        , const string &src        // source string
                        , bool case_sensitive      // is case sensitive
                        , bool match_whole_word    // is match whole word
                        , bool forward             // is forward
                        , bool sub_string          // compare only substring by the start char index
                        , const int start_char_idx // start position of the source word
                        , int &start_result_idx    // start position of the searched results
                        );  

    // Generate the search result
    void generate_search_result(SearchContext &ctx
                                , SearchWords &queue
                                , PluginRangeImpl* &result
                                , bool forward);

    // Get content area from bitmap
    bool get_content_from_bitmap(SplashBitmap *bitmap, PDFRectangle &rect);

    // Rendering aborting function, dealing with the aborting request
    static GBool abort_render_check(void *data);

    // Try to calculate the size of a page
    static unsigned int try_calc_length(const double zoom_value
                                        , const double crop_width
                                        , const double crop_height);

private:
    ///  page number
    int     page_number;

    ///  the render-related attributes
    PDFRenderAttributes render_attr;

    // render result
    SplashBitmap    *bitmap;
    Links           *links;
    TextPage        *text;

    // Reference of the document
    PDFController   *doc_controller;

    // Lock the page
    bool   b_lock;

    // Status of rendering process
    RenderStatus render_status;

    // Reference id of this page
    int ref_id;

    // Content area of a page
    RenderArea content_area;

    // CTM and ICTM of a PDF page. It is used for retrieving rectangle of hyperlink
    double ctm[6];
    double ictm[6];

    friend class PDFRenderTask;
    friend class PDFSearcher;
};

typedef PDFPage* PagePtr;

};//namespace pdf

#endif //PDF_PAGE_H_


