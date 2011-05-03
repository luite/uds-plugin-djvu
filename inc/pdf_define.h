/*
 * File Name: pdf_define.h
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

#ifndef PDF_DEFINE_H_
#define PDF_DEFINE_H_

#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <list>
#include <cassert>
#include <glib.h>
#include <iconv.h>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <goo/GooString.h>
#include <goo/GooList.h>
// #include <splash/SplashBitmap.h>

// #include <poppler/Link.h>
// #include <poppler/TextOutputDev.h>
// #include <poppler/PDFDoc.h>
// #include <poppler/GlobalParams.h>
// #include <poppler/Outline.h>
// #include <poppler/Link.h>

#include <string.h> // for memset
#include "pdf_doc.h"

#include "plugin_inc.h"
#include "plugin_type.h"
#include "plugin_render.h"
#include "string_impl.h"

namespace pdf
{

using namespace std;

typedef string string;
typedef vector<std::string> stringlist;

#ifdef WIN32
#include <windows.h>
#define pdf_printf sprintf_s
#else
#include <sys/time.h>
#define pdf_printf sprintf
#endif

#define RENDER_TEXT
#define PRERENDER_NUMBER       2

#define ZERO_RANGE             0.0001
#define PRERENDER_REF_ID       -1

#define ANCHOR_COMPARE_ERROR   -2
#define DEFAULT_SIZE_LIMIT      30 * 1024 * 1024

#define MAX_ZOOM               6401.0f
#define MIN_ZOOM               8.0f

typedef enum
{
    PDF_SEARCH_ALL = 0,
    PDF_SEARCH_NEXT
}PDFSearchType;

/// Define the PluginRangeImpl for replacing the reference of PluginRange.
/// This class would destroy the start and end anchor automatically.
class PluginRangeImpl
{
public:
    PluginRangeImpl()
        : start_anchor(0)
        , end_anchor(0)
    {}
    ~PluginRangeImpl()
    {
        if (start_anchor != 0)
        {
            delete (StringImpl*)start_anchor;
            start_anchor = 0;
        }
        if (end_anchor != 0)
        {
            delete (StringImpl*)end_anchor;
            end_anchor = 0;
        }
    }
public:
    UDSString* start_anchor;
    UDSString* end_anchor;
};

/// Get current time
int get_cur_time();

/// Estimate whether the render area is valid
bool is_render_area_valid(const RenderArea & area);

/// Get the content area in pixel
void get_content_area_in_pixel(const RenderArea & area,
                               const int origin_width,
                               const int origin_height,
                               PluginRectangle & rect);

/// Compare two PDF pages
class PDFPage;
bool operator < (const PDFPage & left, const PDFPage & right);
bool operator >= (const PDFPage & left, const PDFPage & right);

/// Compare the priority of two pages
class PDFPrerenderPolicy;
int compare_priority(const int src_page,
                     const int dst_page,
                     PDFPrerenderPolicy *policy);
};

#endif //PDF_DEFINE_H_

