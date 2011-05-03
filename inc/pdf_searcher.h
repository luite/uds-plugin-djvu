/*
 * File Name: pdf_searcher.h
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

#ifndef PDF_SEARCHER_H_
#define PDF_SEARCHER_H_

#include "pdf_define.h"
#include "pdf_collection.h"
#include "pdf_search_criteria.h"

namespace pdf
{

/// @brief the search context wrappers not only the search criteria,
/// but also the page number and start index. It would be only used in our
/// searching algorithm.
struct SearchContext
{
    bool match_whole_word;
    bool case_sensitive;
    bool search_all;
    bool forward;
    int  word_cursor;
    int  char_cursor;
    int  page_num;
    stringlist dst_words;

    SearchContext(): match_whole_word(true)
        , case_sensitive(true)
        , search_all(false)
        , forward(true)
        , word_cursor(0)
        , char_cursor(0)
        , page_num(1)
        , dst_words() {}
    ~SearchContext() {}
};

/// @brief Information of a search task
// TO BE REMOVED
struct PDFSearchTaskInfo
{
    unsigned int ref_id;
    bool search_all;
    bool forward;

    void operator = (PDFSearchTaskInfo &right)
    {
        this->ref_id = right.ref_id;
        this->search_all = right.search_all;
        this->forward = right.forward;
    }
};
//

typedef PDFElemCollection<PluginRangeImpl*, int> PDFSearchPage;

typedef PDFCollection<PDFSearchPage*> PDFSearchDocument;

typedef enum
{
    RES_OK = 0,
    RES_NOT_FOUND,
    RES_BEGIN,
    RES_END,
    RES_ERROR,
    RES_ABORTED,
    RES_PAUSED
}SearchResult;

/// @brief PDFSearcher provides the searching function
class PDFController;
class PDFSearchTask;
class PDFSearcher
{
public:
    explicit PDFSearcher(PDFController *doc)
        : doc_controller(doc)
        , search_ctx()
    {}

    ~PDFSearcher()
    {}

    /// construct a search context for the "search next" task
    bool begin_search_next(const PDFSearchCriteria &criteria
                           , const string &from_anchor);

    /// construct a search context for the "search all" task
    bool begin_search_all(const PDFSearchCriteria &criteria);

    /// search the next word
    SearchResult search_next(PDFSearchDocument &results, PDFSearchTask *task);

    /// search in the whole document
    SearchResult seach_all(PDFSearchDocument &results, PDFSearchTask *task);

    /// dump the current search process, for restarting search task
    bool dump_search_process(string &anchor);

    /// notify the listeners that the searching is done
    void notify(SearchResult ret_code, PDFSearchDocument &results
        , unsigned int search_id);

    /// get the pointer of PDFController instance
    PDFController* get_doc_ctrl() const { return doc_controller; }

    /// export the PDFSearchDocument to PDFSearchCollection for UDS.
    /// content of the doc would be cleared after this function.
    /// TODO. This function and PDFSearchDocument should be removed due to
    /// the low efficiency.
    static void export_search_doc_to_coll(PDFSearchDocument &doc
        , PDFRangeCollection &collection);

private:
    /// Search in the current PDFPage
    SearchResult search_current_page(SearchContext &ctx
                                     , PDFSearchPage &results);

    /// Parse the destination string
    void parse_dst_string(const string &dst_str, stringlist &str_list);

    /// Clear the search context
    void clear_search_ctx();

private:
    // Reference to PDF renderer
    PDFController *doc_controller;

    // Search context contains the necessary status when executing a search
    // task
    SearchContext search_ctx;

};

};

#endif //PDF_SEARCHER_H_

