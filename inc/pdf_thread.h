/*
 * File Name: pdf_thread.h
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

#ifndef PDF_THREAD_H
#define PDF_THREAD_H

#include <list>
#include <glib.h>
#include <string>
#include <vector>

#include "mutex.h"
#include "condition.h"
#include "task.h"

namespace pdf
{

class Thread
{
public:
    /// @brief Constructors and destructors.
    Thread();
    ~Thread();

    /// @brief Run thread.
    bool start();

    /// @brief Stop current thread.
    void stop(bool cancel_all_tasks = true);

    /// @brief Append the task to the end of the task queue.
    bool append_task(Task* new_task);

    /// @brief Insert the task to the head of the task queue.
    /// @param abort_current This flag tells the thread that after inserting
    /// the new task to the head of the queue, abort current running task
    /// immediately. We should NOT give user a capability to abort the 
    /// running task because the aborted task may generate a new task, and
    /// put it to the end of the queue, which may be executed by worker
    /// thread immediately. That is NOT we want.
    bool prepend_task(Task* new_task, bool abort_current);

    /// @brief Cancel all of the tasks including current one
    /// @param user_data if this value is not null, thread will clear all of the
    /// tasks with this value.
    void cancel_tasks(void* user_data = 0);

    /// @brief Remove all tasks in the task queue.
    void clear_all(void* user_data = 0, TaskType t = TASK_INVALID);

    /// @brief Abort or remove a given task
    bool abort_task(void* user_data, TaskType t, unsigned int id);

private:
    /// @brief Abort current task if thread is executing a task.
    bool abort_current_task(Task *new_task);

    /// @brief Thread functions.
    static gpointer thread_func(gpointer args);
    gpointer non_static_thread_func();

private:
    enum ThreadCmd
    {
        CMD_NONE,      ///< No external commond
        CMD_TERMINATE, ///< Terminate thread, abort current task and quit at once
        CMD_STOP       ///< Stop thread, wait until all tasks are executed
    };

    typedef std::list<Task*>    TaskQueue;
    typedef TaskQueue::iterator TaskQueueIter;
private:
    GThread          *thread;
    TaskQueue        task_queue;
    ThreadCmd        thread_cmd;
    Task*            running_task;

    /// @brief Mutexes and conditions
    Mutex    queue_mutex;
    Mutex    cancel_task_mutex;
    Mutex    running_task_mutex;
    Cond     queue_cond;
};

};
#endif // THREAD_H

