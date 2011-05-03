/*
 * File Name: pdf_render_requests.h
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

#ifndef PDF_RENDER_REQUESTS_H
#define PDF_RENDER_REQUESTS_H

#include "mutex.h"
#include "pdf_define.h"

namespace pdf
{

using namespace std;

class PDFRenderRequests
{
public:
    PDFRenderRequests();
    ~PDFRenderRequests();

    /// Add render request with priority
    void add_request(const size_t page_number, const int priority);

    /// Append render request without priority
    void append_request(const size_t page_number);

    /// Remove render request
    void remove_request(const size_t page_number);

    /// Clear all of the render requests
    void clear();

    /// Get priority of a page. Return -1 means the page is not in the
    /// request queue
    int get_priority(const size_t page_number);

    /// Update the queue by new requests
    void update(const std::vector<size_t> & requests);

    /// Check whether the page is contained in the queue
    bool contains(const size_t page_number);

private:
    typedef std::tr1::unordered_map<size_t, int> Queue;
    typedef Queue::iterator QueueIter;

private:
    /// The queue of requests
    Queue queue;

    /// Mutex of the queue
    Mutex queue_mutex;
};

};

#endif
