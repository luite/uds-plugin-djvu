/*
 * File Name: pdf_toc.h
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

#ifndef _PDF_TOC_H_
#define _PDF_TOC_H_

#include "pdf_define.h"

namespace pdf
{

struct TocItem
{
    string anchor;        ///< The anchor.
    string text;          ///< The display text.
    string goto_anchor;   ///< The anchor of destination.
    struct TocItem * sibling;     ///< The next sibling.
    struct TocItem * first_child; ///< The first child.

    TocItem();
    ~TocItem();

    bool is_invisible_root();
    void disconnect();
};

class PDFController;
class PDFToc
{
public:
    PDFToc(PDFController *doc);
    ~PDFToc(void);

    // check, get and free table of content
    bool has_toc();
    TocItem * get_toc(void);
    void free_toc(TocItem * p_toc);

    bool get_goto_anchor_of_toc_idx(int idx, string & anchor);

private:
    /// Create a new toc item
    TocItem * create_new_item(OutlineItem * data, int idx);

    /// Append children of current toc item to toc tree
    /// This function will return the first child of the parent
    void append_toc_children(TocItem * parent, int & idx, GooList * items);

    /// Get the goto anchor of a toc item
    bool get_dest_page_of_toc_item(OutlineItem * item, int & page_num);

    PDFController *document() { return doc_ctrl; }

private:
    PDFController *doc_ctrl;   ///< Reference to the PDFDoc.
    TocItem *toc;              ///< The result of getting toc.
};

};
#endif // _PDF_TOC_H_


