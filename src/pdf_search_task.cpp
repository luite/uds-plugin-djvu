/*
 * File Name: pdf_search_task.cpp
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

#include "log.h"

#include "pdf_search_task.h"

namespace pdf
{

PDFSearchTask::PDFSearchTask(const PDFSearchCriteria &c
                             , const string &anchor
                             , PDFSearchType t
                             , PDFSearcher *s
                             , unsigned int id)
: search_type(t)
, searcher(s)
, criteria(c)
, from_anchor(anchor)
, search_id(id)
{
    type = TASK_SEARCH;
}

PDFSearchTask::~PDFSearchTask()
{
}

void PDFSearchTask::execute()
{
    LOGPRINTF("Execute Search Task\n");
    reset();
    SearchResult res = RES_NOT_FOUND;

    // construct a container to keep all of the search results
    PDFSearchDocument results;
    
    if (search_type == PDF_SEARCH_ALL)
    {
        if (searcher->begin_search_all(criteria))
        {
            res = searcher->seach_all(results, this);
        }
    }
    else if (search_type == PDF_SEARCH_NEXT)
    {
        if (searcher->begin_search_next(criteria, from_anchor))
        {
            res = searcher->search_next(results, this);
        }
    }

    if (res == RES_PAUSED)
    {
        // the search task might restart later, so update the from anchor
        searcher->dump_search_process(from_anchor);
    }
    else if (res != RES_ABORTED)
    {
        // broadcast
        searcher->notify(res, results, search_id);
    }
}

void* PDFSearchTask::get_user_data()
{
    return searcher->get_doc_ctrl();
}

unsigned int PDFSearchTask::get_id()
{
    return search_id;
}

}// namespace pdf

