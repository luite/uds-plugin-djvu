/*
 * File Name: marker_entry_impl.h
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

#ifndef PDF_MARKER_ENTRY_IMPL_H_
#define PDF_MARKER_ENTRY_IMPL_H_

#include "plugin_inc.h"
#include "pdf_collection.h"
#include "string_impl.h"
#include "log.h"

namespace pdf
{

// Function to allocate a MarkerEntry structure.
MarkerEntry* marker_entry_new(unsigned int uds_private_size);

// Release the whole marker tree pointed by self.
void marker_entry_free_recursive(MarkerEntry* self);

template <>
class PDFCollection<MarkerEntry *> : public PDFCollectionBase
{
public:
    PDFCollection(): vect() {}
    virtual ~PDFCollection() {clear();}

    virtual bool get_first_element(void **data_ptr)
    {
        if (size())
        {
            *data_ptr = static_cast<void *>(&vect[0]);
            return true;
        }
        return false;
    }

    virtual int get_count()
    {
        return size();
    }

    void add(MarkerEntry* t)
    {
        vect.push_back(t);
    }

    void clear()
    {
        VectIter begin = vect.begin();
        VectIter end   = vect.end();
        VectIter iter  = begin;
        for(; iter != end; ++iter)
        {
            MarkerEntry* first_child = reinterpret_cast<MarkerEntry *>(*iter);
            marker_entry_free_recursive(first_child);
        }
        vect.clear();
    }

    MarkerEntry* get(int num)
    {
        assert(num >= 0 && num < static_cast<int>(vect.size()));

        return vect[num];
    }

    MarkerEntry* front()
    {
        if (size() > 0)
        {
            return vect[0];
        }
        return 0;
    }

    MarkerEntry* back()
    {
        if (size() > 0)
        {
            return vect[size()-1];
        }
        return 0;
    }

    int size()
    {
        return static_cast<int>(vect.size());
    }

private:
    typedef std::vector<MarkerEntry *> Vect;
    typedef std::vector<MarkerEntry *>::iterator VectIter;

private:
    Vect vect;
};

};  // namespace pdf

#endif
