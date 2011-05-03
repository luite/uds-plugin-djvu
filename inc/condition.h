/*
 * File Name: condition.h
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

#ifndef CONDITION_H
#define CONDITION_H

#include <glib.h>

namespace pdf
{

class Cond
{
public:
    Cond()
        : cond_(0)
    {
        cond_  = g_cond_new();
    }

    ~Cond()
    {
        g_cond_free(cond_);
    }

    void signal()
    {
        // TODO. Do we have to lock here?
        g_cond_signal(cond_);
    }

    void wait(GMutex* m)
    {
        if (m != 0)
        {
            g_cond_wait(cond_, m);
        }
    }

private:
    GCond*   cond_;
};

};

#endif

