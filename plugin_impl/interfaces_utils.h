/*
 * File Name: interfaces_utils.h
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

#ifndef INTERFACE_UTILS_H_
#define INTERFACE_UTILS_H_

#include <typeinfo>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "log.h"

namespace utils
{

const unsigned int MAX_OBJECTS    = 20;     // max. instances for any ..Impl class
const unsigned int MAX_INTERFACES = 13;     // max. interfaces on any ..Impl object

/// @brief The interface entry base class.
class InterfaceBase
{
public:
    InterfaceBase() {};
    virtual ~InterfaceBase(){} ;
    virtual bool is_name_equal(const char *name) = 0;
    virtual void * get_pointer() = 0;
};

/// @brief Every InterfaceEntry stores the necessary information related
/// to a interface, such as:
/// - Interface name.
/// - Interface pointer value.
template<typename T>
class InterfaceEntry : public InterfaceBase
{
public:
    InterfaceEntry(T *p) 
        : ptr(p)
    { 
    }

    ~InterfaceEntry()
    {
    }

    bool is_name_equal(const char * name)
    {
        if (name_.size() <= 0)
        {
            name_ = typeid(T).name();
            std::string::size_type start = name_.find("IPlugin");
            if (start != std::string::npos)
            {
                name_ = name_.substr(start);
            }
            else
            {
                name_.clear();
            }
        }
        return name_ == name;
    }

    virtual void * get_pointer()
    { 
        return ptr; 
    }

private:
    std::string name_;  ///< The interface name.
    T * ptr;            ///< The casted interface pointer.
};

/// @brief The InterfaceTable contains a list of interfaces. From this class
/// caller can add and query a given interface.
class InterfaceTable
{
public:
    InterfaceTable()
    {
    }

    ~InterfaceTable()
    {
        clear();
    }

public:
    template <class T>
    void add_entry(T * ptr)
    {
        interfaces.push_back(new InterfaceEntry<T>(ptr));
        unsigned int size = interfaces.size();
        if (size > MAX_INTERFACES)
        {
            ERRORPRINTF("Too many interfaces for class %s: interfaces.size [%u] max [%u]",
                        typeid(T).name(), size, MAX_INTERFACES);
        }
    }

    bool query_interface(const char * name, void **return_ptr)
    {
        Iter begin = interfaces.begin();
        Iter end   = interfaces.end();
        for(Iter it = begin; it != end; ++it)
        {
            if ((*it)->is_name_equal(name))
            {
                *return_ptr = (*it)->get_pointer();
                return true;
            }
        }
        // This could happen when caller wants to query a optional
        // interface. Should not use assert here.
        return false;
    }

    template <class T>
    bool contain_interface(T *ptr)
    {
        Iter begin = interfaces.begin();
        Iter end   = interfaces.end();
        for(Iter it = begin; it != end; ++it)
        {
            if (ptr == (*it)->get_pointer())
            {
                return true;
            }
        }
        return false;
    }

private:
    void clear()
    {
        Iter begin = interfaces.begin();
        Iter end   = interfaces.end();
        for(Iter it = begin; it != end; ++it)
        {
            delete (*it);
        }
        interfaces.clear();
    }

private:
    typedef std::vector<InterfaceBase *> Interfaces;
    typedef std::vector<InterfaceBase *>::iterator Iter;
    Interfaces interfaces;
};

/// @brief This class enables caller to search between interface and object.
/// It can store objects of the same class.
template <typename T>
class ObjectTable
{
public:
    ObjectTable(){}
    ~ObjectTable()
    {
        unsigned int size = table_.size();
        if (size != 0)
        {
            ERRORPRINTF("ObjectTable for class %s not empty: size [%u]",
                        typeid(T).name(), size);
        }
    }

public:

    /// @brief Put object into object table. Extract the interface pointer
    /// from object. Caller should make sure the static_cast<Arg *>(object)
    /// is OK.
    template <typename Arg>
    bool add_interface(T *object)
    {
        TableIter iter = table_.find(object);
        if (iter != table_.end())
        {
            iter->second->add_entry(static_cast<Arg *>(object));
        }
        else
        {
            InterfaceTablePtr ptr = new InterfaceTable;
            ptr->add_entry<Arg>(static_cast<Arg *>(object));
            table_[object] = ptr;
            unsigned int size = table_.size();
            if (size > MAX_OBJECTS)
            {
                ERRORPRINTF("Too many objects for class %s: ObjectTable.size [%u] max [%u]",
                            typeid(T).name(), size, MAX_OBJECTS);
            }
        }
        return true;
    }

    bool query_interface(T * object, const char *name, void **return_ptr)
    {
        TableIter iter = table_.find(object);
        if (iter != table_.end())
        {
            return iter->second->query_interface(name, return_ptr);
        }
        return false;
    }

    /// @brief Remove the object from object table.
    bool remove(T * object)
    {
        TableIter iter = table_.find(object);
        if (iter != table_.end())
        {
            delete iter->second;
            table_.erase(iter);
            return true;
        }
        return false;
    }

    /// @brief Retrieve object from interface poiner.
    template <typename Arg>
    T * get_object(Arg * arg)
    {
        TableIter begin = table_.begin();
        TableIter end   = table_.end();
        for(TableIter iter = begin; iter != end; ++iter)
        {
            if (iter->second->contain_interface(arg))
            {
                return iter->first;
            }
        }
        assert(false);
        return 0;
    }


private:
    typedef InterfaceTable * InterfaceTablePtr;
    typedef std::map<T *, InterfaceTablePtr> Table;
    typedef typename std::map<T *, InterfaceTablePtr>::iterator TableIter;
    Table table_;

};

};  // namespace utils


#endif

