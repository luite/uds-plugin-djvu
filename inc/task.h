/*
 * File Name: task.h
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

#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include "condition.h"
#include "mutex.h"

namespace pdf
{

enum TaskType
{
    TASK_RENDER = 0,
    TASK_SEARCH,
    TASK_INVALID
};

class Task
{
public:
    /// Constructor of task.
    Task()
        : state(INIT)
    {
    }

    /// Destructor of task.
    virtual ~Task()
    {
    }

    /// Execute task with task context.
    virtual void execute() = 0;

    /// get the user data
    virtual void* get_user_data() = 0;

    /// get task id
    virtual unsigned int get_id() = 0;

    /// get the type of task
    TaskType get_type() const { return type; }

    /// Check if the task is aborted or not.
    inline bool is_aborted() const { return state == ABORTED; }
    void abort() { state = ABORTED; }

    /// Reset the status to "INIT"
    void reset() { state = INIT; }

    /// Pause current task, it can be used in search
    inline bool is_paused() const { return state == PAUSE; }
    void pause() { state = PAUSE; }

private:
    inline bool is_finished() const { return state == FINISHED; }

    /// Abort the task and wait until it's aborted.
    void abort_and_wait(Mutex &mutex)
    {
        ScopeMutex lock(&mutex);
        if (is_aborted() || is_finished())
        {
            return;
        }
        state = ABORTED;
        cancel_cond.wait(mutex.get_gmutex());
    }

    /// Emit the end signal to wake up the waiting thread.
    /// It's necessary to change the state, which can protect the working thread
    /// from waiting forever.
    void notify_end()
    {
        // Make sure the state is either FINISHED or ABORTED.
        // So abort_and_wait will never wait when the task is ended.
        if (!is_aborted() && !is_paused())
        {
            state = FINISHED;
        }
        cancel_cond.signal();
    }

protected:
    TaskType  type;           ///< Task type.

private:
    static const int INIT     = 0;
    static const int FINISHED = 1;
    static const int ABORTED  = 2;
    static const int PAUSE    = 3;

private:
    int       state;          ///< Task state.
    Cond      cancel_cond;    ///< Maybe should use a better name.

    friend class Thread;
};

};

#endif

