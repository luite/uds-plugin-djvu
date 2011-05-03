/*
 * File Name: pdf_page.cpp
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

#include "utils.h"
#include "log.h"
#include "task.h"

#include "pdf_page.h"
#include "pdf_doc_controller.h"
#include "pdf_renderer.h"

namespace pdf
{

void init_render_area(RenderArea & area)
{
    area.x_offset = 0.0f;
    area.y_offset = 0.0f;
    area.width  = -1.0f;
    area.height = -1.0f;
}

bool is_render_area_valid(const RenderArea & area)
{
    return (area.width > 0.0f && area.height > 0.0f);
}

void get_content_area_in_pixel(const RenderArea & area,
                               const int origin_width,
                               const int origin_height,
                               PluginRectangle & rect)
{
    rect.x = static_cast<int>(origin_width * area.x_offset);
    rect.y = static_cast<int>(origin_height * area.y_offset);
    rect.width = static_cast<int>(origin_width * area.width);
    rect.height = static_cast<int>(origin_height * area.height);
}

void get_std_string_from_text_word(TextWord * word, std::string & result)
{
    /*UGooString u_str(*(word->getText()));

    int len = u_str.getLength() * sizeof(Unicode);

    char *res_buf = new char[len];
    if (ucs2utf8(reinterpret_cast<char*>(u_str.unicode()), len, res_buf, len))
    {
        result = std::string(res_buf);
    }*/
    GooString * text = word->getText();
    if (text != 0)
    {
        result = std::string(text->getCString());
        delete text;
    }
}

PDFPage::PDFPage(int page_num, const PDFRenderAttributes & attr)
{
    init();
    page_number = page_num;
    render_attr = attr;
}

PDFPage::~PDFPage(void)
{
    destroy();
}

void PDFPage::init()
{
    page_number = 0;
    bitmap = 0;
    links = 0;
    text = 0;
    doc_controller = 0;
    b_lock = false;
    render_status = RENDER_STOP;
    ref_id = PRERENDER_REF_ID;
    init_render_area(content_area);
}

size_t PDFPage::operator()()
{
    return static_cast<size_t>(page_number);
}

bool PDFPage::operator == (const PDFPage & right)
{
    return ((this->page_number == right.page_number) &&
            this->render_attr == right.render_attr);
}

bool PDFPage::operator == (const PDFRenderAttributes & right)
{
    return (this->render_attr == right);
}

void PDFPage::set_render_attr(const PDFRenderAttributes & attr)
{
    if (render_attr == attr)
    {
        return;
    }

    render_attr = attr;
}

void PDFPage::destroy_text()
{
    if (text) 
    {
        delete text;
        text = 0;
    }
}

unsigned int PDFPage::destroy_bitmap()
{
    unsigned int size = 0;
    if (bitmap)
    {
        size = length();
        delete bitmap;
        bitmap = 0;
    }
    return size;
}

void PDFPage::destroy_links()
{
    if (links) 
    {
        delete links;
        links = 0;
    }
}

unsigned int PDFPage::destroy()
{
    if (locked())
    {
        return 0;
    }

    if (get_render_status() == RENDER_RUNNING)
    {
        // if the page is in rendering, cannot delete it
        // it won't happen now, because the deleting is executed
        // in working thread.
        return 0;
    }

    // reset the render status
    set_render_status(RENDER_STOP);

    destroy_links();
    destroy_text();
    unsigned int size = destroy_bitmap();

    return size;
}

TextWordList* PDFPage::get_words_list()
{
    TextWordList *words = 0;

    if (text)
    {
        words = text->makeWordList(gFalse);
    }

    return words;
}

int PDFPage::get_bitmap_width()
{
    if (!bitmap)
    {
        return 0;
    }

    return bitmap->getWidth();
}

int PDFPage::get_bitmap_height()
{
    if (!bitmap)
    {
        return 0;
    }

    return bitmap->getHeight();
}

unsigned int PDFPage::length()
{
    if (!bitmap)
    {
        return 0;
    }
    return bitmap->getHeight() * bitmap->getRowSize();
}

unsigned int PDFPage::try_calc_length(const double zoom_value
                                      , const double crop_width
                                      , const double crop_height)
{
    int width = static_cast<int>(crop_width + 1.0f);
    int height = static_cast<int>(crop_height + 1.0f);
    double zoom = (zoom_value + 1.0f)/ 100.0f;
    int row_stride = ((width + 3)>> 2) << 2;

    return static_cast<unsigned int>(row_stride * height * zoom * zoom);
}

int PDFPage::get_bitmap_row_stride()
{
    if (!bitmap)
    {
        return 0;
    }

    return bitmap->getRowSize();
}

const unsigned char* PDFPage::get_bitmap_data()
{
    if (!bitmap)
    {
        return 0;
    }

    return bitmap->getDataPtr();
}

void PDFPage::update_bitmap(SplashBitmap *m) 
{
    if (bitmap == m)
    {
        return;
    }

    bitmap = m;
}

void PDFPage::update_links(Links *l) 
{
    if (links == l)
    {
        return;
    }

    links = l;
}

void PDFPage::update_text(TextPage *t) 
{
    if (text == t)
    {
        return;
    }
	if(text) delete text; // fixme added by luite, was this a memory leak?
    text = t;
}

