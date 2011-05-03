/*
 * File Name: pdf_thread.cpp
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

#include "pdf_thread.h"

namespace pdf
{

// Notice: The sequence:
// non_static_thread_func     |  abort_current_task
//   notify_end               |    running_task_mutex
//   running_task_mutex       |    abort_and_wait
// In the task, we also add necessary mechanism to prevent from waiting
// signal that will never be emitted.
// TODO: In order to improve performance, it's necessary to 
// provide abort and abort_and_wait.

Thread::Thread()
    : thread(NULL)
    , thread_cmd(CMD_NONE)
    , running_task(0)
{
}

Thread::~Thread()
{
}

gpointer Thread::thread_func(gpointer args)
{
    Thread* thiz = reinterpret_cast<Thread *>(args);
    return thiz->non_static_thread_func();
}

// Worker thread
gpointer Thread::non_static_thread_func()
{
    while (true)
    {
        Task *task = 0;
        {
            ScopeMutex m(&queue_mutex);
            while (task_queue.empty() &&
                   thread_cmd == CMD_NONE)
            {
                queue_cond.wait(queue_mutex.get_gmutex());
            }

            if (thread_cmd != CMD_NONE)
            {
                break;
            }

            // Get task from task queue
            task = task_queue.front();
            task_queue.pop_front();

            // About executing task, update running_task variable.
            ScopeMutex r(&running_task_mutex);
            running_task = task;
        }

        task->execute();
        task->notify_end();

        // Task end(aborted or finished) reset running_task.
        {
            ScopeMutex lock(&running_task_mutex);
            running_task = 0;
        }

        /// Memory leak here. When the task is aborted, the task will not
        /// be removed. So we must release the task here.
        /// Make it the only entry to release the task.
        if (!task->is_paused())
        {
            delete task;
        }
    }

    return 0;
}

void Thread::cancel_tasks(void* user_data)
{
    clear_all(user_data);

    ScopeMutex r(&running_task_mutex);
    if (running_task != 0 &&
        running_task->get_user_data() == user_data)
    {
        running_task->abort_and_wait(cancel_task_mutex);
    }
}

void Thread::clear_all(void* user_data, TaskType t)
{
    ScopeMutex m(&queue_mutex);

    TaskQueueIter idx = task_queue.begin();
    while (idx != task_queue.end())
    {
        bool is_remove = false;
        if (user_data == 0)
        {
            is_remove = true;
        }
        else if (user_data == (*idx)->get_user_data())
        {
            if (t == TASK_INVALID || (t == (*idx)->type))
            {
                is_remove = true;
            }
        }

        if (is_remove)
        {
            delete *idx;
            idx = task_queue.erase(idx);
        }
        else
        {
            idx++;
        }
    }

    if (user_data == 0)
    {
        task_queue.clear();
    }
}

bool Thread::start()
{
    if (thread != NULL)
    {
        // The thread has been started.
        return false;
    }

    // Reset the thread cmd. TODO: Replace this variable by a boolean.
    thread_cmd = CMD_NONE;
    
    thread = g_thread_create(thread_func, this, TRUE, NULL);
    return thread != NULL;
}

/// TODO, still a problem here. running task must be associated with
/// document. Can not use cancle all tasks directly.
void Thread::stop(bool cancel_all_tasks)
{
    if (thread == NULL)
    {
        return;
    }

    thread_cmd = cancel_all_tasks ? CMD_TERMINATE : CMD_STOP;

    {
        ScopeMutex r(&running_task_mutex);
        if (running_task != 0)
        {
            if (cancel_all_tasks)
            {
                running_task->abort_and_wait(cancel_task_mutex);
            }
        }
    }

    queue_cond.signal();

    // Wait for worker thread to die.
    g_thread_join(thread);

    clear_all();

    // Set the thread to be NULL
    thread = NULL;
}

bool Thread::append_task(Task* new_task)
{
    if (thread_cmd == CMD_NONE)
    {
        // Append task to the end of the task queue.
        {
            ScopeMutex m(&queue_mutex);
            task_queue.push_back(new_task);
        }

        // Tells the worker thread that a new task is available.
        queue_cond.signal();
        return true;
    }

    return false;
}

bool Thread::prepend_task(Task* new_task, bool abort_current)
{
    if (thread_cmd == CMD_NONE)
    {
        // Insert task to the beginning of the task queue.
        ScopeMutex m(&queue_mutex);
        task_queue.push_front(new_task);

        // Tells the worker thread that a new task is available.
        queue_cond.signal();

        if (abort_current)
        {
            abort_current_task(new_task);
        }

        return true;
    }

    return false;
}

bool Thread::abort_current_task(Task *new_task)
{
    ScopeMutex r(&running_task_mutex);
    if (running_task != 0)
    {
        switch (new_task->get_type())
        {
        case TASK_RENDER:
            {
                if (running_task->get_type() == TASK_SEARCH)
                {
                    // pause running task and push it next to the first task
                    running_task->pause();
                    if (!task_queue.empty())
                    {
                        TaskQueueIter idx = task_queue.begin();
                        task_queue.insert(++idx, running_task);
                    }
                    else
                    {
                        task_queue.push_front(running_task);
                    }
                }
                else
                {
                    running_task->abort();
                }
            }
            break;
        case TASK_SEARCH:
            {
                // search should not abort render task
                if (running_task->get_type() != TASK_RENDER)
                {
                    running_task->abort();
                }
            }
            break;
        default:
            break;
        }

        return true;
    }

    // Woker thread is waiting for a task
    return false;
}

bool Thread::abort_task(void* user_data, TaskType t, unsigned int id)
{
    {
        ScopeMutex r(&running_task_mutex);
        if (running_task != 0 &&
            running_task->get_type() == t &&
            running_task->get_user_data() == user_data &&
            running_task->get_id() == id)
        {
            // if running task is the one, abort it
            running_task->abort();
            return true;
        }
    }

    // search the task in queue
    ScopeMutex m(&queue_mutex);
    TaskQueueIter idx = task_queue.begin();
    while (idx != task_queue.end())
    {
        if ((*idx)->get_type() == t &&
            (*idx)->get_user_data() == user_data &&
            (*idx)->get_id() == id)
        {
            delete *idx;
            task_queue.erase(idx);
            return true;
        }
        idx++;
    }

    return false;
}

}

