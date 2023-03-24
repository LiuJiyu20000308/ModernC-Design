#ifndef TYPELIST_H
#define TYPELIST_H

#include "TypeTraits.h"

template <class T, class U>
struct Typelist{
    typedef T Head;
    typedef U Taill
};

namespace TL{

    // Compute the length of a typelist
    template <class TList> struct Length;
    template<> struct Length<NullType>{
        enum { value = 0 };
    };
    template <class T, class U>
    struct Length< Typelist<T,U> >{
        enum {value = 1 + Length<U>::value };
    };
    
    //find the type at a given index in a typelist
    template <class TList, unsigned int index> struct TypeAt;
        
    template <class Head, class Tail>
    struct TypeAt<Typelist<Head, Tail>, 0>{
        typedef Head Result;
    };

    template <class Head, class Tail, unsigned int i>
    struct TypeAt<Typelist<Head, Tail>, i>{
        typedef typename TypeAt<Tail, i - 1>::Result Result;
    };

    //Finds the type at a given index of no strict in a typelist 
    template <class TList, unsigned int index,
            typename DefaultType = NullType>
    struct TypeAtNonStrict{
        typedef DefaultType Result;
    };
    
    template <class Head, class Tail, typename DefaultType>
    struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType>{
        typedef Head Result;
    };
    
    template <class Head, class Tail, unsigned int i, typename DefaultType>
    struct TypeAtNonStrict<Typelist<Head, Tail>, i, DefaultType>{
        typedef typename 
            TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
    };

    //Finds the index of a type in a typelist
    template <class TList, class T> struct IndexOf;
    
    template <class T> 
    struct IndexOf<NullType, T>{
        enum { value = -1 };
    };
    
    template <class T, class Tail> 
    struct IndexOf<Typelist<T, Tail>, T>{
        enum { value = 0 };
    };
    
    template <class Head, class Tail, class T>
    struct IndexOf<Typelist<Head, Tail>, T>{
    private:
        enum { temp = IndexOf<Tail, T>::value };
    public:
        enum { value = (temp == -1 ? -1 : 1 + temp) };
    };

    // Appends a type or a typelist to another
    template <class TList, class T> struct Append;
        
    template <> struct Append<NullType, NullType>{
        typedef NullType Result;
    };
    
    template <class T> struct Append<NullType, T>{
        typedef TYPELIST_1(T) Result;
    };
    
    template <class Head, class Tail>
    struct Append<NullType, Typelist<Head, Tail> >{
        typedef Typelist<Head, Tail> Result;
    };
    
    template <class Head, class Tail, class T>
    struct Append<Typelist<Head, Tail>, T>{
        typedef Typelist<Head, 
                typename Append<Tail, T>::Result> Result;
    };

    //Erase the first occurence if any, of a type in a typelist.
    template <class TList, class T> struct Erase;
        
    template <class T>
    struct Erase<NullType, T>{
        typedef NullType Result;
    };

    template <class T, class Tail> 
    struct Erase<Typelist<T, Tail>, T>{
        typedef Tail Result;
    };

    template <class Head, class Tail, class T>
    struct Erase<Typelist<Head, Tail>, T>{
        typedef Typelist<Head, 
                typename Erase<Tail, T>::Result> Result;
    };

    template <class TList, class T> struct EraseAll;
    
    template <class T>
    struct EraseAll<NullType, T>{
        typedef NullType Result;
    };
    
    template <class T, class Tail>
    struct EraseAll<Typelist<T, Tail>, T>{
        typedef typename EraseAll<Tail, T>::Result Result;
    };

    template <class Head, class Tail, class T>
    struct EraseAll<Typelist<Head, Tail>, T>{
        typedef Typelist<Head, 
                typename EraseAll<Tail, T>::Result> Result;
    };

    // Remove all duplicate types in a typelist
    template <class TList> struct NoDuplicates;
        
    template <> struct NoDuplicates<NullType>{
        typedef NullType Result;
    };

    template <class Head, class Tail>
    struct NoDuplicates< Typelist<Head, Tail> >{
    private:
        typedef typename NoDuplicates<Tail>::Result L1;
        typedef typename Erase<L1, Head>::Result L2;
    public:
        typedef Typelist<Head, L2> Result;
    };

    // Replaces the first occurence of a type in a typelist, with another type
    template <class TList, class T, class U> struct Replace;

    template <class T, class U>
    struct Replace<NullType, T, U>{
        typedef NullType Result;
    };

    template <class T, class Tail, class U>
    struct Replace<Typelist<T, Tail>, T, U>{
        typedef Typelist<U, Tail> Result;
    };

    template <class Head, class Tail, class T, class U>
    struct Replace<Typelist<Head, Tail>, T, U>{
        typedef Typelist<Head,
                typename Replace<Tail, T, U>::Result> Result;
    };

    template <class TList, class T, class U> struct ReplaceAll;
        
    template <class T, class U>
    struct ReplaceAll<NullType, T, U>{
        typedef NullType Result;
    };
    
    template <class T, class Tail, class U>
    struct ReplaceAll<Typelist<T, Tail>, T, U>{
        typedef Typelist<U, typename ReplaceAll<Tail, T, U>::Result> Result;
    };
    
    template <class Head, class Tail, class T, class U>
    struct ReplaceAll<Typelist<Head, Tail>, T, U>{
        typedef Typelist<Head,
                typename ReplaceAll<Tail, T, U>::Result> Result;
    };

    // Reverses a typelist
    template <class TList> struct Reverse;
        
    template <>
    struct Reverse<NullType>{
        typedef NullType Result;
    };
    
    template <class Head, class Tail>
    struct Reverse< Typelist<Head, Tail> >{
        typedef typename Append<
            typename Reverse<Tail>::Result, Head>::Result Result;
    };

    // Finds the type in a typelist that is the most derived from a given type
    template <class TList, class T> struct MostDerived;
        
    template <class T>
    struct MostDerived<NullType, T>{
        typedef T Result;
    };
    
    template <class Head, class Tail, class T>
    struct MostDerived<Typelist<Head, Tail>, T>{
    private:
        typedef typename MostDerived<Tail, T>::Result Candidate;
    public:
        typedef typename Select<
            SuperSubclass<Candidate,Head>::value,
                Head, Candidate>::Result Result;
    };

    // Arranges the types in a typelist so that the most derived types appear first
    template <class TList> struct DerivedToFront;
        
    template <>
    struct DerivedToFront<NullType>{
        typedef NullType Result;
    };
    
    template <class Head, class Tail>
    struct DerivedToFront< Typelist<Head, Tail> >{
    private:
        typedef typename MostDerived<Tail, Head>::Result
            TheMostDerived;
        typedef typename Replace<Tail,
            TheMostDerived, Head>::Result Temp;
        typedef typename DerivedToFront<Temp>::Result L;
    public:
        typedef Typelist<TheMostDerived, L> Result;
    };
}


#define TYPELIST_1(T1) Typelist<T1, NullType>

#define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3)Typelist<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
    Typelist<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
    Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
    Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
    Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
    Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
    Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
    Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
    Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
    Typelist<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) \
    Typelist<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) \
    Typelist<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) \
    Typelist<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) \
    Typelist<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) \
    Typelist<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) \
    Typelist<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
    Typelist<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define TYPELIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
    Typelist<T1, TYPELIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) >

#define TYPELIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
    Typelist<T1, TYPELIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) >

#define TYPELIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
    Typelist<T1, TYPELIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) >

#define TYPELIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
    Typelist<T1, TYPELIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) >

#define TYPELIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
    Typelist<T1, TYPELIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25) >

#endif