SearchResult PDFPage::search(SearchContext &ctx
    , PDFSearchPage &results)
{
    if (!text)
    {
        return RES_ERROR;
    }

    TextWordList *words = get_words_list();
    PluginRangeImpl *result = 0;
    SearchResult ret = RES_NOT_FOUND;

    int count = 0;

    // the search operation would update the index of start word
    // in the context    
    if (ctx.forward)
    {
        if (ctx.word_cursor < 0)
        {
            // set the start word to be the first one
            ctx.word_cursor = 0;
        }

        // search forward
        result = search_string_forward(ctx, words);
        if (ctx.search_all)
        {    
            while(result != 0)
            {
                results.add(result);
                count++;
                result = search_string_forward(ctx, words);
            }
        }
        else
        {
            if (result)
            {
                results.add(result);
                count++;
            }
        }

    }
    else
    {
        if (ctx.word_cursor < 0)
        {
            // set the start word to be the last one
            ctx.word_cursor = words->getLength() - 1;
        }

        //search backward
        result = search_string_backward(ctx, words);
        if (ctx.search_all)
        {
            while(result != 0)
            {
                results.add(result);
                count++;
                result = search_string_backward(ctx, words);
            }
        }
        else
        {
            if (result)
            {
                results.add(result);
                count++;
            }
        }
    }

    if (count > 0)
    {
        ret = RES_OK;
    }

    delete words;
    return ret;
}

// Search the destination string forwardly, at the same time update the 
// word index
PluginRangeImpl* PDFPage::search_string_forward(SearchContext &ctx
    , TextWordList *words)
{
    if (ctx.dst_words.empty())
    {
        return 0;
    }

    // update the current index
    PluginRangeImpl *result = 0;

    int cur_word = ctx.word_cursor;
    int len      = words->getLength();
    if (cur_word >= len)
    {
        // there is no words left in current page
        return 0;
    }

    TextWord *word = words->get(cur_word);
    if (ctx.char_cursor >= word->getLength())
    {
        // move to the next word
        cur_word++;
        // reset char cursor
        ctx.char_cursor = 0;
    }

    SearchWords words_queue;

    if (ctx.dst_words.size() > 1)
    {
        // if need searching a words list, we should search by the status
        // status of the metching procedure
        MatchStatus stat = STATUS_HEADER;

        // index of the word in destination string
        int dst_word_index = 0;

        // set the cursor to record the first matched result
        // if the suceed words are not matched, roll back to this word
        int first_matched_word = cur_word;

        while (cur_word < len)
        {
            // match success, break out
            if (dst_word_index >= static_cast<int>(ctx.dst_words.size()))
            {
                break;
            }

            word = words->get(cur_word);

            if (word == 0)
            {
                // the word is NULL
                ERRORPRINTF("Null word in search");
                break;
            }

            //string word_str(word->getText()->getCString());
            string word_str;
            get_std_string_from_text_word(word, word_str);

            if (word_str.empty())
            {
                // the word is empty
                ERRORPRINTF("Empty word in search");
                break;
            }

            // record the index of search result
            int start_result_idx = 0;

            // set the search context
            bool forward = true;
            bool match_whole_word = ctx.match_whole_word;
            if (!match_whole_word)
            {
                if (stat == STATUS_BODY)
                {
                    // if it is "body" word in the words list
                    // we must compare by whole word
                    match_whole_word = true;
                }
                else if (stat == STATUS_HEADER)
                {
                    // if it is "header" word in the words list
                    // we must compare from the end char
                    forward = false;
                    // reset the char cursor
                    ctx.char_cursor = static_cast<int>(word_str.size()) - 1;
                }
            }

            if (compare_string(ctx.dst_words[dst_word_index]
                , word_str
                , ctx.case_sensitive
                , match_whole_word
                , forward
                , true
                , ctx.char_cursor
                , start_result_idx))
            {
                // update the matching status
                int next_word_index = dst_word_index + 1;

                switch (stat)
                {
                case STATUS_HEADER:                   
                    if (next_word_index < 
                        (static_cast<int>(ctx.dst_words.size()) - 1))
                    {
                        // next word is the body one
                        stat = STATUS_BODY;
                    }
                    else if (next_word_index == 
                        (static_cast<int>(ctx.dst_words.size()) - 1))
                    {
                        // next word is the last one
                        stat = STATUS_TAIL;
                    }

                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , word->getLength() - 1));

                    // set the index of first match word
                    first_matched_word = cur_word;

                    break;
                case STATUS_BODY:
                    if (next_word_index ==
                        (static_cast<int>(ctx.dst_words.size()) - 1))
                    {
                        // next word is the last one
                        stat = STATUS_TAIL;
                    }

                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , word->getLength() - 1));

                    break;
                case STATUS_TAIL:
                    //Search succeed
                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , start_result_idx
                        + static_cast<int>(ctx.dst_words[dst_word_index].size()) - 1));

                    break;
                default:
                    break;
                }

                // move index to the next word
                dst_word_index = next_word_index;
            }
            else
            {
                //match fails, clear the previous status and the results queue
                if (stat != STATUS_HEADER)
                {
                    dst_word_index = 0;
                    stat = STATUS_HEADER;
                    words_queue.clear();

                    // roll back to the first matched word
                    cur_word = first_matched_word;
                }
            }

            // next text word
            cur_word++;
            ctx.char_cursor = 0;
        }
    }
    else
    {
        // if only search a single word, we should take the situation
        // that pattern string appears repeatly in the source string
        while (cur_word < len)
        {

            word = words->get(cur_word);

            if (word == 0)
            {
                ERRORPRINTF("Null word in search");
                break;
            }

            //string word_str(word->getText()->getCString());
            string word_str;
            get_std_string_from_text_word(word, word_str);

            if (word_str.empty())
            {
                ERRORPRINTF("Empty word in search");
                break;
            }

            int start_result_idx = 0;
            if (compare_string(ctx.dst_words[0]
                , word_str
                , ctx.case_sensitive
                , ctx.match_whole_word
                , true
                , false
                , ctx.char_cursor
                , start_result_idx))
            {
                // this word meets the condition
                // push it into the results list
                words_queue.add(SearchWordRecord(cur_word
                    , start_result_idx
                    , start_result_idx + static_cast<int>(ctx.dst_words[0].size()) - 1));
                break;
            }

            // next text word
            cur_word++;
            ctx.char_cursor = 0;
        }

    }

    // generate the search result
    if (words_queue.get_count() == static_cast<int>(ctx.dst_words.size()))
    {
        generate_search_result(ctx, words_queue, result, true);
    }

    return result;
}

