
#ifndef FUNCTOR_INC_
#define FUNCTOR_INC_

#include "../2&3.Techniques/Typelist.h"
#include "../4.SmallObj/SmallObj.h"
#include <typeinfo>
#include <memory>

namespace Private
    {
        template <typename R, template <class> class ThreadingModel>
        struct FunctorImplBase : public SmallObject<ThreadingModel>
        {
            typedef R ResultType;
            
            typedef EmptyType Parm1;
            typedef EmptyType Parm2;

            virtual FunctorImplBase* DoClone() const = 0;
            template <class U>
            static U* Clone(U* pObj)
            {
                if (!pObj) return 0;
                U* pClone = static_cast<U*>(pObj->DoClone());
                assert(typeid(*pClone) == typeid(*pObj));
                return pClone;
            }
        };
    }



// macro DEFINE_CLONE_FUNCTORIMPL
// Implements the DoClone function for a functor implementation
#define DEFINE_CLONE_FUNCTORIMPL(Cls) \
    virtual Cls* DoClone() const { return new Cls(*this); }



// class template FunctorImpl
// The base class for a hierarchy of functors. The FunctorImpl class is not used
//     directly; rather, the Functor class manages and forwards to a pointer to
//     FunctorImpl
// You may want to derive your own functors from FunctorImpl.
// Specializations of FunctorImpl for up to 15 parameters follow

template <typename R, class TList, 
    template <class> class ThreadingModel = DEFAULT_THREADING>
class FunctorImpl;


// class template FunctorImpl

template <typename R, template <class> class ThreadingModel>
class FunctorImpl<R, NullType, ThreadingModel>
    : public Private::FunctorImplBase<R, ThreadingModel>
{
public:
    typedef R ResultType;
    virtual R operator()() = 0;
};

template <typename R, typename P1, template <class> class ThreadingModel>
class FunctorImpl<R, TYPELIST_1(P1), ThreadingModel>
    : public Private::FunctorImplBase<R, ThreadingModel>
{
public:
    typedef R ResultType;
    typedef typename TypeTraits<P1>::ParameterType Parm1;
    virtual R operator()(Parm1) = 0;
};

template <typename R, typename P1, typename P2, 
    template <class> class ThreadingModel>
class FunctorImpl<R, TYPELIST_2(P1, P2), ThreadingModel>
    : public Private::FunctorImplBase<R, ThreadingModel>
{
public:
    typedef R ResultType;
    typedef typename TypeTraits<P1>::ParameterType Parm1;
    typedef typename TypeTraits<P2>::ParameterType Parm2;
    virtual R operator()(Parm1, Parm2) = 0;
};


// class template FunctorHandler
// Wraps functors and pointers to functions

template <class ParentFunctor, typename Fun>
class FunctorHandler
    : public ParentFunctor::Impl
{
    typedef typename ParentFunctor::Impl Base;

public:
    typedef typename Base::ResultType ResultType;
    typedef typename Base::Parm1 Parm1;
    typedef typename Base::Parm2 Parm2;
    
    FunctorHandler(const Fun& fun) : f_(fun) {}
    
    DEFINE_CLONE_FUNCTORIMPL(FunctorHandler)

    // operator() implementations for up to 15 arguments
            
    ResultType operator()()
    { return f_(); }

    ResultType operator()(Parm1 p1)
    { return f_(p1); }
    
    ResultType operator()(Parm1 p1, Parm2 p2)
    { return f_(p1, p2); }
    
private:
    Fun f_;
};


// class template FunctorHandler
// Wraps pointers to member functions

template <class ParentFunctor, typename PointerToObj,
    typename PointerToMemFn>
class MemFunHandler : public ParentFunctor::Impl
{
    typedef typename ParentFunctor::Impl Base;

public:
    typedef typename Base::ResultType ResultType;
    typedef typename Base::Parm1 Parm1;
    typedef typename Base::Parm2 Parm2;
    
    MemFunHandler(const PointerToObj& pObj, PointerToMemFn pMemFn) 
    : pObj_(pObj), pMemFn_(pMemFn)
    {}
    
    DEFINE_CLONE_FUNCTORIMPL(MemFunHandler)
    
    ResultType operator()()
    { return ((*pObj_).*pMemFn_)(); }

    ResultType operator()(Parm1 p1)
    { return ((*pObj_).*pMemFn_)(p1); }
    
    ResultType operator()(Parm1 p1, Parm2 p2)
    { return ((*pObj_).*pMemFn_)(p1, p2); }
    
private:
    PointerToObj pObj_;
    PointerToMemFn pMemFn_;
};
    

// class template Functor
// A generalized functor implementation with value semantics

template <typename R, class TList = NullType,
    template<class> class ThreadingModel = DEFAULT_THREADING>
class Functor
{
public:
    // Handy type definitions for the body type
    typedef FunctorImpl<R, TList, ThreadingModel> Impl;
    typedef R ResultType;
    typedef TList ParmList;
    typedef typename Impl::Parm1 Parm1;
    typedef typename Impl::Parm2 Parm2;

