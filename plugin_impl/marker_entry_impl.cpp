/*
 * File Name: marker_entry_impl.cpp
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

#include "marker_entry_impl.h"
#include "log.h"

namespace pdf
{

MarkerEntry* marker_entry_new(unsigned int uds_private_size)
{
    unsigned int marker_entry_size = sizeof(MarkerEntry) + uds_private_size;
    char* p = new char[marker_entry_size];
    memset(p, 0, marker_entry_size);

    MarkerEntry* marker_entry = reinterpret_cast<MarkerEntry *>(p);
    marker_entry->type = MARKER_UDS_PRIVATE;
    return marker_entry;
}

void marker_entry_free_recursive(MarkerEntry* self)
{
    if (self->first_child)
    {
        marker_entry_free_recursive(self->first_child);
        self->first_child = 0;
    }

    if (self->sibling)
    {
        marker_entry_free_recursive(self->sibling);
        self->sibling = 0;
    }

    if (self->anchor)
    {
        StringImpl* p = static_cast<StringImpl *>(self->anchor);
        delete p;
        self->anchor = 0;
    }

    if (self->text)
    {
        StringImpl* p = static_cast<StringImpl *>(self->text);
        delete p;
        self->text = 0;
    }
}

}  // namespace pdf
