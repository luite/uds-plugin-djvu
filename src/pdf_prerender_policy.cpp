/*
 * File Name: pdf_prerender_policy.cpp
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

#include "pdf_prerender_policy.h"
#include "pdf_page.h"
#include "pdf_doc_controller.h"

namespace pdf
{

static const int ALLOWED_HYPERLINKS_NUMBER = 3;

// Compare the priority between two pages
int compare_priority(const int src_page,
                     const int dst_page,
                     PDFPrerenderPolicy *policy)
{
    int src_pri = policy->get_requests().get_priority(src_page);
    int dst_pri = policy->get_requests().get_priority(dst_page);
    // the value of priority is lower, the priority is higher
    if (src_pri > dst_pri)
    {
        return -1;
    }
    else if (src_pri < dst_pri)
    {
        return 1;
    }
    return 0;
}

bool operator < (const PDFPage & left, const PDFPage & right)
{
    // check bitmap
    // Has Bitmap < Doesn't have Bitmap
    if (left.get_bitmap() != 0 && right.get_bitmap() == 0)
    {
        return true;
    }
    else if (left.get_bitmap() == 0 && right.get_bitmap() != 0)
    {
        return false;
    }

    // check lock
    // Unlocked one < Locked one
    if (!left.locked() && right.locked())
    {
        return true;
    }
    else if (left.locked() && !right.locked())
    {
        return false;
    }

    // check GLOBAL requests list
    // compare the priority of two pages
    int ret_priority = compare_priority(left.get_page_num(),
                                        right.get_page_num(),
                                        left.get_doc_controller()->get_prerender_policy());
    if (ret_priority < 0)
    {
        return true;
    }
    return false;
}

bool operator >= (const PDFPage & left, const PDFPage & right)
{
    return !(left < right);
}

void add_request_page(const int current_page,
                      const int offset,
                      const int total,
                      bool forward,
                      std::vector<size_t> & result)
{
    int next_page = forward ? (current_page + offset) : (current_page - offset);
    if ((next_page > 0 && next_page <= total))
    {
        result.push_back(next_page);
    }
}

PDFPrerenderPolicy::PDFPrerenderPolicy()
: is_prerender(true)
{
}

PDFPrerenderPolicy::~PDFPrerenderPolicy()
{
}

PDFPrerenderPolicyNormal::PDFPrerenderPolicyNormal()
: PDFPrerenderPolicy()
{
}

PDFPrerenderPolicyNormal::~PDFPrerenderPolicyNormal()
{
}

int PDFPrerenderPolicyNormal::get_allowed_hyperlinks_number()
{
    return ALLOWED_HYPERLINKS_NUMBER;
}

void PDFPrerenderPolicyNormal::generate_requests_list(const int current_page,
                                                      const int previous_page,
                                                      const int total,
                                                      std::vector<size_t> & result)
{
    // clear the requests and put the current page at the front of the vector
    result.clear();
    result.push_back(current_page);

    // get the step of previous page and current page
    int step = current_page - previous_page;

    // the current page should always be added
    switch (step)
    {
    case -1:
    case 1:
        {
            // prerender the next pages
            next_pages_first(current_page, total, step > 0, result);
        }
        break;
    case -5:
    case 5:
        {
            // prerender the next 5 pages
            faraway_pages_first(current_page, total, step > 0, result);
        }
    case 0:
        {
            // prerender the nearby pages
            nearby_pages_first(current_page, total, result);
        }
        break;
    default:
        {
            // prerender the nearby pages
            nearby_pages_first(current_page, total, result);
        }
        break;
    }

    // update the requests queue
    requests.update(result);
}

void PDFPrerenderPolicyNormal::next_pages_first(const int current_page,
                                                const int total,
                                                bool forward,
                                                std::vector<size_t> & result)
{
    // current + 1
    add_request_page(current_page, 1, total, forward, result);

    // current - 1
    add_request_page(current_page, -1, total, forward, result);

    // current + 2
    add_request_page(current_page, 2, total, forward, result);

    // current + 3
    add_request_page(current_page, 3, total, forward, result);

    // current + 5
    add_request_page(current_page, 5, total, forward, result);
}

void PDFPrerenderPolicyNormal::faraway_pages_first(const int current_page,
                                                   const int total,
                                                   bool forward,
                                                   std::vector<size_t> & result)
{
    // current + 5
    add_request_page(current_page, 5, total, forward, result);

    // current + 1
    add_request_page(current_page, 1, total, forward, result);

    // current - 1
    add_request_page(current_page, -1, total, forward, result);

    // current + 2
    add_request_page(current_page, 2, total, forward, result);

    // current + 3
    add_request_page(current_page, 3, total, forward, result);
}

void PDFPrerenderPolicyNormal::nearby_pages_first(const int current_page,
                                                  const int total,
                                                  std::vector<size_t> & result)
{
    // current + 1
    add_request_page(current_page, 1, total, true, result);

    // current - 1
    add_request_page(current_page, -1, total, true, result);
}

}

