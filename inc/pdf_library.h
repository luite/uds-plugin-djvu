/*
 * File Name: pdf_library.h
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

#ifndef PDF_LIBRARY_H_
#define PDF_LIBRARY_H_

#include "pdf_thread.h"
#include "pdf_define.h"

namespace pdf
{

/// PDFLibrary maintains all of the documents
class PDFController;
class PDFLibrary
{
public:
    ~PDFLibrary();

    /// get the instance of library
    static PDFLibrary & instance();

    /// get the instance of thread
    Thread & get_thread() { return thread; }

    /// make enough memory for some one document
    bool make_enough_memory(PDFController *doc_ptr,
                            const int page_num,
                            const int length);

    /// add new document
    void add_document(PDFController *doc_ptr);

    /// remove the existing document
    void remove_document(PDFController *doc_ptr);

    /// remove all of the tasks related to a document
    void remove_tasks_by_document(PDFController *doc);

    /// handle adding render task
    void thread_add_render_task(Task *task, bool prerender, bool abort_current);

    /// handle adding search task
    void thread_add_search_task(Task *task);

    /// clear all of the render tasks
    void thread_cancel_render_tasks(void *user_data);

private:
    typedef std::vector<PDFController*> Documents;
    typedef Documents::iterator DocumentsIter;

private:
    // vector of documents
    Documents docs;

    // the task executing thread
    Thread thread;

    // memory limitation of PDF plugin
    unsigned int size_limit;

private:
    PDFLibrary();
    PDFLibrary(const PDFLibrary &right);

    /// try to start thread
    void try_start_thread();

    /// try to stop thread
    void try_stop_thread();
};

};

#endif //PDF_DOC_CONTROLLER_H_

