/*
 * File Name: pdf_observer.h
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

#ifndef PDF_OBSERVER_H_
#define PDF_OBSERVER_H_

#include "pdf_define.h"

/// A simple observer implementation.
namespace pdf
{

    typedef struct
    {
    }empty;

    typedef empty* UNUSABLE;

    //The base class of all functors.
    //Each functor supports specified number of parameters.
    template <class R, class A1 = UNUSABLE,
                       class A2 = UNUSABLE,
                       class A3 = UNUSABLE,
                       class A4 = UNUSABLE,
                       class A5 = UNUSABLE>
    class IFunctor
    {
    public:
        typedef IFunctor<R, A1, A2, A3, A4, A5>* IFunctorPtr;

    public:
        IFunctor(){}
        virtual ~IFunctor(){}

        virtual R operator()() = 0;
        virtual R operator()(A1 arg1) = 0;
        virtual R operator()(A1 arg1, A2 arg2) = 0;
        virtual R operator()(A1 arg1, A2 arg2, A3 arg3) = 0;
        virtual R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4) = 0;
        virtual R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5) = 0;

        //Compare whether the two functors are same.
        virtual bool compare(IFunctorPtr p) = 0;

    };

    template <class T, class R, class A1=UNUSABLE,
                                class A2=UNUSABLE,
                                class A3=UNUSABLE,
                                class A4=UNUSABLE,
                                class A5=UNUSABLE>
    class Functor : public IFunctor<R, A1, A2, A3, A4, A5>
    {

        typedef R (T::*Function0)();
        typedef R (T::*Function1)(A1);
        typedef R (T::*Function2)(A1, A2);
        typedef R (T::*Function3)(A1, A2, A3);
        typedef R (T::*Function4)(A1, A2, A3, A4);
        typedef R (T::*Function5)(A1, A2, A3, A4, A5);


        typedef Functor<T, R, A1, A2, A3, A4, A5>* FunctorPtr;

    public:
        typedef IFunctor<R, A1, A2, A3, A4, A5>* IFunctorPtr;

    public:
        //Constructors of Functor.
        Functor(T *callee, Function0 func0)
        {
            instance = callee;
            functor.func0_ = func0;
        }
        Functor(T *callee, Function1 func1)
        {
            instance = callee;
            functor.func1_ = func1;
        }
        Functor(T *callee, Function2 func2)
        {
            instance = callee;
            functor.func2_ = func2;
        }
        Functor(T *callee, Function3 func3)
        {
            instance = callee;
            functor.func3_ = func3;
        }
        Functor(T *callee, Function4 func4)
        {
            instance = callee;
            functor.func4_ = func4;
        }
        Functor(T *callee, Function5 func5)
        {
            instance = callee;
            functor.func5_ = func5;
        }

        ~Functor()
        {
        }

        //Calling operators of each functor.
        R operator()()
        {
            return (instance->*(functor.func0_))();
        }

        R operator()(A1 arg1)
        {
            return (instance->*(functor.func1_))(arg1);
        }

        R operator()(A1 arg1, A2 arg2)
        {
            return (instance->*(functor.func2_))(arg1, arg2);
        }

        R operator()(A1 arg1, A2 arg2, A3 arg3)
        {
            return (instance->*(functor.func3_))(arg1, arg2, arg3);
        }

        R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4)
        {
            return (instance->*(functor.func4_))(arg1, arg2, arg3, arg4);
        }

        R operator()(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
        {
            return (instance->*(functor.func5_))(arg1, arg2, arg3, arg4, arg5);
        }

        // Compare the function pointer
        bool compare(IFunctorPtr p) 
        { 
            return functor.func0_ == (FunctorPtr(p))->functor.func0_; 
        }


    private:
        T           *instance;
        union FUNCTOR
        {
            Function0   func0_;
            Function1   func1_;
            Function2   func2_;
            Function3   func3_;
            Function4   func4_;
            Function5   func5_;
        }functor;
    };
    
    //ReceiversOperations operates the receivers vector.
    template <class F>
    class ReceiversOperations
    {
        typedef typename std::vector<F> Receivers;
        typedef typename std::vector<F>::iterator ReceiversIter;
    public:
        // Clear all of the receivers
        static void clear(Receivers& receivers)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            for(it = begin; it != end; ++it)
            {
                delete (*it);
            }
            receivers.clear();
        }

        // Remove one receiver
        static bool remove_receiver(F p, Receivers& receivers)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            for(it = begin; it != end; ++it)
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
    };


    /// @brief Signal supporting five parameters.
    /// This is the primary class which provides the template for all partial
    /// specialized classes.
    template <class R, class A1 = UNUSABLE,
                       class A2 = UNUSABLE,
                       class A3 = UNUSABLE,
                       class A4 = UNUSABLE,
                       class A5 = UNUSABLE>
    class Signal
    {
        typedef IFunctor<R, A1, A2, A3, A4, A5>* IFunctorPtr;
    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)(A1, A2, A3, A4, A5))
        {
            IFunctorPtr func_ptr = new Functor<T, R, A1, A2, A3, A4, A5>(p, func);
            receivers.push_back(func_ptr);
        }

        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)(A1, A2, A3, A4, A5))
        {
            Functor<T, R, A1, A2, A3, A4, A5> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast(A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)(arg1, arg2, arg3, arg4, arg5);
            }
        }

        size_t count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

    /// @brief Signal supporting none parameter.
    template <class R>
    class Signal<R, UNUSABLE, UNUSABLE, UNUSABLE, UNUSABLE, UNUSABLE>
    {
        typedef IFunctor<R>* IFunctorPtr;

    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)())
        {
            IFunctorPtr func_ptr = new Functor<T, R>(p, func);
            receivers.push_back(func_ptr);
        }

        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)())
        {
            Functor<T, R> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast()
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)();
            }
        }

        size_t count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

    /// @brief Signal supporting one parameter.
    template <class R, class A1>
    class Signal<R, A1, UNUSABLE, UNUSABLE, UNUSABLE, UNUSABLE>
    {
        typedef IFunctor<R, A1>* IFunctorPtr;
    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)(A1))
        {
            IFunctorPtr func_ptr = new Functor<T, R, A1>(p, func);
            receivers.push_back(func_ptr);
        }

        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)(A1))
        {
            Functor<T, R, A1> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast(A1 arg1)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)(arg1);
            }
        }

        size_t count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

    /// @brief Signal supporting two parameters.
    template <class R, class A1,
                       class A2>
    class Signal<R, A1, A2, UNUSABLE, UNUSABLE, UNUSABLE>
    {
        typedef IFunctor<R, A1, A2>* IFunctorPtr;
    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)(A1, A2))
        {
            IFunctorPtr func_ptr = new Functor<T, R, A1, A2>(p, func);
            receivers.push_back(func_ptr);
        }

        
        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)(A1, A2))
        {
            Functor<T, R, A1, A2> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast(A1 arg1, A2 arg2)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)(arg1, arg2);
            }
        }

        unsigned int count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

    /// @brief Signal supporting three parameters.
    template <class R, class A1,
                       class A2,
                       class A3>
    class Signal<R, A1, A2, A3, UNUSABLE, UNUSABLE>
    {
        typedef IFunctor<R, A1, A2, A3>* IFunctorPtr;
    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)(A1, A2, A3))
        {
            IFunctorPtr func_ptr = new Functor<T, R, A1, A2, A3>(p, func);
            receivers.push_back(func_ptr);
        }

        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)(A1, A2, A3))
        {
            Functor<T, R, A1, A2, A3> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast(A1 arg1, A2 arg2, A3 arg3)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)(arg1, arg2, arg3);
            }
        }

        size_t count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

    /// @brief Signal supporting four parameters.
    template <class R, class A1,
                       class A2,
                       class A3,
                       class A4>
    class Signal<R, A1, A2, A3, A4, UNUSABLE>
    {
        typedef IFunctor<R, A1, A2, A3, A4>* IFunctorPtr;
    public:
        Signal(){}
        ~Signal(){ReceiversOperations<IFunctorPtr>::clear(receivers);}

    public:
        /// @brief Add receiver to receive pre-defined signals.
        template <class T>
        void add_slot(T* p, R (T::*func)(A1, A2, A3, A4))
        {
            IFunctorPtr func_ptr = new Functor<T, R, A1, A2, A3, A4>(p, func);
            receivers.push_back(func_ptr);
        }

        /// @brief Remove receiver from the receivers list.
        template <class T>
        bool remove_slot(T* p, R (T::*func)(A1, A2, A3, A4))
        {
            Functor<T, R, A1, A2, A3, A4> func_obj(p, func);
            return ReceiversOperations<IFunctorPtr>::
                   remove_receiver(&func_obj, receivers);
        }

        /// @brief Notify all receivers.
        void broadcast(A1 arg1, A2 arg2, A3 arg3, A4 arg4)
        {
            ReceiversIter begin, end, it;
            begin = receivers.begin();
            end   = receivers.end();
            IFunctorPtr ptr = 0;
            for(it= begin; it != end; ++it)
            {
                ptr = *it;
                (*ptr)(arg1, arg2, arg3, arg4);
            }
        }

        size_t count() {return receivers.size();}

    private:
        typedef typename std::vector<IFunctorPtr> Receivers;
        typedef typename std::vector<IFunctorPtr>::iterator ReceiversIter;
        Receivers receivers;
    };

};//namespace pdf

#endif

