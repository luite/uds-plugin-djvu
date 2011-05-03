/*
 * File Name: pdf_render_requests.cpp
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

#include "pdf_render_requests.h"

namespace pdf
{

static const int PRIORITY_MIN = 0xFFFF;

PDFRenderRequests::PDFRenderRequests()
{
}

PDFRenderRequests::~PDFRenderRequests()
{
}

void PDFRenderRequests::add_request(const size_t page_number, const int priority)
{
    ScopeMutex m(&queue_mutex);
    queue[page_number] = priority;
}

void PDFRenderRequests::append_request(const size_t page_number)
{
    ScopeMutex m(&queue_mutex);
    // the request is appeneded with the lowest priority in the queue
    if (queue.find(page_number) != queue.end())
    {
        // if the page is in the request list, stop appending
        return;
    }

    int priority = -1;
    QueueIter iter = queue.begin();
    for (; iter != queue.end(); ++iter)
    {
        if (iter->second > priority)
        {
            priority = iter->second;
        }
    }

    queue[page_number] = (priority + 1);
}

void PDFRenderRequests::remove_request(const size_t page_number)
{
    ScopeMutex m(&queue_mutex);
    QueueIter iter = queue.find(page_number);
    if (iter != queue.end())
    {
        queue.erase(iter);
    }
}

void PDFRenderRequests::clear()
{
    ScopeMutex m(&queue_mutex);
    queue.clear();
}

int PDFRenderRequests::get_priority(const size_t page_number)
{
    ScopeMutex m(&queue_mutex);
    QueueIter iter = queue.find(page_number);
    if (iter == queue.end())
    {
        return PRIORITY_MIN;
    }
    return queue[page_number];
}

void PDFRenderRequests::update(const std::vector<size_t> & requests)
{
    ScopeMutex m(&queue_mutex);
    queue.clear();
    for (size_t idx = 0; idx < requests.size(); ++idx)
    {
        queue[requests.at(idx)] = idx;
    }
}

bool PDFRenderRequests::contains(const size_t page_number)
{
    ScopeMutex m(&queue_mutex);
    return (queue.find(page_number) != queue.end());
}

}