PluginRangeImpl* PDFPage::search_string_backward(SearchContext &ctx
    , TextWordList *words)
{
    int len = words->getLength();
    if (len <= 0)
    {
        return 0;
    }

    // update the current index
    PluginRangeImpl *result = 0;

    int cur_word = ctx.word_cursor;
    if (cur_word >= len)
    {
        cur_word = len - 1;
    }

    TextWord *word = 0;
    if (ctx.char_cursor < 0)
    {
        // move to the previous word
        cur_word--;
        if (cur_word >= 0)
        {
            word = words->get(cur_word);
            // reset the char cursor
            ctx.char_cursor = word->getLength() - 1;
        }
    }

    SearchWords words_queue;

    if (ctx.dst_words.size() > 1)
    {
        // status of the metching procedure
        // the initial status is "tail"
        MatchStatus stat = STATUS_TAIL;

        // index of the word in destination string
        int dst_words_end  = static_cast<int>(ctx.dst_words.size()) - 1;
        int dst_word_index = dst_words_end;

        // set the cursor to record the first matched result
        // if the suceed words are not matched, roll back to this word
        int first_matched_word = cur_word;

        while (cur_word >= 0)
        {
            // match success, break out
            if (dst_word_index < 0)
            {
                break;
            }

            word = words->get(cur_word);

            if (word == 0)
            {
                ERRORPRINTF("Null word in search");
                break;
            }

            //string word_str(word->getText()->getCString());
            string word_str;
            get_std_string_from_text_word(word, word_str);

            if (word_str.empty())
            {
                ERRORPRINTF("Empty word in search");
                break;
            }

            int start_result_idx = 0;
            bool forward = false;
            bool match_whole_word = ctx.match_whole_word;
            if (!match_whole_word)
            {
                if (stat == STATUS_BODY)
                {
                    // if it is "body" word in the words list
                    // we must compare by whole word
                    match_whole_word = true;
                }
                else if (stat == STATUS_TAIL)
                {
                    // if it is "header" word in the words list
                    // we must compare from the end char
                    forward = true;
                    ctx.char_cursor = 0;
                }
            }

            if (compare_string(ctx.dst_words[dst_word_index]
                , word_str
                , ctx.case_sensitive
                , match_whole_word
                , forward
                , true
                , ctx.char_cursor
                , start_result_idx))
            {
                // update the matching status
                int next_word_index = dst_word_index - 1;
                switch (stat)
                {
                case STATUS_TAIL:
                    if (next_word_index > 0)
                    {
                        // previous word is the body one
                        stat = STATUS_BODY;
                    }
                    else if (next_word_index == 0)
                    {
                        // previous word is the header one
                        stat = STATUS_HEADER;
                    }

                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , start_result_idx
                        + static_cast<int>(ctx.dst_words[dst_word_index].size()) - 1));

                    // set the first matched word
                    first_matched_word = cur_word;

                    break;
                case STATUS_BODY:
                    if (next_word_index == 0)
                    {
                        // previous word is the header one
                        stat = STATUS_HEADER;
                    }

                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , word->getLength() - 1));

                    break;
                case STATUS_HEADER:
                    // search succeed
                    // push the match word into the results list
                    words_queue.add(SearchWordRecord(cur_word
                        , start_result_idx
                        , word->getLength() - 1));

                    break;
                default:
                    break;
                }
                // move index to the previous word
                dst_word_index = next_word_index;
            }
            else
            {
                //match fails, clear the previous status and the results queue
                if (stat != STATUS_TAIL)
                {
                    dst_word_index = dst_words_end;
                    stat = STATUS_TAIL;
                    words_queue.clear();

                    // roll back to the first matched word
                    cur_word = first_matched_word;
                }
            }

            // next text word
            cur_word--;
            if (cur_word >= 0)
            {
                ctx.char_cursor = words->get(cur_word)->getLength() - 1;
            }
        }
    }
    else
    {
        while (cur_word >= 0)
        {
            // if only search a single word, we can simply compare it
            word = words->get(cur_word);

            if (word == 0)
            {
                ERRORPRINTF("Null word in search");
                break;
            }

            //string word_str(word->getText()->getCString());
            string word_str;
            get_std_string_from_text_word(word, word_str);

            if (word_str.empty())
            {
                ERRORPRINTF("Empty word in search");
                break;
            }

            int start_result_idx = 0;
            if (compare_string(ctx.dst_words[0]
                , word_str
                , ctx.case_sensitive
                , ctx.match_whole_word
                , false
                , false
                , ctx.char_cursor
                , start_result_idx))
            {
                // this word meets the condition
                // push it into the results list
                words_queue.add(SearchWordRecord(cur_word
                    , start_result_idx
                    , start_result_idx + static_cast<int>(ctx.dst_words[0].size()) - 1));
                break;
            }
            // next text word
            cur_word--;
            if (cur_word >= 0)
            {
                ctx.char_cursor = words->get(cur_word)->getLength() - 1;
            }
        }
    }

    // generate the search result
    if (words_queue.get_count() == static_cast<int>(ctx.dst_words.size()))
    {
        generate_search_result(ctx, words_queue, result, false);
    }

    return result;
}

