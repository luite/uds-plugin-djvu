/*
 * File Name: pdf_toc.cpp
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

#include "pdf_toc.h"
#include "pdf_doc_controller.h"

namespace pdf
{

static const char * unicode_to_utf8(const Unicode * uni_str, int len)
{
    gsize bytes_read;
    gsize bytes_written;
    GError *error = NULL;

    gchar * utf8_str = g_convert((const gchar *)uni_str, len * 4,
                                 "UTF-8", "UCS-4LE",
                                 &bytes_read,
                                 &bytes_written,
                                 &error);

    //LOGPRINTF("%s", utf8_str);

    return utf8_str;
}

TocItem::TocItem()
: anchor()
, text()
, goto_anchor()
, sibling(0)
, first_child(0)
{
}

TocItem::~TocItem()
{
    if (first_child != 0)
    {
        delete first_child;
    }

    if (sibling != 0)
    {
        delete sibling;
    }
}

bool TocItem::is_invisible_root()
{
    return anchor.empty();
}

void TocItem::disconnect()
{
    sibling = 0;
    first_child = 0;
}

PDFToc::PDFToc(PDFController *doc)
: doc_ctrl(doc)
, toc(0)
{
}

PDFToc::~PDFToc(void)
{
    free_toc(toc);
}

bool PDFToc::has_toc()
{
    Outline * outline = doc_ctrl->get_pdf_doc()->getOutline();
    if (!outline)
    {
        LOGPRINTF("No table of content.");
        return false;
    }

    GooList * items = outline->getItems();
    if (!items || items->getLength() < 1)
    {
        LOGPRINTF("No table of content.");
        return false;
    }

    return true;
}

TocItem * PDFToc::get_toc(void)
{
    if (toc)
    {
        // return the toc if it exists.
        return toc;
    }

    if (!has_toc())
    {
        return 0;
    }

    //assert(doc_ctrl != 0);
    if (doc_ctrl == 0)
    {
        return 0;
    }

    Outline * outline = doc_ctrl->get_pdf_doc()->getOutline();
    GooList * items = outline->getItems();

    int idx = 0;
    // construct the invisible root node.
    TocItem root;

    append_toc_children(&root, idx, items);
    toc = root.first_child;

    // disconnect the root node
    root.disconnect();
    return toc;
}

TocItem * PDFToc::create_new_item(OutlineItem * data, int idx)
{
    // Get goto anchor
    int dst_page = 0;
    get_dest_page_of_toc_item(data, dst_page);

    // Get the anchor of destination page
    PDFAnchor goto_anchor;
    if (dst_page > 0)
    {
        goto_anchor.page_num = dst_page;
        //goto_anchor.file_name = document()->name();
    }
    else
    {
        ERRORPRINTF("Wrong TOC item:%d", idx);
        return 0;
    }

    Unicode * uni_char = data->getTitle();
    int title_length = data->getTitleLength();
    const char * text = unicode_to_utf8(uni_char, title_length);

    if (!text || !strlen(text))
    {
        WARNPRINTF("Can't get [%d]th TOC item.", idx);
        return 0;
    }

    // Get anchor
    PDFAnchor param;
    param.toc_idx   = idx;
    param.page_num  = dst_page;
    //param.file_name = document()->name();

    TocItem * item = new TocItem;

    item->anchor = param.get_string();
    item->text = string(text);
    item->goto_anchor = goto_anchor.get_string();
    item->sibling = 0;
    item->first_child = 0;

    g_free((gpointer)text);
    return item;
}

void PDFToc::append_toc_children(TocItem * parent, int & idx, GooList * items)
{
    int numItems = items->getLength();
    for ( int i = 0; i < numItems; ++i )
    {
        // Iterate over every object in 'items'
        OutlineItem * item = (OutlineItem *)items->get(i);
        TocItem * toc_item = create_new_item(item, idx++);

        if (toc_item != 0)
        {
            // the invalid toc item should not be displayed
            if (parent->first_child == 0)
            {
                parent->first_child = toc_item;
            }
            else
            {
                TocItem * cur = parent->first_child;
                while (cur->sibling != 0)
                {
                    cur = cur->sibling;
                }
                cur->sibling = toc_item;
            }

            // Recursively descend over children
            item->open();
            GooList * children = item->getKids();
            if (children)
            {
                append_toc_children(toc_item, idx, children);
            }
            item->close();
        }
    }
}

bool PDFToc::get_dest_page_of_toc_item(OutlineItem * item, int & page_num)
{
    // Find the page the link refers to
    LinkAction * action = item->getAction();
    if (action && (action->getKind() == actionGoTo))
    {
        // Caculate the page number of destination of this link.
        LinkDest * dest;
#ifdef WIN32
        UGooString *named_dest;
#else
        GooString *named_dest;
#endif
        page_num = 0;

        dest = ((LinkGoTo *)action)->getDest();
        named_dest = ((LinkGoTo *)action)->getNamedDest();

        if (dest && dest->isPageRef())
        {
            Ref pageRef = dest->getPageRef();
            page_num = doc_ctrl->get_pdf_doc()->findPage(pageRef.num, pageRef.gen);
        }
        else if (named_dest)
        {
            dest = doc_ctrl->get_pdf_doc()->findDest(named_dest);
            if (dest)
            {
                Ref pageRef = dest->getPageRef();
                page_num = doc_ctrl->get_pdf_doc()->findPage(pageRef.num, pageRef.gen);
            }
        }

        if (page_num > 0)
        {
            return true;
        }
    }

    return false;
}

void PDFToc::free_toc(TocItem * p_toc)
{
    //if (!p_toc)
    //{
    //    return;
    //}

    //if (p_toc->first_child)
    //{
    //    free_toc(p_toc->first_child);
    //}
    //else if (p_toc->sibling)
    //{
    //    free_toc(p_toc->sibling);
    //}
    //else 
    //{
    //    delete p_toc;
    //}
    if (p_toc != 0)
    {
        delete p_toc;
    }
}

// Notes, preorder algorithm which is same as the algorithm
// when calculating the toc_idx.
bool PDFToc::get_goto_anchor_of_toc_idx(int idx, string & anchor)
{
    int i = -1;

    typedef list<TocItem *> Stack;
    Stack stack;

    TocItem * cur = toc;
    stack.push_back(cur);

    while (!stack.empty()) 
    {
        // Go down to the highest level.
        cur = stack.back();
        while (cur) 
        {
            // Visit cur
            i++;

            // Condition judgement.
            if (i == idx)
            {
                /*LOGPRINTF("%d %s", idx, cur->goto_anchor.c_str());*/
                anchor = cur->goto_anchor.c_str();
                return true;
            }
            
            stack.push_back(cur->first_child);
            cur = stack.back();
        }
       
        // Popup the empty pointer.
        stack.pop_back();

        // Go to the next sibling.
        if (!stack.empty()) 
        {
            cur = stack.back();
            stack.pop_back();
            stack.push_back(cur->sibling);
        }
    }

    return false;
}

};


