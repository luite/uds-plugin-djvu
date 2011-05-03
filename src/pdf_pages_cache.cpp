/*
 * File Name: pdf_pages_cache.cpp
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

#include "pdf_pages_cache.h"
#include "pdf_doc_controller.h"

namespace pdf
{

PagesCache::PagesCache(void)
: size_limit(0)
, total_length(0)
, pages()
, cache_mutex()
{
}

PagesCache::~PagesCache(void)
{
    clear();
}

bool PagesCache::reset(const unsigned int size)
{
    ScopeMutex m(&cache_mutex);

    int real_size = static_cast<int>(size);

    if (size_limit > static_cast<unsigned int>(real_size))
    {
        //remove the redundant pages
        while (total_length > real_size)
        {
            if (!remove_page())
            {
                // cannot remove the needed page
                return false;
            }
        }
    }

    size_limit = static_cast<unsigned int>(real_size);
    return true;
}

unsigned int PagesCache::size()
{
    ScopeMutex m(&cache_mutex);
    return size_limit;
}

void PagesCache::clear()
{
    ScopeMutex m(&cache_mutex);

    PagesIter begin = pages.begin();
    PagesIter end = pages.end();
    PagesIter iter = begin;
    for(; iter != end; ++iter)
    {
        delete iter->second;
    }
    pages.clear();
    total_length = 0;
}

/// Add a new page
void PagesCache::add_page(PagePtr p)
{
    ScopeMutex m(&cache_mutex);
    // insert the new page into cache
    pages[(*p)()] = p;
}

/// Remove the old pages to make sure the memory is enough
/// NOTE: length might be less than 0
bool PagesCache::make_enough_memory(const int page_num, const int length)
{
    ScopeMutex m(&cache_mutex);

    int sum = total_length + length;
    if (sum <= static_cast<int>(size_limit))
    {
        return true;
    }

    // remove the most useless pages until the sum is less than
    // a quarter of the size limitation
    unsigned int size = (size_limit >> 1);
    while (sum > static_cast<int>(size))
    {
        if (!remove_page(page_num))
        {
            // remove fails, because:
            // 1. there is no any cached images any more
            // 2. the image is locked
            if ((total_length + length) <= static_cast<int>(size_limit))
            {
                // if the total length plus with the length is less
                // than the size limitation, it means there is enough
                // space left, otherwise return false.
                return true;
            }
            TRACE("Skip Page:%d\n\n", page_num);
            return false;
        }
        sum = total_length + length;
    }

    return true;
}

void PagesCache::clear_cached_bitmaps()
{
    ScopeMutex m(&cache_mutex);
    // clear all cached bitmaps
    LOGPRINTF("Clear cached bitmaps due to out of memory\n\n");
    PagePtr page = 0;
    PagesIter iter = pages.begin();
    for (; iter != pages.end(); ++iter)
    {
        page = iter->second;
        if (page->get_bitmap() && !page->locked())
        {
            int delta = static_cast<int>(page->destroy());

            // update the total length
            total_length -= delta;
        }
    }
}

PagePtr PagesCache::get_page(const size_t idx)
{
    ScopeMutex m(&cache_mutex);

    PagePtr page = 0;
    PagesIter iter = pages.find(idx);
    if (iter != pages.end())
    {
        page = iter->second;
    }
    return page;
}

void PagesCache::update_mem_usage(const int length)
{
    ScopeMutex m(&cache_mutex);
    total_length += length;
    /*LOGPRINTF("Add memory usage:%d, total length:%d\n"
            , length
            , total_length);*/
}

bool PagesCache::remove_page(const int page_num)
{
    // remove the out-of-date page based on the remove strategy
    PagesIter begin = pages.begin();
    PagesIter end = pages.end();
    PagesIter iter = begin;
    PagesIter remove_iter = iter;

    while(iter != end)
    {
        // get the page with lowest priority
        if (*(iter->second) < *(remove_iter->second))
        {
            remove_iter = iter;
        }
        iter++;
    }

    if (remove_iter->second->get_bitmap() && !remove_iter->second->locked())
    {
        if (page_num >= 0 &&
            (remove_iter->second == pages[page_num] ||
             compare_priority(remove_iter->second->get_page_num(),
                              page_num,
                              remove_iter->second->get_doc_controller()->get_prerender_policy())
                              >= 0))
        {
            // if the request page is the lowest or all of the cached page has
            // higher priority, MUST not remove anything
            return false;
        }

        // delete the valid page(length > 0 and NOT locked)
        // sub the total length
        int delta = static_cast<int>(remove_iter->second->destroy());

        // update the total length
        total_length -= delta;

        TRACE("Remove Cached Page:%d, Total Length:%d, Delta Length:%d\n\n"
            , remove_iter->second->get_page_num()
            , total_length
            , delta);

        if (total_length < 0)
        {
            // this is an exception 
            total_length = 0;
        }
    }
    else if (!remove_iter->second->get_bitmap())
    {
        WARNPRINTF("No bitmap now");
        return false;
    }
    else
    {
        WARNPRINTF("Cannot remove the locked page %d"
            , remove_iter->second->get_page_num());
        return false;
    }

    return true;
}

}