void PDFPage::generate_search_result(SearchContext &ctx
    , SearchWords &queue
    , PluginRangeImpl* &result
    , bool forward)
{
    if (queue.get_count() == 0)
    {
        return;
    }

    result = new PluginRangeImpl;
    SearchWordRecord begin;
    SearchWordRecord end;
    if (forward)
    {
        begin = queue.front();
        end   = queue.back();
    }
    else
    {
        begin = queue.back();
        end   = queue.front();
    }

    int idx_start  = begin.start_char_index;
    int idx_end    = end.end_char_index;
    int word_start = begin.word_index;
    int word_end   = end.word_index;

    if (forward)
    {
        // set the current search position to the last word
        ctx.word_cursor = word_end;
        ctx.char_cursor = idx_end;
    }
    else
    {
        // set the current search position to the first word
        ctx.word_cursor = word_start;
        ctx.char_cursor = idx_start;
    }

    PDFAnchor param;
    param.page_num = page_number;
    param.word_num = word_start;
    param.char_idx = idx_start;
    //param.file_name = get_doc_controller()->name();
    result->start_anchor = new StringImpl(param.get_string());

    param.char_idx = idx_end;
    param.word_num = word_end;
    result->end_anchor = new StringImpl(param.get_string());
}

bool PDFPage::compare_string(const string &dst
    , const string &src
    , bool case_sensitive
    , bool match_whole_word
    , bool forward
    , bool sub_string
    , const int start_char_idx
    , int &start_result_idx)
{
    string dst_str = dst;
    string src_str = src;

    if (!case_sensitive)
    {
        //Transform all of the charactor into upper case
        std::transform(dst_str.begin(), dst_str.end(), dst_str.begin(), (int(*)(int))tolower);

        std::transform(src_str.begin(), src_str.end(), src_str.begin(), (int(*)(int))tolower);
    }

    if (match_whole_word)
    {
        // return false if size dismatch
        if (dst_str.size() != src_str.size())
        {
            return false;
        }

        return dst_str == src_str;
    }

    if (forward)
    {
        // test the length
        if (src.size() - start_char_idx < dst_str.size())
        {
            return false;
        }

        if (sub_string)
        {
            // retrieve the a sub string from the start character index
            // then compare with the destination string
            src_str = src_str.substr(start_char_idx, dst_str.size());

            if (src_str == dst_str)
            {
                // if succeed, return the start index
                start_result_idx = start_char_idx;
            }
            else
            {
                // otherwise return error position
                start_result_idx = static_cast<int>(src_str.npos);
            }
        }
        else
        {
            start_result_idx = static_cast<int>(src_str.find(dst_str
                , static_cast<size_t>(start_char_idx)));
        }
    }
    else
    {
        // test the length
        if (start_char_idx - static_cast<int>(dst_str.size()) + 1 < 0)
        {
            return false;
        }

        if (sub_string)
        {
            // set the start position of retrieving
            int pos = start_char_idx - static_cast<int>(dst_str.size()) + 1;

            if (start_char_idx >= static_cast<int>(src_str.size()))
            {
                return false;
            }

            // retrieve the sub string and stop at the char (start + 1)
            src_str = src_str.substr(pos, start_char_idx + 1);

            if (src_str == dst_str)
            {
                // if succeed, return the start position
                start_result_idx = pos;
            }
            else
            {
                // otherwise, return the error position
                start_result_idx = static_cast<int>(src_str.npos);
            }
        }
        else
        {
            start_result_idx = static_cast<int>(src_str.rfind(dst_str
                , static_cast<size_t>(start_char_idx)));
        }
    }

    if (static_cast<size_t>(start_result_idx) != src_str.npos)
    {
        return  true;
    }

    return false;
}

bool merge_rectangle(const double x_min, const double y_min, 
    const double x_max, const double y_max, 
    PDFRectangle *rect)
{
    if (!rect->isValid())
    {
        // initialize the rectangle
        rect->x1 = x_min;
        rect->x2 = x_max;
        rect->y1 = y_min;
        rect->y2 = y_max;
        return true;
    }

    if (fabs(y_min - rect->y1) < ZERO_RANGE
        && fabs(y_max - rect->y2) < ZERO_RANGE)
    {
        rect->x1 = min(x_min, rect->x1);
        rect->x2 = max(x_max, rect->x2);
        return true;
    }

    return false;
}

