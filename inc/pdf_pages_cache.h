/*
 * File Name: pdf_pages_cache.h
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

#ifndef PDF_PAGES_CACHE_H_
#define PDF_PAGES_CACHE_H_

#include "log.h"
#include "mutex.h"

#include "pdf_define.h"
#include "pdf_page.h"

namespace pdf
{

/// PagesCache caches the used PDFPage instances
class PDFPage;
class PagesCache
{
public:
    PagesCache(void);
    ~PagesCache(void);

    /// Reset the size of pages cache
    bool reset(const unsigned int size);

    /// Get the size of pages cache
    unsigned int size();

    /// Clear the pages cache
    void clear();

    /// Add a new page
    void add_page(PagePtr p);

    /// Remove the old pages to make sure the memory is enough
    /// NOTE: length might be less than 0
    bool make_enough_memory(const int page_num, const int length);

    /// Clear the cached bitmaps but locked page
    void clear_cached_bitmaps();

    /// Get a page
    PagePtr get_page(const size_t idx);

    /// Increase total length by adding the page length
    void update_mem_usage(const int length);

    /// Get the mutex, for externally locking the cache
    Mutex & get_mutex() { return cache_mutex; }

private:
    // remove a page: return true when a page is sucessfully deleted;
    // otherwise there is only one page in cache and it is locked.
    bool remove_page(const int page_num = -1);

private:
    typedef std::tr1::unordered_map<size_t, PagePtr> Pages;
    typedef Pages::iterator PagesIter;

private:
    // the size limit
    unsigned int size_limit;

    // the memory cost of current cached pages
    int total_length;

    // the pages list
    Pages pages;

    // the mutext of the cached pages
    Mutex cache_mutex;
};

};//namespace pdf

#endif //PDF_PAGES_CACHE_H_

