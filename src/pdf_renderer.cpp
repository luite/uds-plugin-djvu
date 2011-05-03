/*
 * File Name: pdf_renderer.cpp
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

#include "pdf_renderer.h"
#include "pdf_doc_controller.h"
#include "pdf_anchor.h"
#include "pdf_render_task.h"
#include "pdf_library.h"
#include "log.h"

namespace pdf
{

SplashColor PDFRenderer::background_color = {255, 255, 255, 0};

PDFRenderer::PDFRenderer()
: doc_controller(0)
, splash_output_dev(0)
, text_output_dev(0)
, thumbnail_output_dev(0)
, view_attr()
, cur_render_attr()
, render_mutex()
{
}

PDFRenderer::~PDFRenderer(void)
{
    destroy();
}

bool PDFRenderer::initialize(PDFController* doc)
{
    //Destroy the pre-built renderer.
    destroy();

    doc_controller = doc;
    //assert(doc_controller);
    if (doc_controller == 0)
    {
        ERRORPRINTF("Null Document Controller");
        return false;
    }

    SplashColorMode mode = splashModeMono8;

    // Create splash output device
    splash_output_dev = new SplashOutputDev(mode, 4, gFalse, PDFRenderer::background_color);
    if (splash_output_dev == 0)
    {
        ERRORPRINTF("Cannot Open Splash Output Device");
        return false;
    }

    // Start output device with the document
    splash_output_dev->startDoc(doc_controller->get_pdf_doc()->getXRef());
    
    // Create text output device
    text_output_dev = new TextOutputDev(NULL, gTrue, gFalse, gFalse);
    if (text_output_dev == 0)
    {
        ERRORPRINTF("Cannot Open Text Output Device");
        return false;
    }

    // Create thumbnail output device
    thumbnail_output_dev = new SplashOutputDev(mode, 4, gFalse, PDFRenderer::background_color);
    if (thumbnail_output_dev == 0)
    {
        ERRORPRINTF("Cannot Open Thumbnail Output Device");
        return false;
    }

    thumbnail_output_dev->startDoc(doc_controller->get_pdf_doc()->getXRef());

    init_pages_index_table();

    return true;

}

void PDFRenderer::destroy()
{
    delete splash_output_dev;
    splash_output_dev = 0;

    delete text_output_dev;
    text_output_dev = 0;

    delete thumbnail_output_dev;
    thumbnail_output_dev = 0;
}

void PDFRenderer::init_pages_index_table()
{
    // clear all of the pages.
    doc_controller->clear_cache();

    unsigned int num = doc_controller->page_count();
    for (unsigned int i = 1; i <= num; ++i)
    {
        // generate a page instance by current render setting
        gen_page(i);
    }
}

void PDFRenderer::calc_real_zoom(int page_number,
                                 const PDFRenderAttributes &origin_attr,
                                 PDFRenderAttributes &real_attr)
{
    if (doc_controller == 0)
    {
        ERRORPRINTF("Serious Problem, Null Document Controller");
        return;
    }

    real_attr = origin_attr;
    double real_zoom = origin_attr.get_zoom_setting();

    if (real_zoom < 0)
    {
        double crop_width = doc_controller->get_page_crop_width(page_number);
        double crop_height = doc_controller->get_page_crop_height(page_number);

        int display_width = doc_controller->get_display_width();
        int display_height = doc_controller->get_display_height();

        // special zoom definition
        double zoom_v = 0.0f, zoom_h = 0.0f;
        if (real_zoom == PLUGIN_ZOOM_TO_PAGE ||
            real_zoom == PLUGIN_ZOOM_TO_WIDTH)
        {
            if (real_attr.get_rotate() == Clockwise_Degrees_90 ||
                real_attr.get_rotate() == Clockwise_Degrees_270)
            {
                std::swap(crop_height, crop_width);
            }

            zoom_v = static_cast<double>(display_height) * 100 / crop_height;
            zoom_h = static_cast<double>(display_width) * 100 / crop_width;

            if (real_zoom == PLUGIN_ZOOM_TO_PAGE)
            {
                // caculate by page
                real_zoom = min(zoom_v, zoom_h);
            }
            else
            {
                // caculate by width
                real_zoom = zoom_h;
            }
        }
        else if (real_zoom == PLUGIN_ZOOM_TO_CROP_BY_PAGE ||
                 real_zoom == PLUGIN_ZOOM_TO_CROP_BY_WIDTH)
        {
            // try to get the content area from the page
            PagePtr page = doc_controller->get_page(page_number);
            if (page != 0 && is_render_area_valid(page->get_content_area()))
            {
                // calculate the real zoom by the content area
                PluginRectangle rect;
                get_content_area_in_pixel(page->get_content_area(),
                                          crop_width, crop_height, rect);

                crop_width = rect.width;
                crop_height = rect.height;
                if (real_attr.get_rotate() == Clockwise_Degrees_90 ||
                    real_attr.get_rotate() == Clockwise_Degrees_270)
                {
                    std::swap(crop_height, crop_width);
                }

                zoom_v = static_cast<double>(display_height) * 100 / crop_height;
                zoom_h = static_cast<double>(display_width) * 100 / crop_width;

                if (real_zoom == PLUGIN_ZOOM_TO_CROP_BY_PAGE)
                {
                    real_zoom = min(zoom_v, zoom_h);
                }
                else
                {
                    real_zoom = zoom_h;
                }
            }
        }
    }

    real_attr.set_real_zoom_value(real_zoom);
}

bool PDFRenderer::render_cover_page(const int width, const int height
                                    , PluginBitmapAttributes *output)
{
    if (doc_controller->page_count() < 1)
    {
        // return false if it is an empty document
        return false;
    }

    int cover_num = 1;
    // 1. calculate the zoom, fit for best
    double crop_width = doc_controller->get_page_crop_width(cover_num);
    double crop_height = doc_controller->get_page_crop_height(cover_num);

    double zoom = min(static_cast<double>(width) / crop_width,
        static_cast<double>(height) / crop_height);

    // 2. render the splash bitmap of cover
    // lock when rendering
    ScopeMutex m(&render_mutex);

    RenderRet ret = doc_controller->get_pdf_doc()->displayPage(
        get_thumbnail_output_dev()
        , cover_num
        , zoom * get_view_attr().get_device_dpi_h()
        , zoom * get_view_attr().get_device_dpi_v()
        , 0
        , gFalse //useMediaBox, TODO.
        , gFalse  //crop, TODO.
        , gFalse  //doLinks, TODO.
    );

    if (ret == Render_Error || ret == Render_Invalid)
    {
        ERRORPRINTF("Error in rendering cover page:%d\n", cover_num);
        return false;
    }

    SplashBitmap *cover_map = get_thumbnail_output_dev()->takeBitmap();
    if (cover_map != 0)
    {
        memcpy((void*)output->data, cover_map->getDataPtr(),
            cover_map->getRowSize() * cover_map->getHeight());
        delete cover_map;
        return true;
    }

    return false;
}

void PDFRenderer::post_prerender_task(const size_t page_number,
                                      const PDFRenderAttributes &page_attr)
{
    PDFRenderAttributes real_attr;
    // calculate the real render attributes
    calc_real_zoom(page_number, page_attr, real_attr);

    // try to get the page from cache
    PagePtr page = doc_controller->get_page(page_number);
    PDFRenderTask *task = 0;
    if (page)
    {
        if (!(page->get_render_attr() == page_attr) ||
            page->get_render_status() == PDFPage::RENDER_STOP)
        {
            // if reset the render attributes or the previous rendering stops
            // generate a new prerender task
            task = gen_render_task(page, real_attr);
        }
    }
    else
    {
        // append new prerender task and create a new page
        task = gen_render_task(page_number, real_attr);
    }

    if (task != 0)
    {
        PDFLibrary::instance().thread_add_render_task(task, true, false);
    }
}

void PDFRenderer::post_prerender_hyperlinks_task(PagePtr page)
{
    if (page == 0)
    {
        ERRORPRINTF("Invalid page when posting hyperlink rendering task");
        return;
    }

    Links *links = page->get_links();

    if (links == 0)
    {
        return;
    }

    int link_num = links->getNumLinks();
    if (link_num <= 0 ||
        link_num > doc_controller->get_prerender_policy()->get_allowed_hyperlinks_number())
    {
        return;
    }

    link_num = doc_controller->get_prerender_policy()->get_allowed_hyperlinks_number();
    PDFRenderAttributes real_attr;
    for (int i = 0; i < link_num; i++)
    {
        int goto_page_num = page->get_goto_page_of_link(i);
        if (goto_page_num <= 0)
        {
            //WARNPRINTF("The %d th link is invalid.", i);
            continue;
        }

        // calculate the real render attributes
        calc_real_zoom(goto_page_num, page->get_render_attr(), real_attr);

        // generate render task
        PagePtr goto_page = doc_controller->get_page(goto_page_num);
        PDFRenderTask* task = 0;
        if (goto_page)
        {
            if (!(goto_page->get_render_attr() == page->get_render_attr()) ||
                goto_page->get_render_status() == PDFPage::RENDER_STOP)
            {
                task = gen_render_task(goto_page, real_attr);
            }
        }
        else
        {
            // append new prerender task and create a new page
            task = gen_render_task(goto_page_num, real_attr);
        }

        if (task != 0)
        {
            // add the hyperlinked page into request list
            doc_controller->get_prerender_policy()->get_requests().append_request(goto_page_num);
            PDFLibrary::instance().thread_add_render_task(task, true, false);
        }
    }
}

void PDFRenderer::post_render_task(int page_num,
                                   const PDFRenderAttributes &render_attr,
                                   PluginRenderResultImpl *render_res,
                                   const unsigned int ref_id)
{
    if (page_num <= 0 || page_num > static_cast<int>(doc_controller->page_count()))
    {
        handle_page_ready(render_res, 0, TASK_RENDER_INVALID_PAGE);
        return;
    }

    // set the current displaying page
    int last_page = doc_controller->get_cur_page_num();
    doc_controller->set_cur_page_num(page_num);

    PDFRenderAttributes real_attr;
    calc_real_zoom(page_num, render_attr, real_attr);
    // update global render setting
    cur_render_attr = real_attr;

    // generate request queue of rendering based on current page and last page
    // at this moment, the previous request queue would be cleared.
    std::vector<size_t> requests;
    doc_controller->get_prerender_policy()->generate_requests_list(page_num,
                                                                   last_page,
                                                                   doc_controller->page_count(),
                                                                   requests);

    // estimate whether the requested page is cached
    PagePtr page = doc_controller->get_page(page_num);
    PDFRenderTask* render_task = 0;
    bool abort_current_task = true;
    if (page)
    {
        {
            ScopeMutex m(&(doc_controller->get_pages_cache().get_mutex()));
            if (page->get_render_attr() == cur_render_attr)
            {
                if (page->get_render_status() == PDFPage::RENDER_DONE &&
                    render_res != 0)
                {
                    // update the reference to this page, make it ready to display
                    // need NOT generate a new render task
                    page->set_ref_id(ref_id);
                    render_res->set_page(page);
                }
                else
                {
                    // create a new render task
                    render_task = gen_render_task(page,
                                                  real_attr,
                                                  render_res,
                                                  ref_id);
                    if (page->get_render_status() == PDFPage::RENDER_RUNNING)
                    {
                        abort_current_task = false;
                    }
                }
            }
            else
            {
                render_task = gen_render_task(page,
                                              real_attr,
                                              render_res,
                                              ref_id);
            }
        }
    }
    else
    {
        // the page is not cached, create a new Render task
        render_task = gen_render_task(page_num,
                                      real_attr,
                                      render_res,
                                      ref_id);
    }

    if (render_task != 0)
    {
        LOGPRINTF("PDF tries to render page:%d zoom:%f\n\n",
                  page_num,
                  real_attr.get_zoom_setting());

        // cancel all of the render tasks in the queue
        PDFLibrary::instance().thread_cancel_render_tasks(render_task->get_user_data());

        // add task for normally rendering
        PDFLibrary::instance().thread_add_render_task(render_task,
                                                      false,
                                                      abort_current_task);
    }
    else if (page != 0 && render_res != 0 &&
             page->get_render_status() == PDFPage::RENDER_DONE)
    {
        // if the page is ready, return it to UDS
        handle_page_ready(render_res, page, TASK_RENDER_DONE);
    }

    // prerender the pages, start from 1 in the requests list because
    // the normal rendering page is always located at 0.
    for (size_t idx = 1; idx < requests.size(); ++idx)
    {
        int dst_page = requests.at(idx);
        post_prerender_task(dst_page, render_attr);
    }
}

void PDFRenderer::handle_page_ready(PluginRenderResultImpl *render_res,
                                    PagePtr page,
                                    RenderStatus stat)
{
    switch (stat)
    {
    case TASK_RENDER_DONE:
        {
            if (page == 0)
            {
                ERRORPRINTF("Invalid page when handling render result");
                return;
            }

            if (render_res != 0)
            {
                // It's rendering a certain page, 
                // prerender the pages for hyperlinks on this page.
                post_prerender_hyperlinks_task(page);
            }
        }
        break;
    case TASK_RENDER_OOM:
        {
            // notify uds if it is out of memory
        }
        break;
    case TASK_RENDER_INVALID_PAGE:
        {
            // notify uds it is an invalid page
        }
        break;
    default:
        break;
    }

    if (render_res != 0)
    {
        // notify uds that the page has been done, only for rendered result
        doc_controller->sig_page_ready.broadcast(render_res, stat);
    }
}

PDFRenderTask* PDFRenderer::gen_render_task(int page_num,
                                            const PDFRenderAttributes &page_attr,
                                            PluginRenderResultImpl *render_res,
                                            int ref_id)
{
    return (new PDFRenderTask(page_num,
                              page_attr,
                              doc_controller,
                              render_res,
                              ref_id));
}

PDFRenderTask* PDFRenderer::gen_render_task(PagePtr page,
                                            const PDFRenderAttributes &page_attr,
                                            PluginRenderResultImpl *render_res,
                                            int ref_id)
{
    return (new PDFRenderTask(page,
                              page_attr,
                              doc_controller,
                              render_res,
                              ref_id));
}

PagePtr PDFRenderer::gen_page(int page_num, const PDFRenderAttributes &attr)
{
    PagePtr page = new PDFPage(page_num, attr);
    page->set_doc_controller(doc_controller);

    // put the page into cache
    doc_controller->pages_cache.add_page(page);

    return page;
}

PagePtr PDFRenderer::gen_page(int page_num)
{
    // construct a default page
    PagePtr page = new PDFPage(page_num, cur_render_attr);
    page->set_doc_controller(doc_controller);

    // put the page into cache
    doc_controller->pages_cache.add_page(page);

    return page;
}

} //namespace pdf