bool PDFPage::get_bounding_rectangles(const string &start_anchor
    , const string &end_anchor
    , PDFRectangles &rects)
{
    PDFAnchor start_param(start_anchor);
    PDFAnchor end_param(end_anchor);

    //Cannot identify the word in seperated pages
    if (start_param.page_num != page_number ||
        start_param.page_num != end_param.page_num)
    {
        return false;
    }

    //int len = end_param.char_idx - start_param.char_idx + 1;
    //assert(len > 0);
    //assert(start_param.word_num >= 0 && end_param.word_num >= 0);

    // TODO. Get rectange from hyperlink (without word)
    PDFRectangle pdf_rect;
    if (start_param.word_num >= 0 && end_param.word_num >= 0)
    {
        TextWordList * words = get_words_list();
        for(int i = start_param.word_num; i <= end_param.word_num; ++i)
        {
            double x_min = 0.0, y_min = 0.0, x_max = 0.0, y_max = 0.0;
            words->get(i)->getBBox(&x_min, &y_min, &x_max, &y_max);

            if (!merge_rectangle(x_min, y_min, x_max, y_max, &pdf_rect))
            {
                PluginRectangle rect;
                rect.x = static_cast<int>(pdf_rect.x1);
                rect.y = static_cast<int>(pdf_rect.y1);
                rect.width  = static_cast<int>(pdf_rect.x2 - pdf_rect.x1) + 1;
                rect.height = static_cast<int>(pdf_rect.y2 - pdf_rect.y1) + 1;
                rects.add(rect);

                // update the rectangle
                pdf_rect.x1 = x_min;
                pdf_rect.x2 = x_max;
                pdf_rect.y1 = y_min;
                pdf_rect.y2 = y_max;
            }
        }

        delete words;

        if (pdf_rect.isValid())
        {
            // add the last rectangle into the list
            PluginRectangle rect;
            rect.x = static_cast<int>(pdf_rect.x1);
            rect.y = static_cast<int>(pdf_rect.y1);
            rect.width  = static_cast<int>(pdf_rect.x2 - pdf_rect.x1) + 1;
            rect.height = static_cast<int>(pdf_rect.y2 - pdf_rect.y1) + 1;
            rects.add(rect);
        }
    }
    else if (start_param.link_idx >= 0 && end_param.link_idx >= 0)
    {
        if (links == 0)
        {
            return false;
        }

        for(int i = start_param.link_idx; i <= end_param.link_idx; ++i)
        {
            double x_min = 0.0, y_min = 0.0, x_max = 0.0, y_max = 0.0;
            Link *link = links->getLink(i);
            link->getRect(&x_min, &y_min, &x_max, &y_max);
            int real_x_min, real_y_min, real_x_max, real_y_max;
            coordinates_user_to_dev(x_min, y_min, &real_x_min, &real_y_min);
            coordinates_user_to_dev(x_max, y_max, &real_x_max, &real_y_max);
            x_min = min(real_x_min, real_x_max);
            x_max = max(real_x_min, real_x_max);
            y_min = min(real_y_min, real_y_max);
            y_max = max(real_y_min, real_y_max);

            if (!merge_rectangle(x_min, y_min, x_max, y_max, &pdf_rect))
            {
                PluginRectangle rect;
                rect.x = static_cast<int>(pdf_rect.x1);
                rect.y = static_cast<int>(pdf_rect.y1);
                rect.width  = static_cast<int>(pdf_rect.x2 - pdf_rect.x1) + 1;
                rect.height = static_cast<int>(pdf_rect.y2 - pdf_rect.y1) + 1;
                rects.add(rect);

                // update the rectangle
                pdf_rect.x1 = x_min;
                pdf_rect.x2 = x_max;
                pdf_rect.y1 = y_min;
                pdf_rect.y2 = y_max;
            }

        }

        if (pdf_rect.isValid())
        {
            // add the last rectangle into the list
            PluginRectangle rect;
            rect.x = static_cast<int>(pdf_rect.x1);
            rect.y = static_cast<int>(pdf_rect.y1);
            rect.width  = static_cast<int>(pdf_rect.x2 - pdf_rect.x1) + 1;
            rect.height = static_cast<int>(pdf_rect.y2 - pdf_rect.y1) + 1;
            rects.add(rect);
        }
    }

    return true;
}

bool PDFPage::render_splash_map(PDFRenderer *renderer, void *abort_data)
{
    if (locked())
    {
        if (get_render_status() == RENDER_STOP)
        {
            // the previous rendering stopped by some reason
            // reset the lock
            unlock();
        }
        else
        {
            // cannot render when the page is locked
            LOGPRINTF("Locked, Cannot render\n");
            return false;
        }
    }

    // destroy the pre-rendered results
    destroy_links();
    doc_controller->update_memory_usage((-1) * destroy_bitmap());

    // set the status to rendering
    set_render_status(RENDER_RUNNING);

    // second render the page
    RenderRet ret = Render_Error;
    SplashBitmap *b = 0;
    Links *l = 0;

    // lock when rendering
    ScopeMutex m(&(renderer->get_render_mutex()));

    ret = doc_controller->get_pdf_doc()->displayPage(
        renderer->get_splash_output_dev()
        , page_number
        , render_attr.get_real_zoom_value() * 0.01 * renderer->get_view_attr().get_device_dpi_h()
        , render_attr.get_real_zoom_value() * 0.01 * renderer->get_view_attr().get_device_dpi_v()
        , render_attr.get_rotate()
        , gFalse //useMediaBox, TODO.
        , gTrue  //crop, TODO.
        , gTrue  //doLinks, TODO.
        , abort_render_check
        , abort_data
    );

    if (ret == Render_Error || ret == Render_Invalid)
    {
        LOGPRINTF("1. Error in rendering page:%d\n", get_page_num());
        return false;
    }

    // take bitmap
    b = renderer->get_splash_output_dev()->takeBitmap();

    // take hyperlinks
#ifdef WIN32
    l = doc_controller->get_pdf_doc()->takeLinks();
#else
    l = doc_controller->get_pdf_doc()->getLinks(page_number);
#endif

    if (ret == Render_Done)
    {
        update_bitmap(b);
        update_links(l);

        // retrieve ctm and ictm
        memcpy(ctm, renderer->get_splash_output_dev()->getDefCTM(), 6 * sizeof(double));
        memcpy(ictm, renderer->get_splash_output_dev()->getDefICTM(), 6 * sizeof(double)); 

        doc_controller->update_memory_usage(length());
        LOGPRINTF("Rendering of page:%d Done! Length:%d\n", get_page_num(), length());
        return true;
    }
    else if (ret == Render_Abort)
    {
        // MUST remove the temporary render results
        delete b;
        delete l;
        set_render_status(RENDER_STOP);
        LOGPRINTF("Rendering of page:%d is aborted! Task:%p\n", get_page_num(), abort_data);
        return false;
    }

    LOGPRINTF("2. Error in rendering page:%d\n", get_page_num());
    return false;
}

