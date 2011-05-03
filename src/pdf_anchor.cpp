/*
 * File Name: pdf_anchor.cpp
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

#include "pdf_anchor.h"

namespace pdf
{
#define SECTION_SEPERATOR         G_DIR_SEPARATOR
#define EVALUATE_SEPERATOR        ':'
#define TERMINATION_FLAG          '\0'
#define PDF_ANCHOR_HEADER         "pdf:"
#define PDF_PAGE_NUMER_PREFIX     "page"
#define PDF_WORD_PREFIX           "word"
#define PDF_CHAR_PREFIX           "char"
#define PDF_HYPERLINK_PREFIX      "link"
#define PDF_TOC_INDEX_PREFIX      "toc"
#define PDF_FILE_NAME_PREFIX      "file"

PDFAnchor::PDFAnchor()
{
    reset();
}

PDFAnchor::PDFAnchor(const string &anchor)
{
    reset();
    parse(anchor);
    anchor_str = anchor;
}

PDFAnchor::~PDFAnchor()
{
}

void PDFAnchor::reset()
{
    page_num = 0;
    word_num = -1;
    char_idx = -1;
    link_idx = -1;
    toc_idx  = -1;
    file_name.clear();
}

bool PDFAnchor::operator == (const PDFAnchor &right) const
{
    return ((this->page_num == right.page_num)
        &&(this->word_num == right.word_num)
        &&(this->char_idx == right.char_idx)
        &&(this->link_idx == right.link_idx)
        &&(this->toc_idx == right.toc_idx)
        &&(this->file_name == right.file_name));
}

bool PDFAnchor::operator < (const PDFAnchor &right) const
{
    if (this->page_num != right.page_num)
    {
        return (this->page_num < right.page_num);
    }

    if (this->word_num != right.word_num)
    {
        return (this->word_num < right.word_num);
    }

    if (this->char_idx != right.char_idx)
    {
        return (this->char_idx < right.char_idx);
    }

    if (this->link_idx != right.link_idx)
    {
        return (this->link_idx < right.link_idx);
    }

    if (this->toc_idx != right.toc_idx)
    {
        return (this->toc_idx < right.toc_idx);
    }

    return false;
}

bool PDFAnchor::operator > (const PDFAnchor &right) const
{
    if (this->page_num != right.page_num)
    {
        return (this->page_num > right.page_num);
    }
    
    if (this->word_num != right.word_num)
    {
        return (this->word_num > right.word_num);
    }

    if (this->char_idx != right.char_idx)
    {
        return (this->char_idx > right.char_idx);
    }

    if (this->link_idx != right.link_idx)
    {
        return (this->link_idx > right.link_idx);
    }

    if (this->toc_idx != right.toc_idx)
    {
        return (this->toc_idx > right.toc_idx);
    }

    return false;
}

const string& PDFAnchor::get_string()
{
    // TODO. check the validation of anchor string to avoid redundant packing.
    pack(anchor_str);
    return anchor_str;
}

int PDFAnchor::compare(const PDFAnchor &a1, const PDFAnchor &a2)
{
    //if (a1.file_name != a2.file_name)
    //{
    //    // return if they belog to different file
    //    return ANCHOR_COMPARE_ERROR;
    //}

    if (a1 == a2)
    {
        return 0;
    }

    if (a1 < a2)
    {
        return -1;
    }

    return 1;
}

bool PDFAnchor::parse(const string &anchor)
{
    size_t pos = anchor.find(PDF_FILE_NAME_PREFIX);
    std::string file_part;
    std::string front_part;
    if (pos != anchor.npos)
    {
        file_part  = anchor.substr(pos);
        front_part = anchor.substr(0, pos - 1);
    }
    else
    {
        front_part = anchor;
    }
    const char *pchar = front_part.c_str();
	WARNPRINTF("Parsing anchor: %s", anchor.c_str());
    bool ret = false;
    // parse the sections
    stringlist arglist;
    string     argument;
    while(!ret)
    {
        switch( *pchar )
        {
            case SECTION_SEPERATOR:
                {
                    arglist.push_back(argument);
                    argument.clear();
                    pchar++;
                }
                break;
            case TERMINATION_FLAG:
                {
                    arglist.push_back(argument);
                    ret = true;
                }
                break;
            default:
                {
                    argument.push_back(*pchar++);
                }
                break;
        }
    }

    for(size_t i = 1; i < arglist.size(); ++i)
    {
        if (get_prefix(arglist[i]) == PDF_PAGE_NUMER_PREFIX)
        {
            page_num = get_num(arglist[i]);
        }
        else if (get_prefix(arglist[i]) == PDF_WORD_PREFIX)
        {
            word_num = get_num(arglist[i]);
        }
        else if (get_prefix(arglist[i]) == PDF_CHAR_PREFIX)
        {
            char_idx = get_num(arglist[i]);
        }
        else if (get_prefix(arglist[i]) == PDF_HYPERLINK_PREFIX)
        {
            link_idx = get_num(arglist[i]);
        }
        else if (get_prefix(arglist[i]) == PDF_TOC_INDEX_PREFIX)
        {
            toc_idx = get_num(arglist[i]);
        }
    }

    if (!file_part.empty())
    {
        file_name = get_postfix(file_part);
    }
    return true;
}

bool PDFAnchor::pack(string &anchor)
{
    //if (page_num < 0)
    //{
    //    LOGPRINTF("Cannot pack the anchor with invalid page number!");
    //    return false;
    //}
    anchor = PDF_ANCHOR_HEADER;
    anchor += SECTION_SEPERATOR;
    anchor += PDF_PAGE_NUMER_PREFIX;
    anchor += EVALUATE_SEPERATOR;
    char buf[32];
    memset(buf, 0, 32);
    pdf_printf(buf, "%d", page_num);
    anchor += buf;
    
    if (word_num >= 0)
    {
        anchor += SECTION_SEPERATOR;
        anchor += PDF_WORD_PREFIX;
        anchor += EVALUATE_SEPERATOR;
        memset(buf, 0, 32);
        pdf_printf(buf, "%d", word_num);
        anchor += buf;
    }
    
    if (char_idx >= 0)
    {
        anchor += SECTION_SEPERATOR;
        anchor += PDF_CHAR_PREFIX;
        anchor += EVALUATE_SEPERATOR;
        memset(buf, 0, 32);
        pdf_printf(buf, "%d", char_idx);
        anchor += buf;
    }

    if (link_idx >= 0)
    {
        anchor += SECTION_SEPERATOR;
        anchor += PDF_HYPERLINK_PREFIX;
        anchor += EVALUATE_SEPERATOR;
        memset(buf, 0, 32);
        pdf_printf(buf, "%d", link_idx);
        anchor += buf;
    }

    if (toc_idx >= 0)
    {
        anchor += SECTION_SEPERATOR;
        anchor += PDF_TOC_INDEX_PREFIX;
        anchor += EVALUATE_SEPERATOR;
        memset(buf, 0, 32);
        pdf_printf(buf, "%d", toc_idx);
        anchor += buf;
    }

    // add the file name into anchor
    // this is not a mandatory option
    if (!file_name.empty())
    {
        anchor += SECTION_SEPERATOR;
        anchor += PDF_FILE_NAME_PREFIX;
        anchor += EVALUATE_SEPERATOR;
        anchor += file_name;
    }

    return true;
}

void PDFAnchor::set_end_anchor()
{
    string name = file_name;
    reset();
    file_name = name;
    page_num  = 0;
}

bool PDFAnchor::is_end_anchor() const
{
    return (page_num <= 0);
}

int PDFAnchor::get_num(const string &page_sub_str)
{
    size_t idx = page_sub_str.find(EVALUATE_SEPERATOR);

    if (idx != page_sub_str.npos)
    {
        string num_str = page_sub_str.substr(idx + 1);
        return atoi(num_str.c_str());
    }
        
    return 0;
}

const string PDFAnchor::get_prefix(const string &str)
{
    return str.substr(0, str.find(EVALUATE_SEPERATOR));
}

const string PDFAnchor::get_postfix(const string &str)
{
    return str.substr(str.find(EVALUATE_SEPERATOR) + 1);
}

void PDFAnchor::validate_for_getting_text(void)
{
    if (page_num)
    {
        if (word_num == -1)
        {
            word_num = 0;
        }

        if (char_idx == -1)
        {
            char_idx = 0;
        }
    }
}
}// namespace pdf

