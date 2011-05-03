/*
 * File Name: mutex.h
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

#ifndef MUTEX_H
#define MUTEX_H

#include <glib.h>

namespace pdf
{

class Mutex
{
public:
    Mutex() { mutex_ = g_mutex_new(); }
    ~Mutex() { g_mutex_free(mutex_); }

    void lock() { g_mutex_lock(mutex_); }
    void unlock() { g_mutex_unlock(mutex_); }

    GMutex* get_gmutex() { return mutex_; }

private:
    GMutex *mutex_;
};

class ScopeMutex
{
public:
    ScopeMutex(Mutex *m)
        : mutex_(m)
    {
        mutex_->lock();
    }
    ~ScopeMutex()
    {
        mutex_->unlock();
    }

private:
    Mutex* mutex_;
};

};
#endif