void PDFPage::set_render_status(RenderStatus s)
{
    render_status = s;
}

bool PDFPage::render_text(PDFRenderer *renderer, bool use_defalt_setting)
{
    static const double DEFAULT_ZOOM = 0.2f;

    destroy_text();
    // currently, the text rendering cannot be aborted

    // lock when rendering
    ScopeMutex m(&(renderer->get_render_mutex()));

    doc_controller->get_pdf_doc()->displayPage(
        renderer->get_text_output_dev()
        , page_number
        , (use_defalt_setting ? DEFAULT_ZOOM : render_attr.get_real_zoom_value() * 0.01) *
          renderer->get_view_attr().get_device_dpi_h()
        , (use_defalt_setting ? DEFAULT_ZOOM : render_attr.get_real_zoom_value() * 0.01) *
          renderer->get_view_attr().get_device_dpi_v()
        , render_attr.get_rotate()
        , gFalse
        , gTrue
        , gFalse
        );

    update_text(renderer->get_text_output_dev()->takeText());

    return true;
}

bool PDFPage::get_content_area(PDFRenderer *renderer, RenderArea &area)
{
    static const double SHRINK_ZOOM = 0.2f;
    static const int    EXPAND_STEP = 2;

    if (!is_render_area_valid(content_area))
    {
        // lock when rendering
        ScopeMutex m(&(renderer->get_render_mutex()));

        RenderRet ret = doc_controller->get_pdf_doc()->displayPage(
        renderer->get_thumbnail_output_dev()
        , get_page_num()
        , SHRINK_ZOOM * renderer->get_view_attr().get_device_dpi_h()
        , SHRINK_ZOOM * renderer->get_view_attr().get_device_dpi_v()
        , 0
        , gFalse  //useMediaBox, TODO.
        , gFalse  //crop, TODO.
        , gFalse  //doLinks, TODO.
        );

        if (ret == Render_Error || ret == Render_Invalid)
        {
            ERRORPRINTF("Error in rendering thumbnail page:%d\n", get_page_num());
            return false;
        }

        SplashBitmap *thumb_map = renderer->get_thumbnail_output_dev()->takeBitmap();
        PDFRectangle content_rect;
        bool succeed = get_content_from_bitmap(thumb_map, content_rect);
        // calculate the render area by the rectangle
        double page_width = thumb_map->getWidth();
        double page_height = thumb_map->getHeight();
        delete thumb_map;
        if (!succeed)
        {
            // set the content area to be the page area
            content_rect.x1 = content_rect.y1 = 0;
            content_rect.x2 = page_width;
            content_rect.y2 = page_height;
        }
        else
        {
            // expand the content area to avoid content covering
            double inc_x2 = 0;
            double inc_y2 = 0;

            // expand x1
            if (content_rect.x1 > EXPAND_STEP)
            {
                content_rect.x1 -= EXPAND_STEP;
                inc_x2 = EXPAND_STEP;
            }
            else
            {
                inc_x2 = content_rect.x1;
                content_rect.x1 = 0;
            }

            // expand y1
            if (content_rect.y1 > EXPAND_STEP)
            {
                content_rect.y1 -= EXPAND_STEP;
                inc_y2 = EXPAND_STEP;
            }
            else
            {
                inc_y2 = content_rect.y1;
                content_rect.y1 = 0;
            }

            // expand x2
            content_rect.x2 += (inc_x2 + 1);
            if (content_rect.x2 > page_width)
            {
                content_rect.x2 = page_width;
            }

            // expand y2
            content_rect.y2 += (inc_y2 + 1);
            if (content_rect.y2 > page_height)
            {
                content_rect.y2 = page_height;
            }
        }

        content_area.x_offset = static_cast<float>(content_rect.x1 / page_width);
        content_area.y_offset = static_cast<float>(content_rect.y1 / page_height);

        content_area.width =
            static_cast<float>((content_rect.x2 - content_rect.x1) / page_width);
        content_area.height =
            static_cast<float>((content_rect.y2 - content_rect.y1) / page_height);

        if (content_area.width > 1.0f)
        {
            content_area.x_offset = 0.0f;
            content_area.width    = 1.0f;
        }
        if (content_area.height > 1.0f)
        {
            content_area.y_offset = 0.0f;
            content_area.height   = 1.0f;
        }
    }

    area = content_area;

    return true;
}

