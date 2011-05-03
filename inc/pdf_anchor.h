/*
 * File Name: pdf_anchor.h
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

#ifndef PDF_ANCHOR_H_
#define PDF_ANCHOR_H_

#include "pdf_define.h"

namespace pdf
{

class PDFAnchor
{
public:
    int     page_num;      ///< The page number, default as 1
    int     word_num;      ///< The word offset, default as -1
    int     char_idx;      ///< The index of the charactor, default as -1
    int     link_idx;      ///< The link number, default as -1
    int     toc_idx;       ///< The index of the toc item
    string  file_name;     ///< Name of the file which is pointed by the anchor

public:
    PDFAnchor();
    PDFAnchor(const string &anchor);
    ~PDFAnchor();

    bool operator == (const PDFAnchor &right) const;

    /// Estimate whether it is an end anchor
    bool is_end_anchor() const;

    /// Set this anchor to be end anchor
    void set_end_anchor();

    /// Get the string of this anchor
    const string& get_string();

    /// return 1 means a1 > a2
    /// return -1 means a1 < a2
    /// return 0 means a1 = a2
    /// return ANCHOR_COMPARE_ERROR means error
    static int compare(const PDFAnchor &a1, const PDFAnchor &a2);

    void validate_for_getting_text(void);

private:
    string    anchor_str;

private:
    bool operator < (const PDFAnchor &right) const;
    bool operator > (const PDFAnchor &right) const;

    /// Parse the anchor string to pdf parameters
    bool parse(const string &anchor);

    /// Package the pdf parameters into anchor string
    bool pack(string &anchor);

    /// Reset all of the parameters to default value
    void reset();

    /// Get number of a sub string
    int get_num(const string &page_sub_str);

    /// Get prefix of a sub string
    const string get_prefix(const string &str);

    /// Get postfix of a sub string
    const string get_postfix(const string &str);

};

};//namespace pdf

#endif //PDF_ANCHOR_H_

