/*
 * File Name: pdf_render_task.h
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

#ifndef PDF_RENDER_TASK_H_
#define PDF_RENDER_TASK_H_

#include "task.h"

#include "pdf_page.h"
#include "pdf_define.h"
#include "pdf_doc_controller.h"

namespace pdf
{

/// @brief The rendering task.
class PDFRenderTask : public Task
{
public:
    // constructor without existing page
    PDFRenderTask(int page_num,
                  const PDFRenderAttributes &attr,
                  PDFController *ctrl,
                  PluginRenderResultImpl *render_res,
                  int id = PRERENDER_REF_ID);

    // constructor with existing page
    PDFRenderTask(PagePtr p,
                  const PDFRenderAttributes &attr,
                  PDFController *ctrl,
                  PluginRenderResultImpl *render_res,
                  int id = PRERENDER_REF_ID);

    virtual ~PDFRenderTask();

    /// execute the rendering task
    void execute();

    /// get the pointer of PDFController intance
    void* get_user_data();

    /// get render id
    unsigned int get_id();

private:
    // PDFPage
    PagePtr page;

    // Page number
    int page_number;

    // Render settings of the page which needs to be rendered
    PDFRenderAttributes page_render_attr;

    // The reference of the document controller
    PDFController *doc_ctrl;

    // The reference id
    int ref_id;

    // The pointer of render result(allocated in main thread)
    PluginRenderResultImpl *render_result;

private:
    // Estimate whether the page is out of date
    bool is_page_out_of_date();

};

};//namespace pdf

#endif //PDF_RENDER_TASK_H_

