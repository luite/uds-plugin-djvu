/*
 * File Name: pdf_search_task.h
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

#ifndef PDF_SEARCH_TASK_H_
#define PDF_SEARCH_TASK_H_

#include "task.h"

#include "pdf_define.h"
#include "pdf_searcher.h"

namespace pdf
{

/// @brief The search task.
class PDFSearchTask : public Task
{
public:
    PDFSearchTask(const PDFSearchCriteria &c
                  , const string &anchor
                  , PDFSearchType t
                  , PDFSearcher *s
                  , unsigned int id);

    virtual ~PDFSearchTask();

    /// @brief execute the rendering task
    void  execute();

    /// @brief get the pointer of document instance
    void* get_user_data();

    /// @brief get search id
    unsigned int get_id();

private:
    // The search type
    PDFSearchType search_type;

    // The reference to the searcher
    PDFSearcher *searcher;

    // The search criteria
    PDFSearchCriteria criteria;

    // The from anchor
    string from_anchor;

    // The search id
    unsigned int search_id;
};

};//namespace pdf

#endif //PDF_SEARCH_TASK_H_

