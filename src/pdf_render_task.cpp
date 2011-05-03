/*
 * File Name: pdf_render_task.cpp
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

#include  "log.h"

#include "pdf_render_task.h"
#include "pdf_library.h"

namespace pdf
{

PDFRenderTask::PDFRenderTask(int page_num, const PDFRenderAttributes &attr
                             , PDFController *ctrl, PluginRenderResultImpl *render_res
                             , int id)
: page(0)
, page_number(page_num)
, page_render_attr(attr)
, doc_ctrl(ctrl)
, ref_id(id)
, render_result(render_res)
{
    type = TASK_RENDER;
}

PDFRenderTask::PDFRenderTask(PagePtr p, const PDFRenderAttributes &attr
                             , PDFController *ctrl, PluginRenderResultImpl *render_res
                             , int id)
: page(p)
, page_number(page->get_page_num())
, page_render_attr(attr)
, doc_ctrl(ctrl)
, ref_id(id)
, render_result(render_res)
{
    type = TASK_RENDER;
}

PDFRenderTask::~PDFRenderTask()
{
}

void PDFRenderTask::execute()
{
    // don't execute the prerender task if the page is out of date
    if (is_page_out_of_date())
    {
        return;
    }

    PDFRenderer *renderer = doc_ctrl->get_renderer();

    // estimate whether the page has been cached
    // it is necessary here although there is same estimation
    // in main thread, because the same page might be constructed
    // in other tasks.
    if (page == 0)
    {
        page = doc_ctrl->get_page(page_number);

        if (page == 0)
        {   
            page = renderer->gen_page(page_number, page_render_attr);
        }
    }

    //assert(page);
    if (page == 0)
    {
        ERRORPRINTF("Cannot Create New Page");
        return;
    }

    // set the reference id, this operation is thread-safe now
    // NOTE: this function must be called before setting render attributes
    // because the main thread would update the ref id if the render attributes
    // changes.
    page->set_ref_id(ref_id);

    // if it is ZOOM_AUTO_CROP mode, it means it is necessary to get the content
    // area of the page
    double real_zoom = page_render_attr.get_real_zoom_value();
    if (real_zoom == PLUGIN_ZOOM_TO_CROP_BY_PAGE ||
        real_zoom == PLUGIN_ZOOM_TO_CROP_BY_WIDTH)
    {
        RenderArea content_area;
        if (!page->get_content_area(renderer, content_area))
        {
            ERRORPRINTF("Cannot get content area of page:%d", page_number);
            return;
        }
        PDFRenderAttributes origin_attr = page_render_attr;
        renderer->calc_real_zoom(page_number, origin_attr, page_render_attr);
        real_zoom = page_render_attr.get_real_zoom_value();
    }

    // if the page is cached and the bitmap has been rendered
    // calculate the delta value. Becuase the old bitmap would
    // be destroyed and new one is going to be rendered in the following
    // step
    int page_len = static_cast<int>(page->length());

    page_len = static_cast<int>(PDFPage::try_calc_length(real_zoom,
                                doc_ctrl->get_page_crop_width(page_number),
                                doc_ctrl->get_page_crop_height(page_number))) -
               page_len;

    PDFPage::RenderStatus cur_status = page->get_render_status();
    if (!(page->get_render_attr() == page_render_attr))
    {
        // if the render attributes change, re-render the page
        // DO NOT update the render setting here, because it might
        // change the length of page
        // DO NOT change the status of page at this moment
        cur_status = PDFPage::RENDER_STOP;
    }
    
    bool render_done = true;

    // render bitmap
    if (cur_status != PDFPage::RENDER_DONE)
    {
        TRACE("Task, Render Page:%d, Ref ID:%d, Current Task:%p\n\n", page_number, ref_id, this);
        // update the page cache to make sure there is enough memory
        // if page_len < 0, it means the page is going to shrink, the memory
        // must be enough
        // TODO. Add the page number as one parameter for making enough memory
        // The pages with lower priorites would be released.
        if (page_len > 0 &&
            !PDFLibrary::instance().make_enough_memory(doc_ctrl,
                                                       page_number,
                                                       page_len))
        {
            WARNPRINTF("Cannot make enough memory to implement rendering");

            // notify uds that it is out of memory that the page rendering
            // is aborted
            renderer->handle_page_ready(render_result, page, TASK_RENDER_OOM);
            return;
        }

        // update the render status
        page->set_render_status(cur_status);

        // only set the render attributes
        page->set_render_attr(page_render_attr);

        render_done = page->render_splash_map(renderer
            , static_cast<void*>(this));

        // render the text page when the render is done
        if (render_done)
        {
            page->render_text(renderer);
        }
    }

    if (render_done)
    {
        // set the render status at last
        page->set_render_status(PDFPage::RENDER_DONE);

        if (render_result != 0)
        {
            // set the page into render result
            render_result->set_page(page);
        }

        // notify uds that the page is ready
        renderer->handle_page_ready(render_result, page, TASK_RENDER_DONE);
    }
    else
    {
        if (render_result != 0 && is_aborted())
        {
            // if the task is aborted, set the render result to "Discard"
            render_result->set_discard(true);
        }
        else
        {
            renderer->handle_page_ready(render_result, page, TASK_RENDER_OOM);
        }
    }

}

bool PDFRenderTask::is_page_out_of_date()
{
    // check whether the page is in request list
    bool out_of_date = !(doc_ctrl->get_prerender_policy()->get_requests().contains(
                         page_number));

    if (out_of_date)
    {
        return out_of_date;
    }

    PDFRenderer *renderer = doc_ctrl->get_renderer();
    // check the render setting
    if (page_render_attr.get_zoom_setting() !=
        renderer->get_render_attr().get_zoom_setting() ||
        page_render_attr.get_rotate() !=
        renderer->get_render_attr().get_rotate())
    {
        out_of_date = true;
    }

    return out_of_date;
}

void* PDFRenderTask::get_user_data()
{
    return doc_ctrl;
}

unsigned int PDFRenderTask::get_id()
{
    return ref_id;
}

}// namespace pdf