    // Member functions

    Functor() : spImpl_(0)
    {}
    
    Functor(const Functor& rhs) : spImpl_(Impl::Clone(rhs.spImpl_.get()))
    {}
    
    Functor(std::auto_ptr<Impl> spImpl) : spImpl_(spImpl)
    {}
    
    template <typename Fun>
    Functor(Fun fun)
    : spImpl_(new FunctorHandler<Functor, Fun>(fun))
    {}

    template <class PtrObj, typename MemFn>
    Functor(const PtrObj& p, MemFn memFn)
    : spImpl_(new MemFunHandler<Functor, PtrObj, MemFn>(p, memFn))
    {}

    typedef Impl * (std::auto_ptr<Impl>::*unspecified_bool_type)() const;

    operator unspecified_bool_type() const
    {
        return spImpl_.get() ? &std::auto_ptr<Impl>::get : 0;
    }

    Functor& operator=(const Functor& rhs)
    {
        Functor copy(rhs);
        // swap auto_ptrs by hand
        Impl* p = spImpl_.release();
        spImpl_.reset(copy.spImpl_.release());
        copy.spImpl_.reset(p);
        return *this;
    }
    
    ResultType operator()() const
    { return (*spImpl_)(); }

    ResultType operator()(Parm1 p1) const
    { return (*spImpl_)(p1); }
    
    ResultType operator()(Parm1 p1, Parm2 p2) const
    { return (*spImpl_)(p1, p2); }

private:
    std::auto_ptr<Impl> spImpl_;
};


namespace Private
{
    template <class Fctor> struct BinderFirstTraits;

    template <typename R, class TList, template <class> class ThreadingModel>
    struct BinderFirstTraits< Functor<R, TList, ThreadingModel> >
    {
        typedef typename TL::Erase<TList, 
                typename TL::TypeAt<TList, 0>::Result>::Result
            ParmList;
        typedef Functor<R, ParmList, ThreadingModel> BoundFunctorType;
        typedef typename BoundFunctorType::Impl Impl;
    };        
}


// class template BinderFirst
// Binds the first parameter of a Functor object to a specific value

template <class OriginalFunctor>
class BinderFirst 
    : public Private::BinderFirstTraits<OriginalFunctor>::Impl
{
    typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
    typedef typename OriginalFunctor::ResultType ResultType;

    typedef typename OriginalFunctor::Parm1 BoundType;

    typedef typename OriginalFunctor::Parm2 Parm1;
    typedef typename OriginalFunctor::Parm3 Parm2;

public:
    BinderFirst(const OriginalFunctor& fun, BoundType bound)
    : f_(fun), b_(bound)
    {}
    
    DEFINE_CLONE_FUNCTORIMPL(BinderFirst)
    
    // operator() implementations for up to 15 arguments
            
    ResultType operator()()
    { return f_(b_); }

    ResultType operator()(Parm1 p1)
    { return f_(b_, p1); }
    
    ResultType operator()(Parm1 p1, Parm2 p2)
    { return f_(b_, p1, p2); }
    
private:
    OriginalFunctor f_;
    BoundType b_;
};


// function template BindFirst
// Binds the first parameter of a Functor object to a specific value


template <class Fctor>
typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
BindFirst(
    const Fctor& fun,
    typename Fctor::Parm1 bound)
{
    typedef typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
        Outgoing;
    
    return Outgoing(std::auto_ptr<typename Outgoing::Impl>(
        new BinderFirst<Fctor>(fun, bound)));
}


// class template Chainer
// Chains two functor calls one after another

template <typename Fun1, typename Fun2>
class Chainer : public Fun2::Impl
{
    typedef Fun2 Base;

public:
    typedef typename Base::ResultType ResultType;
    typedef typename Base::Parm1 Parm1;
    typedef typename Base::Parm2 Parm2;

    Chainer(const Fun1& fun1, const Fun2& fun2) : f1_(fun1), f2_(fun2) {}

    DEFINE_CLONE_FUNCTORIMPL(Chainer)

    // operator() implementations for up to 15 arguments

    ResultType operator()()
    { return f1_(), f2_(); }

    ResultType operator()(Parm1 p1)
    { return f1_(p1), f2_(p1); }
    
    ResultType operator()(Parm1 p1, Parm2 p2)
    { return f1_(p1, p2), f2_(p1, p2); }
    
private:
    Fun1 f1_;
    Fun2 f2_;
};


// function template Chain
// Chains two functor calls one after another


template <class Fun1, class Fun2>
Fun2 Chain(
    const Fun1& fun1,
    const Fun2& fun2)
{
    return Fun2(std::auto_ptr<typename Fun2::Impl>(
        new Chainer<Fun1, Fun2>(fun1, fun2)));
}


#endif  // FUNCTOR_INC_