bool PDFPage::get_content_from_bitmap(SplashBitmap *bitmap, PDFRectangle &rect)
{
    static const int BACKGROUND_COLOR = 255;
    static const int LINE_STEP        = 1;
    static const int SHRINK_STEP      = 1;
    static const double SHRINK_RANGE  = 0.3f;

    // top left
    int x1 = 0;
    int y1 = 0;
    // bottom right
    int x2 = bitmap->getWidth();
    int y2 = bitmap->getHeight();

    int left_edge = static_cast<int>(SHRINK_RANGE * x2);
    int right_edge = static_cast<int>((1.0f - SHRINK_RANGE) * x2);
    int top_edge = static_cast<int>(SHRINK_RANGE * y2);
    int bottom_edge = static_cast<int>((1.0f - SHRINK_RANGE) * y2);

    // current pixel
    Guchar cur_pix;
    bool stop[4] = {false, false, false, false};

    while (!stop[0] || !stop[1] || !stop[2] || !stop[3])
    {
        // check top line
        int x_cur = x1;
        while (x_cur < x2 && !stop[0])
        {
            bitmap->getPixel(x_cur, y1, &cur_pix);
            if (cur_pix != BACKGROUND_COLOR)
            {
                stop[0] = true;
                break;
            }
            x_cur += LINE_STEP;
        }

        // check bottom line
        x_cur = x1;
        while (x_cur < x2 && !stop[1])
        {
            bitmap->getPixel(x_cur, y2, &cur_pix);
            if (cur_pix != BACKGROUND_COLOR)
            {
                stop[1] = true;
                break;
            }
            x_cur += LINE_STEP;
        }

        // check left line
        int y_cur = y1;
        while (y_cur < y2 && !stop[2])
        {
            bitmap->getPixel(x1, y_cur, &cur_pix);
            if (cur_pix != BACKGROUND_COLOR)
            {
                stop[2] = true;
                break;
            }
            y_cur += LINE_STEP;
        }

        // check right line
        y_cur = y1;
        while (y_cur < y2 && !stop[3])
        {
            bitmap->getPixel(x2, y_cur, &cur_pix);
            if (cur_pix != BACKGROUND_COLOR)
            {
                stop[3] = true;
                break;
            }
            y_cur += LINE_STEP;
        }

        // shrink the rectangle
        if (!stop[2])
        {
            if (x1 >= left_edge)
            {
                stop[2] = true;
            }
            else
            {
                x1 += SHRINK_STEP;
            }
        }

        if (!stop[3])
        {
            if (x2 <= right_edge)
            {
                stop[3] = true;
            }
            else
            {
                x2 -= SHRINK_STEP;
            }
        }

        if (!stop[0])
        {
            if (y1 >= top_edge)
            {
                stop[0] = true;
            }
            else
            {
                y1 += SHRINK_STEP;
            }
        }

        if (!stop[1])
        {
            if (y2 <= bottom_edge)
            {
                stop[1] = true;
            }
            else
            {
                y2 -= SHRINK_STEP;
            }
        }

    }

    if (stop[0] && stop[1] && stop[2] && stop[3])
    {
        rect.x1 = x1;
        rect.x2 = x2;
        rect.y1 = y1;
        rect.y2 = y2;
        return true;
    }

    return false;
}

GBool PDFPage::abort_render_check(void *data)
{
    Task *task = static_cast<Task*>(data);

    return static_cast<GBool>(task->is_aborted());
}

// (x,y) -> "pdf:/page:8/link:0/word:12/char:06"
void PDFPage::get_anchor_param_from_coordinates(double x, double y
    , PDFAnchor &param)
{
    int i;
    // Caculate whether (x, y) inside a Link and inside which Link
    int link_index = -1;
    if (links && links->onLink(x, y))
    {
        int link_num;
        Link * link;

        link_num = links->getNumLinks();
        for (i = 0; i < link_num; i++)
        {
            link = links->getLink(i);
            if (link && link->inRect(x, y))
            {
                link_index = i;
                break;
            }
        }
    }

    // get the anchor of a screen point
    // now the anchor is supposed to be like "pdf:/page:8/link:0/word:12/char:06"
    // however, if the point is located on a object(image, shape or any thing else),
    // word and char cannot be retrieved.
    // TODO. add support to the non-text object
    int dx, dy;
    coordinates_user_to_dev(x, y, &dx, &dy);

    int word_index = -1, char_index = -1;

    TextWordList * words = get_words_list();
    if (words != 0)
    {
        int words_num = words->getLength();
        TextWord * word = 0;
        double x_min = 0.0, y_min = 0.0, x_max = 0.0, y_max = 0.0;
        for(i = 0; i < words_num; i++)
        {
            word = words->get(i);
            word->getBBox(&x_min, &y_min, &x_max, &y_max);

            if ((x_min <= dx) && (dx <= x_max) 
                && (y_min <= dy) && (dy <= y_max))
            {
                word_index = i;

                int chars_num = word->getLength();
                for (int j = 0; j < chars_num; j++)
                {
#ifdef WIN32
                    x_min = word->getEdge(j);
                    x_max = word->getEdge(j+1);
#else
                    word->getCharBBox(j, &x_min, &y_min, &x_max, &y_max); 
#endif
                    if ((x_min <= dx) && (dx <= x_max) 
                        && (y_min <= dy) && (dy <= y_max))
                    {
                        char_index = j;
                        break;
                    }
                }
                break;
            }
        }

        delete words;
    }

    // set the anchor
    param.page_num = page_number;
    param.link_idx = link_index;
    param.word_num = word_index;
    param.char_idx = char_index;
    //param.file_name = get_doc_controller()->name();
}

