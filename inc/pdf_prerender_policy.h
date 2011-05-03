/*
 * File Name: pdf_prerender_policy.h
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

#ifndef PDF_PRERENDER_POLICY_H
#define PDF_PRERENDER_POLICY_H

#include "pdf_define.h"
#include "pdf_render_requests.h"

namespace pdf
{

using namespace std;

class PDFPrerenderPolicy
{
public:
    PDFPrerenderPolicy();
    virtual ~PDFPrerenderPolicy();

    virtual void generate_requests_list(const int current_page,
                                        const int previous_page,
                                        const int total,
                                        std::vector<size_t> & result) = 0;

    virtual int get_allowed_hyperlinks_number() = 0;

    void set_prerender(bool prerender_on) { is_prerender = prerender_on; }
    bool support_prerender() const { return is_prerender; }
    PDFRenderRequests & get_requests() { return requests; }
protected:
    PDFRenderRequests requests;
private:
    bool is_prerender;
};

class PDFPrerenderPolicyNormal : public PDFPrerenderPolicy
{
public:
    PDFPrerenderPolicyNormal();
    virtual ~PDFPrerenderPolicyNormal();

    virtual void generate_requests_list(const int current_page,
                                        const int previous_page,
                                        const int total,
                                        std::vector<size_t> & result);

    virtual int get_allowed_hyperlinks_number();

private:
    void next_pages_first(const int current_page,
                          const int total,
                          bool forward,
                          std::vector<size_t> & result);

    void faraway_pages_first(const int current_page,
                             const int total,
                             bool forward,
                             std::vector<size_t> & result);

    void nearby_pages_first(const int current_page,
                            const int total,
                            std::vector<size_t> & result);
};

};

#endif
