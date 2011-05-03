/*
 * File Name: pdf_search_criteria.h
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

#ifndef PDF_SEARCH_CRITERIA_H_
#define PDF_SEARCH_CRITERIA_H_

#include "pdf_define.h"

namespace pdf
{

/// @brief PDFSearchCriteria
struct PDFSearchCriteria
{
    string  text;
    bool    case_sensitive;
    bool    match_whole_word;
    bool    forward;

    PDFSearchCriteria()
        : text(), case_sensitive(false), match_whole_word(false), forward(false)
    {}

    explicit PDFSearchCriteria(const PDFSearchCriteria &right)
        : text(right.text), case_sensitive(right.case_sensitive)
        , match_whole_word(right.match_whole_word)
        , forward(right.forward)
    {
    }

    ~PDFSearchCriteria() {}
};

};

#endif // PDF_SEARCH_CRITERIA_H_


