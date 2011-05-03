/*
 * File Name: pdf_doc_controller.cpp
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

#include "pdf_library.h"
#include "pdf_doc_controller.h"
#include "pdf_render_task.h"
#include "pdf_search_task.h"
#include "pdf_anchor.h"

#ifdef WIN32
#include "poppler/SecurityHandler.h"
void CDECL error(int pos, char *msg, ...) {
}

void * StandardSecurityHandler::getAuthData()
{
    return 0;
}
#endif

namespace pdf
{

// Wrap the global parameters, it is a sigleton class
class PDFGlobalParams
{
public:
    ~PDFGlobalParams()
    {
        // Global variables will be destoried automatically.
        delete globalParams;
        globalParams = 0;
    }

    static PDFGlobalParams & make_instance()
    {
        static PDFGlobalParams params;
        return params;
    }

private:
    PDFGlobalParams()
    {
        //Initialize the global params
        // Notes: globalParams is defined in poppler library.
#ifdef WIN32
        globalParams = new GlobalParams("");
#else        
        globalParams = new GlobalParams();
#endif       
    }

    PDFGlobalParams(const PDFGlobalParams&);
};

PDFController::PDFController(void)
: pages_cache()
, pdf_doc(0)
, toc(this)
, renderer()
, current_page_num(1)
, searcher(this)
, file_name()
, prerender_policy(new PDFPrerenderPolicyNormal)
{
    PDFLibrary::instance().add_document(this);
}

PDFController::~PDFController(void)
{
    PDFLibrary::instance().remove_document(this);

    // close the previous document if it exists
    if (pdf_doc)
    {
        close();
    }

    delete prerender_policy;
}

PluginStatus PDFController::open(const string &path)
{
    // close the previous document if it exists
    if (pdf_doc)
    {
        close();
    }

    // initialize the pdf global parameters
    PDFGlobalParams::make_instance();

    GooString * name = new GooString(path.c_str());

    // create PDFDoc instance
    pdf_doc = new PDFDoc(name);

    if (!pdf_doc->isOk())
    {
        ERRORPRINTF("could not open poppler doc %s", path.c_str());
        return PLUGIN_ERROR_OPEN_FILE;
    }

    if (!renderer.initialize(this))
    {
        return PLUGIN_ERROR_OPEN_FILE;
    }

    // set the file name
    file_name = path;
    return PLUGIN_OK;
}

bool PDFController::close()
{
    // remove all of the tasks related to this document
    PDFLibrary::instance().remove_tasks_by_document(this);

    renderer.destroy();

    if (pdf_doc != 0)
    {
        delete pdf_doc;
        pdf_doc = 0;
    }

    return true;
}

PDFToc * PDFController::get_toc(void)
{
    return &toc;
}

unsigned int PDFController::page_count()
{
    if (pdf_doc == 0)
    {
        return 0;
    }

    return pdf_doc->getNumPages();
}

bool PDFController::get_anchor_of_page(const unsigned int page_number
                                       , string &anchor)
{
	// DEBUG
	WARNPRINTF("get_anchor_of_page: %d", page_number);
	// END DEBUG
	PDFAnchor param;
    param.page_num = page_number;
    //param.file_name = name();

    anchor = param.get_string();
    return true;
}

unsigned int PDFController::get_page_number_of_anchor(const string &anchor)
{
    PDFAnchor param(anchor);
    return static_cast<unsigned int>(param.page_num);
}

PagePtr PDFController::get_page(const int page_num)
{
    return pages_cache.get_page(page_num);
}

int PDFController::compare_anchor(const string & first,
                   const string & second)
{
    PDFAnchor first_param(first);
    PDFAnchor second_param(second);
    return compare_anchor_param(first_param, second_param);
}

int PDFController::compare_anchor_param(const PDFAnchor & first_param,
                                        const PDFAnchor & second_param)
{
    return PDFAnchor::compare(first_param, second_param);
}

bool PDFController::get_hyperlinks_in_page(int page_num,
                                           PDFAnchor *start_param,
                                           PDFAnchor *end_param,
                                           PDFRangeCollection &results)
{
    PagePtr page = get_page(page_num);
    Links *links = 0;
    bool destroy_links = false;
    bool ret = true;
    if (page == 0 || page->get_links() == 0)
    {
        links = pdf_doc->generateLinks(page_num);
        destroy_links = true;
    }
    else
    {
        links = page->get_links();
    }

    if (links->getNumLinks() <= 0)
    {
        ret = false;
    }

    // get all of the hyperlinks from this page
    for (int i = 0; i < links->getNumLinks(); ++i)
    {
        PDFAnchor link_start, link_end;
        link_start.page_num = link_end.page_num = page_num;
        link_start.link_idx = link_end.link_idx = i;
        //link_start.file_name = link_end.file_name = name();

        int comp_ret_start = start_param != 0 ? PDFAnchor::compare(link_start, *start_param) :
            1;
        int comp_ret_end = end_param != 0 ? PDFAnchor::compare(link_end, *end_param) :
            1;

        if (comp_ret_start == 1 && comp_ret_end == 1)
        {
            PluginRangeImpl *result = new PluginRangeImpl;
            result->start_anchor = new StringImpl(link_start.get_string());
            result->end_anchor = new StringImpl(link_end.get_string());
            results.add(result);
        }
    }

    if (destroy_links)
    {
        delete links;
        links = 0;
    }
    return ret;
}

PDFCollectionBase* PDFController::get_hyperlinks_from_range(const string &start,
                                                            const string &end)
{
    PDFAnchor start_param(start), end_param(end);

    PDFRangeCollection *results = new PDFRangeCollection;
    if (start_param.page_num == end_param.page_num)
    {
        get_hyperlinks_in_page(start_param.page_num, &start_param, &end_param
            , *results);
        return results;
    }

    get_hyperlinks_in_page(start_param.page_num, &start_param, 0, *results);

    if (end_param.is_end_anchor())
    {
        return results;
    }
    
    int cur_page_num = start_param.page_num + 1;
    while (cur_page_num < end_param.page_num)
    {
        get_hyperlinks_in_page(cur_page_num, 0, 0, *results);
        cur_page_num ++;
    }
    
    get_hyperlinks_in_page(cur_page_num, 0, &end_param, *results);

    return results;
}

bool PDFController::get_text_from_range(const PDFAnchor &start,
                                        const PDFAnchor &end,
                                        string &result)
{
    bool ret = false;
    result.clear();

    PagePtr page = 0;
    if ( start.page_num == end.page_num || end.is_end_anchor() )
    {
        page = get_page(start.page_num);
        ret = page->get_text_by_range(start, end, result);
    }
    else
    {
        // make an end anchor
        PDFAnchor end_anchor;
        end_anchor.set_end_anchor();
        string  text;

        // get text from start page
        page = get_page(start.page_num);
        ret = page->get_text_by_range(start, end_anchor, text);
        if (!ret)
        {
            return false;
        }
        result += text;

        // get text from middle pages
        for (int idx = (start.page_num + 1); idx < end.page_num; ++idx)
        {
            // make start anchor of current page
            PDFAnchor start_anchor;
            start_anchor.page_num = idx;
            start_anchor.word_num = 0;

            page = get_page(idx);
            ret = page->get_text_by_range(start_anchor, end_anchor, text);
            if (!ret)
            {
                break;
            }

            result += text;
        }

        // get text from the last page
        PDFAnchor start_anchor;
        start_anchor.page_num = end.page_num;
        start_anchor.word_num = 0;

        page = get_page(end.page_num);
        ret = page->get_text_by_range(start_anchor, end, text);
        if (!ret)
        {
            return false;
        }
        result += text;
    }
    return ret;
}

bool PDFController::get_prev_page_anchor(string & anchor)
{
    PDFAnchor current_page(anchor);
    int prev_page = current_page.page_num - 1;
    if (prev_page < 1)
    {
        return false;
    }

    current_page.page_num  = prev_page;
    //current_page.file_name = name();
    anchor = current_page.get_string();
    return true;
}

bool PDFController::get_next_page_anchor(string & anchor)
{
    PDFAnchor current_page(anchor);
    int next_page = current_page.page_num + 1;
    if (next_page > static_cast<int>(page_count()))
    {
        return false;
    }

    current_page.page_num  = next_page;
    //current_page.file_name = name();
    anchor = current_page.get_string();
    return true;
}

bool PDFController::search_next(const PDFSearchCriteria &criteria
                                , const string &from_anchor
                                , unsigned int search_id)
{
    PDFSearchTask *task = new PDFSearchTask(criteria, from_anchor
        , PDF_SEARCH_NEXT, &searcher, search_id);

    PDFLibrary::instance().thread_add_search_task(task);

    return true;
}

bool PDFController::search_all(const PDFSearchCriteria &criteria
                               , unsigned int search_id)
{
    string s;
    PDFSearchTask *task = new PDFSearchTask(criteria, s
        , PDF_SEARCH_ALL, &searcher, search_id);

    PDFLibrary::instance().thread_add_search_task(task);

    return true;
}

bool PDFController::abort_search(unsigned int search_id)
{
    return PDFLibrary::instance().get_thread().abort_task(this, TASK_SEARCH
        , search_id);
}

double PDFController::get_page_crop_width(const int page_num)
{
    PluginRotationDegree rotation = get_page_original_rotation(page_num);
    int width = 0;
    double crop_width  = pdf_doc->getPageCropWidth(page_num);
    double crop_height = pdf_doc->getPageCropHeight(page_num);

    if (rotation == Clockwise_Degrees_90 ||
        rotation == Clockwise_Degrees_270  )
    {
        width = static_cast<int>(crop_height);
    }
    else
    {
        width = static_cast<int>(crop_width);
    }
    return width * renderer.get_view_attr().get_device_dpi_h() / 72.0f;
}

double PDFController::get_page_crop_height(const int page_num)
{
    PluginRotationDegree rotation = get_page_original_rotation(page_num);
    int height = 0;
    double crop_width  = pdf_doc->getPageCropWidth(page_num);
    double crop_height = pdf_doc->getPageCropHeight(page_num);

    if (rotation == Clockwise_Degrees_90 ||
        rotation == Clockwise_Degrees_270  )
    {
        height = static_cast<int>(crop_width);
    }
    else
    {
        height = static_cast<int>(crop_height);
    }
    return height * renderer.get_view_attr().get_device_dpi_v() / 72.0f;
}

bool PDFController::get_page_content_area(const string &anchor, RenderArea &area)
{
    int page_number = get_page_number_of_anchor(anchor);

    if (page_number <= 0 || page_number > static_cast<int>(page_count()))
    {
        // invalid page number
        ERRORPRINTF("Invalid Page Number when getting content area");
        return false;
    }

    PagePtr page = get_page(page_number);

    if (page == 0)
    {
        return false;
    }

    return page->get_content_area(&renderer, area);
}

bool PDFController::make_enough_memory(const int page_num, const int length)
{
    return pages_cache.make_enough_memory(page_num, length);
}

void PDFController::clear_cached_bitmaps()
{
    pages_cache.clear_cached_bitmaps();
}

void PDFController::update_memory_usage(const int length)
{
    pages_cache.update_mem_usage(length);
}

bool PDFController::get_page_crop_width(const string &anchor, double &width)
{
    int page_number = get_page_number_of_anchor(anchor);

    if (page_number > 0 && page_number <= static_cast<int>(page_count()))
    {
        width = get_page_crop_width(page_number);
        return true;
    }

    return false;
}

bool PDFController::get_page_crop_height(const string &anchor, double &height)
{
    int page_number = get_page_number_of_anchor(anchor);

    if (page_number > 0 && page_number <= static_cast<int>(page_count()))
    {
        height = get_page_crop_height(page_number);
        return true;
    }

    return false;
}

PluginRotationDegree PDFController::get_page_original_rotation(const int page_num)
{
    PluginRotationDegree rotation = Clockwise_Degrees_0;

    switch( pdf_doc->getPageRotate(page_num) )
    {
        case 90:
            rotation = Clockwise_Degrees_90;
            break;
        case 180:
            rotation = Clockwise_Degrees_180;
            break;
        case 270:
            rotation = Clockwise_Degrees_270;
            break;
        default:
            ; //ignore
    }

    return rotation;
}

PluginRotationDegree PDFController::get_page_original_rotation(const string &anchor)
{
    int page_number = get_page_number_of_anchor(anchor);

    if (page_number <= 0 || page_number > static_cast<int>(page_count()))
    {
        // invalid page number
        return Clockwise_Degrees_0;
    }

    return get_page_original_rotation(page_number);
}

bool PDFController::is_anchor_in_current_document(const string &anchor)
{
    PDFAnchor anchor_param(anchor);

    // check the file name
    if (!anchor_param.file_name.empty() &&
        anchor_param.file_name != name())
    {
        return false;
    }

    // check the page number
    if (anchor_param.page_num <= 0 ||
        anchor_param.page_num > static_cast<int>(page_count()))
    {
        return false;
    }

    return true;
}

bool PDFController::get_bounding_rectangles(const string &start_anchor
                                          , const string &end_anchor
                                          , PDFRectangles &rects)
{
    int page_num = get_page_number_of_anchor(start_anchor);

    //Get the rendered page
    PagePtr page = get_page(page_num);
    if (!page)
    {
        return false;
    }

    return page->get_bounding_rectangles(start_anchor, end_anchor, rects);
}

bool PDFController::set_memory_limit(const unsigned int bytes)
{
    // clear all of the render tasks related to this document
    PDFLibrary::instance().thread_cancel_render_tasks(this);

    int size = static_cast<int>(bytes >> 1);
    TRACE("Set memory:%d to document:%s\n", size, file_name.c_str());
    return pages_cache.reset(size);
}

unsigned int PDFController::get_memory_limit()
{
    return pages_cache.size();
}

} //namespace pdf


