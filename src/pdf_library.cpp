/*
 * File Name: pdf_library.cpp
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

#include "pdf_library.h"
#include "pdf_doc_controller.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif

namespace pdf
{

unsigned long get_system_free_memory()
{
#ifdef _WIN32
    MEMORYSTATUS info;
    GlobalMemoryStatus(&info);
    return info.dwAvailPhys;
#else
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram * info.mem_unit;
#endif
}

PDFLibrary::PDFLibrary()
: docs()
, thread()
, size_limit(DEFAULT_SIZE_LIMIT)
{
    // start the task executing thread
    thread.start();
}

PDFLibrary::~PDFLibrary()
{
    // stop the task executing thread
    thread.stop();
}

PDFLibrary& PDFLibrary::instance()
{
    static PDFLibrary lib;
    return lib;
}

void PDFLibrary::try_start_thread()
{
    thread.start();
}

void PDFLibrary::try_stop_thread()
{
    if (docs.empty())
    {
        thread.stop();
    }
}

bool PDFLibrary::make_enough_memory(PDFController *doc_ptr,
                                    const int page_num,
                                    const int length)
{
    if (length <= 0)
    {
        WARNPRINTF("Should NOT make enough memory by length:%d", length);
        return false;
    }

    // check the free memory in system
    unsigned long free_mem = get_system_free_memory();
    if (free_mem < static_cast<unsigned long>(length << 1))
    {
        // release all of the cached bitmaps
        DocumentsIter idx = docs.begin();
        for (; idx != docs.end(); ++idx)
        {
            (*idx)->clear_cached_bitmaps();
        }
    }

    return doc_ptr->make_enough_memory(page_num, length);
}

void PDFLibrary::add_document(PDFController *doc_ptr)
{
    try_start_thread();
    docs.push_back(doc_ptr);
}

void PDFLibrary::remove_tasks_by_document(PDFController *doc)
{
    thread.cancel_tasks(doc);
}

void PDFLibrary::remove_document(PDFController *doc_ptr)
{
    DocumentsIter idx = std::find(docs.begin(), docs.end(), doc_ptr);
    if (idx != docs.end())
    {
        docs.erase(idx);
    }
    try_stop_thread();
}

void PDFLibrary::thread_add_render_task(Task *task, bool prerender, bool abort_current)
{
    if (prerender)
    {
        get_thread().append_task(task);
    }
    else
    {
        // post the rendering task into the thread's task list
        get_thread().prepend_task(task, abort_current);
    }
}

void PDFLibrary::thread_add_search_task(Task *task)
{
    get_thread().prepend_task(task, true);
}

void PDFLibrary::thread_cancel_render_tasks(void *user_data)
{
    get_thread().clear_all(user_data, TASK_RENDER);
}

}


