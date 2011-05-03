/*
 * File Name: pdf_searcher.cpp
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

#include "pdf_doc_controller.h"
#include "pdf_searcher.h"
#include "pdf_anchor.h"
#include "pdf_search_task.h"


namespace pdf
{

using namespace std;

bool is_ignore_char(const char c)
{
    return (c == ' ');
}

bool is_end_char(const char c)
{
    return (c == '\0');
}

void PDFSearcher::clear_search_ctx()
{
    search_ctx.dst_words.clear();
}

bool PDFSearcher::begin_search_next(const PDFSearchCriteria &criteria
                                    , const string &from_anchor)
{
    PDFAnchor from_param(from_anchor);
    if (from_param.page_num <= 0 
        || from_param.page_num > 
        static_cast<int>(doc_controller->page_count()))
    {
        from_param.page_num = 1;
    }

    clear_search_ctx();
    // construct the search context
    
    search_ctx.case_sensitive = criteria.case_sensitive;
    search_ctx.match_whole_word = criteria.match_whole_word;
    search_ctx.page_num = from_param.page_num;
    search_ctx.forward = criteria.forward;
    search_ctx.search_all = false;
    search_ctx.word_cursor = from_param.word_num;
    search_ctx.char_cursor = from_param.char_idx;

    if (search_ctx.forward)
    {
        // move forward the start char index
        search_ctx.char_cursor++;
    }
    else
    {
        // move backward the start char index
        // NOTE: We should care about the boundary problem
        // the start anchor might be the last word and the last char
        search_ctx.char_cursor--;
    }

    parse_dst_string(criteria.text, search_ctx.dst_words);

    return true;
}

bool PDFSearcher::begin_search_all(const PDFSearchCriteria &criteria)
{
    clear_search_ctx();

    // construct the search context
    search_ctx.case_sensitive = criteria.case_sensitive;
    search_ctx.match_whole_word = criteria.match_whole_word;
    
    // start from the first page
    search_ctx.page_num = 1;
    //search_ctx.forward = criteria.forward;
    search_ctx.forward = true;
    search_ctx.search_all = true;
    
    // start from the first word
    search_ctx.word_cursor = 0;
    search_ctx.char_cursor = 0;
    parse_dst_string(criteria.text, search_ctx.dst_words);

    return true;
}

SearchResult PDFSearcher::search_next(PDFSearchDocument &results
                                      , PDFSearchTask *task)
{

    PDFSearchPage *search_page = new PDFSearchPage;
    SearchResult res = RES_NOT_FOUND;
    while(res != RES_OK
          && search_ctx.page_num > 0
          && search_ctx.page_num <=
             static_cast<int>(doc_controller->page_count()))
    {
        res = search_current_page(search_ctx, *search_page);

        if (res != RES_OK)
        {
            // forward : increase page number; otherwise decrease page number
            search_ctx.forward ? search_ctx.page_num++
                : search_ctx.page_num--;

            // if it is not the first page, start from the first word if forward
            // else start from the last word
            search_ctx.word_cursor = -1;
        }

        // abort current task
        if (task->is_aborted())
        {
            LOGPRINTF("Task Search Next canceled!\n");
            res = RES_ABORTED;
            break;
        }
        else if (task->is_paused())
        {
            LOGPRINTF("Task Search Next paused!\n");
            res = RES_PAUSED;
            break;
        }
    }

    if (res != RES_OK)
    {
        delete search_page;
    }
    else
    {
        search_page->set_element(search_ctx.page_num);
        results.add(search_page);
    }

    return res;

}

SearchResult PDFSearcher::seach_all(PDFSearchDocument &results
                                    , PDFSearchTask *task)
{
    // return code of this function
    SearchResult res = RES_NOT_FOUND;

    // return code of searching every page
    SearchResult res_once = res;

    PDFSearchPage *search_page = new PDFSearchPage;

    while ( search_ctx.page_num > 0
            && search_ctx.page_num <=
               static_cast<int>(doc_controller->page_count()))
    {
        
        // search the whole page if it is not the current page
        res_once = search_current_page(search_ctx, *search_page);

        if (res_once == RES_OK)
        {
            search_page->set_element(search_ctx.page_num);
            results.add(search_page);
            search_page = new PDFSearchPage;
        }
        
        // forward : increase page number; otherwise decrease page number
        search_ctx.page_num++;
 
        // reset the index of start word to be 0
        search_ctx.word_cursor = 0;

         // abort current task
        if (task->is_aborted())
        {
            LOGPRINTF("Task Search All canceled!\n");
            res = RES_ABORTED;
            break;
        }
        else if (task->is_paused())
        {
            LOGPRINTF("Task Search All paused!\n");
            res = RES_PAUSED;
            break;
        }
    }

    delete search_page;

    if (results.size() > 0)
    {
        res = RES_OK;
    }
    
    return res;
}

bool PDFSearcher::dump_search_process(string &anchor)
{
    PDFAnchor process;
    process.page_num = search_ctx.page_num;
    process.word_num = search_ctx.word_cursor;
    process.char_idx = search_ctx.char_cursor;
    //process.file_name = get_doc_ctrl()->name();

    anchor = process.get_string();
    return true;
}

SearchResult PDFSearcher::search_current_page(SearchContext &ctx
                                              , PDFSearchPage &results)
{
    // get the page directly from cache, we don't have to consider the
    // layout
    PagePtr cur_page = doc_controller->get_page(ctx.page_num);

    // if the page is cached in the rendering cache, return it
    if (cur_page == 0)
    {
        cur_page = doc_controller->get_renderer()->gen_page(ctx.page_num);
        if (cur_page == 0)
        {
            return RES_ERROR;
        }
    }

    TextPage* text_page = cur_page->get_text();
    bool need_remove_text = false;
    if (text_page == 0)
    {
        // render the text of current page
        cur_page->render_text(doc_controller->get_renderer(), true);
        text_page = cur_page->get_text();
        if (text_page == 0)
        {
            return RES_ERROR;
        }
        need_remove_text = true;
    }

    SearchResult res = cur_page->search(ctx, results);

    if (need_remove_text)
    {
        cur_page->destroy_text();
    }

    return res;
}

void PDFSearcher::notify(SearchResult ret_code, PDFSearchDocument &results
                         , unsigned int search_id)
{
    // export the results from PDFSearchDocument to PDFRangeCollection
    PDFRangeCollection *coll = new PDFRangeCollection;
    export_search_doc_to_coll(results, *coll);

    doc_controller->sig_search_results_ready.broadcast(ret_code, coll
        , search_id);
}

void PDFSearcher::parse_dst_string(const string &dst_str, stringlist &str_list)
{
    typedef enum
    {
        PARSE_WORD = 0,
        PARSE_IGNORE_CHAR
    }ParseStatus;

    const char *pchar = dst_str.c_str();

    //Parse the words
    string word;
    ParseStatus status = PARSE_IGNORE_CHAR;
    while(!is_end_char(*pchar))
    {
        //If the character is one of the ignore ones, ignore it.
        if (is_ignore_char(*pchar))
        {
            if (status == PARSE_WORD)
            {
                status = PARSE_IGNORE_CHAR;
                str_list.push_back(word);
                word.clear();
            }
        }
        else
        {
            if (status == PARSE_IGNORE_CHAR)
            {
                status = PARSE_WORD;
            }
            word.push_back(*pchar);
        }
        pchar++;
    }

    if (status == PARSE_WORD)
    {
        // add the last word
        str_list.push_back(word);
    }
}

void PDFSearcher::export_search_doc_to_coll(PDFSearchDocument &doc
                                            , PDFRangeCollection &collection)
{
    for(int i = 0; i < doc.size(); ++i)
    {
        PDFSearchPage *page = doc.get(i);
        for(int k = 0; k < page->size(); ++k)
        {
            PluginRangeImpl *range = page->get(k);
            if (range == 0)
            {
                break;
            }

            PluginRangeImpl *save_range = new PluginRangeImpl;
            save_range->start_anchor = new StringImpl(
                range->start_anchor->get_buffer(range->start_anchor));
            save_range->end_anchor = new StringImpl(
                range->end_anchor->get_buffer(range->end_anchor));

            collection.add(save_range);
        }
        page->clear();
    }
    doc.clear();
}

} // namespace pdf


