#ifndef HIERARCHYGENERATORS
#define HIERARCHYGENERATORS

#include "Typelist.h"
#include "TypeTraits.h"

// The following type helps to overcome subtle flaw in the original 
// implementation of GenScatterHierarchy. 
// The flaw is revealed when the input type list of GenScatterHierarchy 
// contains more then one element of the same type (e.g. TYPELIST_2(int, int)). 
template<class, class> 
struct ScatterHierarchyTag;

template <class TList, template <class> class Unit>
class GenScatterHierarchy;

template <class T1, class T2, template <class> class Unit>
class GenScatterHierarchy<Typelist<T1, T2>, Unit>
    : public GenScatterHierarchy<ScatterHierarchyTag<T1, T2>, Unit>
    , public GenScatterHierarchy<T2, Unit>
{
public:
    typedef Typelist<T1, T2> TList;
    // Insure that LeftBase is unique and therefore reachable
    typedef GenScatterHierarchy<ScatterHierarchyTag<T1, T2>, Unit> LeftBase;
    typedef GenScatterHierarchy<T2, Unit> RightBase;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};
    
// In the middle *unique* class that resolve possible ambiguity
template <class T1, class T2, template <class> class Unit>
class GenScatterHierarchy<ScatterHierarchyTag<T1, T2>, Unit> 
    : public GenScatterHierarchy<T1, Unit>{};

template <class AtomicType, template <class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType>{
    typedef Unit<AtomicType> LeftBase;
    template <typename T> struct Rebind{
        typedef Unit<T> Result;
    };
};

template <template <class> class Unit>
class GenScatterHierarchy<NullType, Unit>{
    template <typename T> struct Rebind{
        typedef Unit<T> Result;
    };
};

//example
template <class T>
struct Holder{
    T value_;
};

// Accesses a field in an object of a type generated with GenScatterHierarchy
// obj is an object of a type H generated with GenScatterHierarchy,
// T is a type in the typelist used to generate H):
// Field<T>(obj)
// returns a reference to Unit<T>, where Unit is the template used to generate H 

template <class T, class H>
typename H::template Rebind<T>::Result& Field(H& obj){
    return obj;
}
    
template <class T, class H>
const typename H::template Rebind<T>::Result& Field(const H& obj){
    return obj;
}


////////////////////////////////////////////////////////////////////////////////
// function template TupleUnit
// The building block of tuples 
////////////////////////////////////////////////////////////////////////////////

template <class T>
struct TupleUnit{
    T value_;
    operator T&() { return value_; }
    operator const T&() const { return value_; }
};

////////////////////////////////////////////////////////////////////////////////
// class template Tuple
// Implements a tuple class that holds a number of values and provides field 
//     access to them via the Field function (below) 
////////////////////////////////////////////////////////////////////////////////

template <class TList>
struct Tuple : public GenScatterHierarchy<TList, TupleUnit>
{};


// helper class when the input type list of GenScatterHierarchy 
// contains more then one element of the same type

template <class H, unsigned int i> struct FieldHelper;
    
template <class H>
struct FieldHelper<H, 0>
{
    typedef typename H::TList::Head ElementType;
    typedef typename H::template Rebind<ElementType>::Result UnitType;
    
    enum
    {
        isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
        isConst = TypeTraits<H>::isConst
    };

    typedef const typename H::LeftBase ConstLeftBase;
    
    typedef typename Select<isConst, ConstLeftBase, 
        typename H::LeftBase>::Result LeftBase;
        
    typedef typename Select<isTuple, ElementType, 
        UnitType>::Result UnqualifiedResultType;

    typedef typename Select<isConst, const UnqualifiedResultType,
                    UnqualifiedResultType>::Result ResultType;
        
    static ResultType& Do(H& obj)
    {
        LeftBase& leftBase = obj;
        return leftBase;
    }
};

template <class H, unsigned int i>
struct FieldHelper
{
    typedef typename TL::TypeAt<typename H::TList, i>::Result ElementType;
    typedef typename H::template Rebind<ElementType>::Result UnitType;
    
    enum
    {
        isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
        isConst = TypeTraits<H>::isConst
    };

    typedef const typename H::RightBase ConstRightBase;
    
    typedef typename Select<isConst, ConstRightBase, 
        typename H::RightBase>::Result RightBase;

    typedef typename Select<isTuple, ElementType, 
        UnitType>::Result UnqualifiedResultType;

    typedef typename Select<isConst, const UnqualifiedResultType,
                    UnqualifiedResultType>::Result ResultType;
        
    static ResultType& Do(H& obj)
    {
        RightBase& rightBase = obj;
        return FieldHelper<RightBase, i - 1>::Do(rightBase);
    }
};

template <int i, class H>
typename FieldHelper<H, i>::ResultType& Field(H& obj){
    return FieldHelper<H, i>::Do(obj);
}

template <int i, class H>
const typename FieldHelper<H, i>::ResultType& 
Field(const H& obj){
    return FieldHelper<H, i>::Do(obj);
}
    
// class template GenLinearHierarchy
template <class TList,
    template <class AtomicType, class Base> class Unit,
    class Root = EmptyType>
class GenLinearHierarchy;

template <class T1, class T2,
    template <class, class> class Unit,
    class Root>
class GenLinearHierarchy<Typelist<T1, T2>, Unit, Root>
    : public Unit< T1, GenLinearHierarchy<T2, Unit, Root> >
{};

template<class T,
    template <class, class> class Unit,
    class Root
>
class GenLinearHierarchy<Typelist<T, NullType>, Unit, Root>
    : public Unit<T, Root>
{};

// template <class T, class Base>
// class EventHandler : public Base{
// public:
//     virtual void OnEvent(T& obj, int eventId);
// };

#endif