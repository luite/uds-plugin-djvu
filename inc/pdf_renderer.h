/*
 * File Name: pdf_renderer.h
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

#ifndef PDF_RENDERER_H_
#define PDF_RENDERER_H_

#include "goo/GooList.h"
// #include "poppler/Object.h"
// #include "poppler/SplashOutputDev.h"
// #include "splash/SplashTypes.h"
#include "pdf_doc.h"

#include "render_result_impl.h"

#include "mutex.h"
#include "pdf_page.h"
#include "pdf_collection.h"
#include "pdf_prerender_policy.h"

namespace pdf
{

class PDFViewAttributes
{
public:
    PDFViewAttributes()
        : display_width(200)
        , display_height(200)
        , color_depth(8)
        , device_dpi_v(72.0)
        , device_dpi_h(72.0)
    {}

    PDFViewAttributes(const PDFViewAttributes &attr)
        : display_width(attr.display_width)
        , display_height(attr.display_height)
        , color_depth(attr.color_depth)
        , device_dpi_v(attr.device_dpi_v)
        , device_dpi_h(attr.device_dpi_h)
    {}

    ~PDFViewAttributes() {}

    PDFViewAttributes& operator=(const PDFViewAttributes& right)
    {
        if (*this == right)
        {
            return *this;
        }

        display_width = right.display_width;
        display_height = right.display_height;
        color_depth  = right.color_depth;
        device_dpi_v = right.device_dpi_v;
        device_dpi_h = right.device_dpi_h;

        return *this;
    }

    bool operator==(const PDFViewAttributes &right)
    {
        return (this->display_width == right.display_width
            && this->display_height == right.display_height
            && this->color_depth == right.color_depth
            && this->device_dpi_v == right.device_dpi_v
            && this->device_dpi_h == right.device_dpi_h);
    }

    void set_device_dpi_h(double h) {device_dpi_h = h;}
    double get_device_dpi_h() const {return device_dpi_h;}
    void set_device_dpi_v(double v) {device_dpi_v = v;}
    double get_device_dpi_v() const {return device_dpi_v;}

    void set_display_width(const int w) {display_width = w;}
    int get_display_width() const {return display_width;}
    void set_display_height(const int h) {display_height = h;}
    int get_display_height() const {return display_height;}

    void set_color_depth(const unsigned int c) {color_depth = c;}
    unsigned int get_color_depth() const {return color_depth;}

private:
    // the display size
    int    display_width;
    int    display_height;

    // the color depth
    unsigned int color_depth;

    // the device vertical dpi
    double device_dpi_v;

    // the device horizontal dpi
    double device_dpi_h;
};

// Define the render status for a task
typedef enum RenderStatus_
{
    TASK_RENDER_DONE = 0,
    TASK_RENDER_OOM,
    TASK_RENDER_INVALID_PAGE
} RenderStatus;

class PDFController;
class PDFRenderTask;
/// @brief The document renderer class.
class PDFRenderer
{
public:
    PDFRenderer();
    virtual ~PDFRenderer(void);

    /// Initialize the pdf renderer
    bool initialize(PDFController* doc);

    /// Destroy the renderer
    void destroy();

    /// Post the render task into the task list of thread
    void post_render_task(int page_num, 
                          const PDFRenderAttributes &render_attr,
                          PluginRenderResultImpl *render_res,
                          const unsigned int ref_id = PRERENDER_REF_ID);

    /// Handle the page ready event
    /// if param page is not null, it means the page has been successfully
    /// rendered. If reference number of the page is invalid(-1), it means
    /// this page is rendered by prerendering. We don't have to notify UDS.
    /// if param page is null, it means the page has not been successfully
    /// rendered (caused by out of memory).
    void handle_page_ready(RenderResultPtr render_res,
                           PagePtr page,
                           RenderStatus stat);

    /// Get the view attributes
    PDFViewAttributes& get_view_attr() { return view_attr; }

    /// Get the current render attributes
    const PDFRenderAttributes& get_render_attr() { return cur_render_attr; }

    /// Generate a page by input context
    PagePtr gen_page(int page_num, const PDFRenderAttributes &attr);

    /// Generate a page by default context
    PagePtr gen_page(int page_num);

    /// Get the mutex of rendering
    Mutex & get_render_mutex() { return render_mutex; }

    /// Render the cover page for UDS
    bool render_cover_page(const int width,
                           const int height,
                           PluginBitmapAttributes *output);

    /// Get the maximum/minimum zoom factor
    static double get_max_zoom() { return MAX_ZOOM; }
    static double get_min_zoom() { return MIN_ZOOM; }

public:
    static SplashColor background_color;

private:
    // Generate render task without existing page
    PDFRenderTask* gen_render_task(int page_num,
                                   const PDFRenderAttributes &page_attr,
                                   PluginRenderResultImpl *render_res = 0,
                                   int ref_id = PRERENDER_REF_ID);

    // Generate render task with existing page
    PDFRenderTask* gen_render_task(PagePtr page,
                                   const PDFRenderAttributes &page_attr,
                                   PluginRenderResultImpl *render_res = 0,
                                   int ref_id = PRERENDER_REF_ID);

    // Initialize all of the PDFPage instances
    // The index table of all pages is constructed
    void init_pages_index_table();

    // Record the initial time
    void init_start_time();

    void calc_real_zoom(int page_number,
                        const PDFRenderAttributes &origin_attr,
                        PDFRenderAttributes &real_attr);

    // Get the output devices
    SplashOutputDev* get_splash_output_dev() const { return splash_output_dev; }
    SplashOutputDev* get_thumbnail_output_dev() const { return thumbnail_output_dev; }
    TextOutputDev* get_text_output_dev() const { return text_output_dev; }

    // Post prerender task
    void post_prerender_task(const size_t page_number,
                             const PDFRenderAttributes &page_attr);

    // Post prerender hyperlinks task
    void post_prerender_hyperlinks_task(PagePtr page);

private:
    // Reference to the PDFController instance
    PDFController   *doc_controller;

    // The output device of image
    SplashOutputDev *splash_output_dev;

    // The output device of text
    TextOutputDev   *text_output_dev;

    // The output device of thumbnail image
    SplashOutputDev *thumbnail_output_dev;

    // the view attributes
    PDFViewAttributes view_attr;

    // default render settings
    PDFRenderAttributes cur_render_attr;

    // the mutex used in rendering
    Mutex render_mutex;

    friend class PDFRenderTask;
    friend class PDFPage;
};

};

#endif //PDF_RENDERER_H_

