/*
 * File Name: signal_slot.h
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

#ifndef SIGNAL_SLOT_H_
#define SIGNAL_SLOT_H_

#include <vector>

/// A simple signal/slot implementation.
namespace utils
{

struct empty {} ;
typedef empty* UNUSABLE;

template < typename A1 = UNUSABLE,
           typename A2 = UNUSABLE,
           typename A3 = UNUSABLE,
           typename A4 = UNUSABLE,
           typename A5 = UNUSABLE>
class SlotBase
{
public:
    SlotBase(){}
    virtual ~SlotBase(){}
    virtual void operator()() = 0;
    virtual void operator()(A1 arg1) = 0;
    virtual void operator()(A1 arg1, A2 arg2) = 0;
    virtual void operator()(A1 arg1, A2 arg2, A3 arg3) = 0;
    virtual void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) = 0;
    virtual void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5) = 0;
    virtual bool compare(SlotBase<A1, A2, A3, A4, A5> * p) = 0;
};


template < class T,  
           typename A1 = UNUSABLE,
           typename A2 = UNUSABLE,
           typename A3 = UNUSABLE,
           typename A4 = UNUSABLE,
           typename A5 = UNUSABLE>
class Slot : public SlotBase<A1, A2, A3, A4, A5>
{
private:
    typedef void (T::*Function0)();
    typedef void (T::*Function1)(A1);
    typedef void (T::*Function2)(A1, A2);
    typedef void (T::*Function3)(A1, A2, A3);
    typedef void (T::*Function4)(A1, A2, A3, A4);
    typedef void (T::*Function5)(A1, A2, A3, A4, A5);
    typedef Slot<T, A1, A2, A3, A4, A5>* SlotPtr;

public:

    Slot(T *p, Function0 f0) : instance(p) { functor.func0_ = f0; }
    Slot(T *p, Function1 f1) : instance(p) { functor.func1_ = f1; }
    Slot(T *p, Function2 f2) : instance(p) { functor.func2_ = f2; }
    Slot(T *p, Function3 f3) : instance(p) { functor.func3_ = f3; }
    Slot(T *p, Function4 f4) : instance(p) { functor.func4_ = f4; }
    Slot(T *p, Function5 f5) : instance(p) { functor.func5_ = f5; }
    ~Slot() {}

    void operator()()
    {
        (instance->*(functor.func0_))();
    }

    void operator()(A1 arg1)
    {
        (instance->*(functor.func1_))(arg1);
    }

    void operator()(A1 arg1, A2 arg2)
    {
        (instance->*(functor.func2_))(arg1, arg2);
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3)
    {
        (instance->*(functor.func3_))(arg1, arg2, arg3);
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4)
    {
        (instance->*(functor.func4_))(arg1, arg2, arg3, arg4);
    }

    void operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
    {
        (instance->*(functor.func5_))(arg1, arg2, arg3, arg4, arg5);
    }

    // Compare the function pointer
    bool compare(SlotBase<A1, A2, A3, A4, A5> * p) 
    { 
        // Have to down_cast. Should compare the instance?
        SlotPtr ptr = static_cast<SlotPtr>(p);
        return  ( functor.func0_ == ptr->functor.func0_ &&
                  instance == ptr->instance );
    }

private:
    T  *instance;
    union Functor
    {
        Function0   func0_;
        Function1   func1_;
        Function2   func2_;
        Function3   func3_;
        Function4   func4_;
        Function5   func5_;
    }functor;
};

template < typename A1 = UNUSABLE,
           typename A2 = UNUSABLE,
           typename A3 = UNUSABLE,
           typename A4 = UNUSABLE,
           typename A5 = UNUSABLE>
class Signal
{
    typedef SlotBase<A1, A2, A3, A4, A5>* SlotBasePtr;

public:
    Signal() 
    {}

    ~Signal(){clear();}

public:
    /// @brief Add receiver to receive pre-defined signals.
    template <class T>
    void add_slot(T* obj, void (T::*func)(void))
    {
        SlotBasePtr p = new Slot<T>(obj, func);
        receivers.push_back(p);
    }

    template <class T>
    void add_slot(T* obj, void (T::*func)(A1))
    {
        SlotBasePtr p = new Slot<T, A1>(obj, func);
        receivers.push_back(p);
    }

    template <class T>
    void add_slot(T* obj, void (T::*func)(A1, A2))
    {
        SlotBasePtr p = new Slot<T, A1, A2>(obj, func);
        receivers.push_back(p);
    }

    template <class T>
    void add_slot(T* obj, void (T::*func)(A1, A2, A3))
    {
        SlotBasePtr p = new Slot<T, A1, A2, A3>(obj, func);
        receivers.push_back(p);
    }

    template <class T>
    void add_slot(T* obj, void (T::*func)(A1, A2, A3, A4))
    {
        SlotBasePtr p = new Slot<T, A1, A2, A3, A4>(obj, func);
        receivers.push_back(p);
    }

    template <class T>
    void add_slot(T* obj, void (T::*func)(A1, A2, A3, A4, A5))
    {
        SlotBasePtr p = new Slot<T, A1, A2, A3, A4, A5>(obj, func);
        receivers.push_back(p);
    }
    
    /// @brief Remove receiver from the receivers list.
    template <class T>
    bool remove_slot(T* p, void (T::*func)())
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    template <class T>
    bool remove_slot(T* p, void (T::*func)(A1 arg1))
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    template <class T>
    bool remove_slot(T* p, void (T::*func)(A1 arg1, A2 arg2))
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    template <class T>
    bool remove_slot(T* p, void (T::*func)(A1 arg1, A2 arg2, A3 arg3))
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    template <class T>
    bool remove_slot(T* p, void (T::*func)(A1 arg1, A2 arg2, A3 arg3, A4 arg4))
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    template <class T>
    bool remove_slot(T* p, void (T::*func)(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5))
    {
        Slot<T, A1, A2, A3, A4, A5> func_obj(p, func);
        return remove_receiver(&func_obj);
    }

    /// @brief Notify all receivers. 
    void broadcast()
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))();
        }
    }

    void broadcast(A1 arg1)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))(arg1);
        }
    }

    /// safe_broadcast is added for object that will be released
    /// during boradcasting.
    void safe_broadcast(A1 arg1)
    {
        Signal<A1, A2, A3, A4, A5> object;
        object.receivers = receivers;
        object.broadcast(arg1);
        object.receivers.clear();
    }

    void broadcast(A1 arg1, A2 arg2)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))(arg1, arg2);
        }
    }

    void broadcast(A1 arg1, A2 arg2, A3 arg3)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))(arg1, arg2, arg3);
        }
    }

    void broadcast(A1 arg1, A2 arg2, A3 arg3, A4 arg4)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))(arg1, arg2, arg3, arg4);
        }
    }

    void broadcast(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it= begin; it != end; ++it)
        {
            (*(*it))(arg1, arg2, arg3, arg4, arg5);
        }
    }


    unsigned int count()
    {
        return static_cast<unsigned int>(receivers.size());
    }

private:
    // Clear all of the receivers
    void clear()
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it = begin; it != end; ++it)
        {
            delete (*it);
        }
        receivers.clear();
    }

    // Remove one receiver
    bool remove_receiver(SlotBasePtr p)
    {
        ReceiversIter begin = receivers.begin();
        ReceiversIter end   = receivers.end();
        for(ReceiversIter it = begin; it != end; ++it)
        {
            if ((*it)->compare(p))
            {
                delete (*it);
                receivers.erase(it);
                return true;
            }
        }
        return false;
    }

private:
    typedef std::vector<SlotBasePtr> Receivers;
    typedef typename std::vector<SlotBasePtr>::iterator ReceiversIter;
    Receivers receivers;
};

};  // namespace utils

#endif
