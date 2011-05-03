/*
 * File Name: thread.h
 */

/*
 * This file is part ofuds-plugin-common.
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

#ifndef THREAD_H
#define THREAD_H

#include <list>
#include <glib.h>

namespace common
{

enum ThreadCmd
{
    CMD_NONE,      ///< No external commond
    CMD_TERMINATE, ///< Terminate thread, abort current task and quit at once
    CMD_STOP       ///< Stop thread, wait until all tasks are executed
};

template <typename T>
class SafeData
{
public:
    /// @brief Constructors and destructors
    SafeData()
    {
        g_static_rw_lock_init(&rw_lock);
    }
    ~SafeData()
    {
        g_static_rw_lock_free(&rw_lock);
    }

    /// @brief Safely get the value
    T get_value()
    {
        
        g_static_rw_lock_reader_lock(&rw_lock);
        T tmp = data;
        g_static_rw_lock_reader_unlock(&rw_lock);

        return tmp;
    }

    /// @brief Safely set the value
    void set_value(const T& new_value)
    {
        g_static_rw_lock_writer_lock(&rw_lock);
        data = new_value;
        g_static_rw_lock_writer_unlock(&rw_lock);
    }

private:
    GStaticRWLock rw_lock;
    T data;
};

class Task
{
public:
    /// @brief Constructors and destructors.
    Task();
    virtual ~Task();

    /// @brief Execute task with task context.
    virtual void execute() = 0;

    /// @brief Send abort request to the current running task.
    void send_abort_request();

    bool is_aborted() const
    {
        return aborted;
    }

private:
    bool aborted;
};

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

    /// @brief Remove all tasks in the task queue.
    void clear_all();

private:
    /// @brief Abort current task if thread is executing a task.
    bool abort_current_task();

    /// @brief Thread functions.
    static gpointer thread_func(gpointer args);
    gpointer non_static_thread_func();

private:
    GThread          *thread;
    std::list<Task*> task_queue;
    ThreadCmd        thread_cmd;
    Task*            running_task;

    /// @brief Mutexes and conditions
    GMutex *queue_mutex;
    GCond  *queue_cond;
};

}; // namespace common

#endif // THREAD_H