bool PDFPage::get_range_param_by_word_index(const int word_index,
                                            PDFAnchor & start_param,
                                            PDFAnchor & end_param)
{
    TextWordList * words = get_words_list();
    bool ret = false;
    if (words != 0)
    {
        int words_num = words->getLength();
        if (word_index >= 0 && word_index < words_num)
        {
            TextWord * word = words->get(word_index);

            start_param.page_num = page_number;
            start_param.word_num = word_index;
            start_param.char_idx = 0;
            //start_param.file_name = get_doc_controller()->name();

            end_param.page_num = page_number;
            end_param.word_num = word_index;
            end_param.char_idx = word->getLength();
            //end_param.file_name = get_doc_controller()->name();

            ret = true;
        }
        delete words;
    }
    return ret;
}

bool PDFPage::get_text_by_range(const PDFAnchor & start_param,
                                const PDFAnchor & end_param,
                                std::string &result)
{
    // TODO. It only supports retrieving single word now.
    // We need implement the function that can retrieve text with multiple words
    if (start_param.page_num != end_param.page_num ||
        start_param.word_num != end_param.word_num)
    {
        // if page number or word number is not equal, return false
        return false;
    }

    TextWordList * words = get_words_list();
    bool ret = false;
    if (words != 0)
    {
        int words_num = words->getLength();
        int word_index = start_param.word_num;
        if (word_index >= 0 && word_index < words_num)
        {
            TextWord * word = words->get(word_index);
            get_std_string_from_text_word(word, result);
            ret = true;
        }

        delete words;
    }

    return ret;
}

bool PDFPage::get_range_param_by_link_index(const int link_index,
    PDFAnchor & start_param,
    PDFAnchor & end_param)
{
    if (links)
    {
        //assert(link_index >= 0 && link_index < links->getNumLinks());
        if (link_index < 0 || link_index >= links->getNumLinks())
        {
            ERRORPRINTF("Error Link Index");
            return false;
        }

        // Link * link = links->getLink(link_index);

        // Disable the area check
        // Do we really need the area check for hyperlinks?
        // In most of the PDF documents, hyperlinks are indicated by a rectangle.
        // We can just return the index of hyperlink

        /*double x_min = 0.0f, y_min = 0.0f, x_max = 0.0f, y_max = 0.0f, y = 0.0f;
        link->getRect(&x_min, &y_min, &x_max, &y_max);

        // calculate the middle point
        y = (y_min + y_max) / 2;

        get_anchor_param_from_coordinates(x_min, y, start_param);
        get_anchor_param_from_coordinates(x_max, y, end_param);*/

        start_param.page_num = page_number;
        start_param.link_idx = link_index;
        //start_param.file_name = get_doc_controller()->name();

        end_param.page_num = page_number;
        end_param.link_idx = link_index;
        //end_param.file_name = get_doc_controller()->name();
        return true;
    }

    return false;
}

int PDFPage::get_goto_page_of_link(int link_index)
{
    if (links == 0)
    {
        return 0;
    }

    //assert(link_index >= 0 && link_index < links->getNumLinks());
    if (link_index < 0 || link_index >= links->getNumLinks())
    {
        ERRORPRINTF("Error Link Index");
        return 0;
    }

    Link *link = links->getLink(link_index);
    int page_num = 0;
    if (link != 0)
    {
        LinkAction *action = link->getAction();
        LinkActionKind kind = action->getKind();
        if (kind == actionGoTo)
        {
            // Caculate the page number of destination of this link.
#ifdef WIN32
            UGooString *named_dest = 0;
#else
            GooString *named_dest = 0;
#endif
            PDFDoc *doc = doc_controller->get_pdf_doc();

            LinkDest *dest = ((LinkGoTo *)action)->getDest();
            named_dest = ((LinkGoTo *)action)->getNamedDest();

            if (dest && dest->isPageRef())
            {
                Ref pageRef = dest->getPageRef();
                page_num = doc->findPage(pageRef.num, pageRef.gen);
            }
            else if (named_dest)
            {
                dest = doc->findDest(named_dest);
                if (dest)
                {
                    Ref pageRef = dest->getPageRef();
                    page_num = doc->findPage(pageRef.num, pageRef.gen);
                }
            }
        }
    }
    return page_num;
}

bool PDFPage::is_hyper_linked_page(int dst_page_num)
{
    if (links == 0)
    {
        return false;
    }

    int link_num = links->getNumLinks();
    if (link_num <= 0)
    {
        return false;
    }

    for (int i = 0; i < link_num; ++i)
    {
        if (dst_page_num == get_goto_page_of_link(i))
        {
            return true;
        }
    }

    return false;
}

// 4 -> "pdf:/page:8" 
bool PDFPage::get_goto_anchor_of_link(int link_index, std::string & anchor)
{
    // Get the anchor of destination for this link.
    int page_num = get_goto_page_of_link(link_index);
    if (page_num > 0)
    {
        PDFAnchor param;
        param.page_num = page_num;
        //param.file_name = get_doc_controller()->name();
        anchor = param.get_string();
        return true;
    }

    return false;
}

void PDFPage::coordinates_dev_to_user(const double dx, const double dy, 
    double * ux, double *uy)
{
  *ux = ictm[0] * dx + ictm[2] * dy + ictm[4];
  *uy = ictm[1] * dx + ictm[3] * dy + ictm[5];
}

void PDFPage::coordinates_user_to_dev(const double ux, const double uy, 
    int * dx, int *dy)
{
  *dx = (int)(ctm[0] * ux + ctm[2] * uy + ctm[4] + 0.5);
  *dy = (int)(ctm[1] * ux + ctm[3] * uy + ctm[5] + 0.5);
}

}//namespace pdf


