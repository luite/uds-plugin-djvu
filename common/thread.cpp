/*
 * File Name: thread.cpp
 */

/*
 * This file is part of uds-plugin-common.
 *
 * uds-plugin-common is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-common is distributed in the hope that it will be useful,
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

#include "thread.h"

namespace common
{

Task::Task()
{
    aborted = false;
}

Task::~Task()
{
}

void Task::send_abort_request()
{
    aborted = true;
}

Thread::Thread()
: thread(NULL),
  thread_cmd(CMD_NONE),
  running_task(0)
{
    queue_mutex = g_mutex_new();
    queue_cond  = g_cond_new();
}

Thread::~Thread()
{
    g_mutex_free(queue_mutex);
    g_cond_free(queue_cond);
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
        g_mutex_lock(queue_mutex);
        while (task_queue.empty())
        {
            if (thread_cmd != CMD_NONE)
            {
                g_mutex_unlock(queue_mutex);
                goto CleanUp;
            }

            g_cond_wait(queue_cond, queue_mutex);
        }

        if (thread_cmd == CMD_TERMINATE)
        {
            g_mutex_unlock(queue_mutex);
            goto CleanUp;
        }

        // Get task from task queue
        Task *task = task_queue.front();
        task_queue.pop_front();

        g_mutex_unlock(queue_mutex);

        // About executing task, update running_task variable.
        running_task = task;
        task->execute();
        // Task execution done, update running_task variable.
        running_task = 0;

        // Release task object
        delete task;
    }

CleanUp:
    clear_all();
    return 0;
}

void Thread::clear_all()
{
    g_mutex_lock(queue_mutex);

    std::list<Task*>::iterator it = task_queue.begin();
    while (it != task_queue.end())
    {
        delete *it;
        it = task_queue.erase(it);
    }

    g_mutex_unlock(queue_mutex);
}

bool Thread::start()
{
    if (thread != NULL)
    {
        // The thread has been started.
        return false;
    }

    thread = g_thread_create(thread_func, this, TRUE, NULL);
    return thread != NULL;
}

void Thread::stop(bool cancel_all_tasks)
{
    if (thread == NULL)
    {
        return;
    }

    thread_cmd = cancel_all_tasks ? CMD_TERMINATE : CMD_STOP;

    if (running_task != 0)
    {
        if (cancel_all_tasks)
        {
            running_task->send_abort_request();
        }
    }
    else
    {
        // Woker thread is waiting for a task, wake it up.
        g_cond_signal(queue_cond);
    }

    // Wait for worker thread to die.
    g_thread_join(thread);

    // Set the thread to be NULL
    thread = NULL;
}

bool Thread::append_task(Task* new_task)
{
    if (thread_cmd == CMD_NONE)
    {
        // Append task to the end of the task queue.
        g_mutex_lock(queue_mutex);
        task_queue.push_back(new_task);
        g_mutex_unlock(queue_mutex);

        // Tells the worker thread that a new task is available.
        g_cond_signal(queue_cond);
        return true;
    }
    else
    {
        delete new_task;
        return false;
    }
}

bool Thread::prepend_task(Task* new_task, bool abort_current)
{
    if (thread_cmd == CMD_NONE)
    {
        // Insert task to the beginning of the task queue.
        g_mutex_lock(queue_mutex);
        task_queue.push_front(new_task);

        if (abort_current)
        {
            abort_current_task();
        }

        g_mutex_unlock(queue_mutex);

        // Tells the worker thread that a new task is available.
        g_cond_signal(queue_cond);

        return true;
    }
    else
    {
        delete new_task;
        return false;
    }
}

bool Thread::abort_current_task()
{
    if (running_task != 0)
    {
        // Worker thread is executing a task, just abort it.
        running_task->send_abort_request();
        return true;
    }
    else
    {
        // Woker thread is waiting for a task
        return false;
    }
}

}
