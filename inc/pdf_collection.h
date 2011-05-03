/*
 * File Name: pdf_collection.h
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

#ifndef PDF_COLLECTION_H_
#define PDF_COLLECTION_H_

#include "pdf_define.h"

namespace pdf
{

class PDFCollectionBase
{
public:
    PDFCollectionBase(){}
    virtual ~PDFCollectionBase(){}

public:
    virtual bool get_first_element(void **data_ptr) = 0;
    virtual int  get_count() = 0;
};

template<class T>
class PDFInstanceCollection : public PDFCollectionBase
{
public:
    PDFInstanceCollection(): vect() {}
    virtual ~PDFInstanceCollection() {clear();}

    virtual bool get_first_element(void **data_ptr)
    {
        T &t = front();
        
        *data_ptr = static_cast<void *>(&t);
        return true;
    }

    virtual int get_count()
    {
        return static_cast<int>(vect.size());
    }

    void add(T t)
    {
        vect.push_back(t);
    }

    void clear()
    {
        vect.clear();
    }

    T& get(int num)
    {
        assert(num >= 0 && num < static_cast<int>(vect.size()));

        return vect[num];
    }

    T& front()
    {
        return vect[0];
    }

    T& back()
    {
        size_t len = vect.size();
        return vect[len - 1];
    }

private:
    typedef std::vector<T> Vect;
    typedef typename Vect::iterator VectIter;

private:
    Vect vect;
};

template<class T>
class PDFCollection : public PDFCollectionBase
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

    void add(T t)
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
            delete *iter;
        }
        vect.clear();
    }

    T get(int num)
    {
        assert(num >= 0 && num < static_cast<int>(vect.size()));

        return vect[num];
    }

    T front()
    {
        if (size() > 0)
        {
            return vect[0];
        }
        return 0;
    }

    T back()
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
    typedef std::vector<T> Vect;
    typedef typename Vect::iterator VectIter;

private:
    Vect vect;
};

template <class T, class E>
class PDFElemCollection : public PDFCollection<T>
{
public:
    PDFElemCollection()
        : PDFCollection<T>()
    {}
    virtual ~PDFElemCollection() {}

    void set_element(E e) {element = e;}

    E& get_element() {return element;}

private:
    E element;
};

typedef PDFCollection<PluginRangeImpl*> PDFRangeCollection;

};//namespace pdf

#endif //PDF_COLLECTION_H_

