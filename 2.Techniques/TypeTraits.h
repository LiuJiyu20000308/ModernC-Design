#ifndef TYPETRAITS_H
#define TYPETRAITS_H

#include "Typelist.h"

template <int v>
struct Int2Type
{
    enum { value = v };
};

template <class T>
struct Type2Type{
    typedef T OriginalType;
};

template <bool flag, typename T, typename U>
struct Select{
    typedef T Result;
};

template <typename T, typename U>
struct Select<false,T,U>{
    typedef U Result;
};

//================================================================
//Offers a means to integrate nonstandard built-in type.
template <typename T>
struct IsCustomUnsignedInt{
    enum { value = 0 };
}; 

template <typename T>
struct IsCustomSignedInt{
    enum { value = 0 };
};

template <typename T>
struct IsCustomFloat{
    enum { value = 0 };
}; 
//=============================================================
template <class T, class U>
class Conversion{
    typedef char Small;
    class Big{ char dummy[2]; };
    static Small Test(U);
    static Big Test(...);
    static T MakeT():
public:
    enum { exists = sizeof(Test(MakeT())) == sizeof(Small) };
    enum { sameType = false; }
};

template <class T>
class Conversion<T,T>{
public:
    enum { exists = 1, sameType = 1 };
};

//T is void or U can converts to T.
//add const modifiers to ensure we are always on the safe side.
#define SUPERSUBCLASS(T, U) \
    (Conversion<const volatile U*, const volatile T*>::exists && \
    !Conversion<const volatile T*, const volatile void*>::sameType)


//================================================================

class NullType {};

struct EmptyType {};

namespace TypeTraitsHelper{
    typedef TYPELIST_4(unsigned char, unsigned short int,
        unsigned int, unsigned long int) StdUnsignedInts;
    typedef TYPELIST_4(signed char, short int,
        int, long int) StdSignedInts;
    typedef TYPELIST_3(bool, char, wchar_t) StdOtherInts;
    typedef TYPELIST_3(float, double, long double) StdFloats;

    template <class U> struct AddReference{
        typedef U & Result;
    };

    template <class U> struct AddReference<U &>{
        typedef U & Result;
    };

    template <> struct AddReference<void>{
        typedef NullType Result;
    };
}

template <typename T>
class TypeTraits{
private:
    template <class U> struct PointerTraits{
        enum { result = false };
        typedef NullType PointeeType;
    };

    template <class U> struct PointerTraits<U*>{
        enum { result = true };
        typedef U PointeeType;
    };

    template <class U> struct ReferenceTraits{
        enum { result = false };
        typedef U ReferredType;
    };
        
    template <class U> struct ReferenceTraits<U&>{
        enum { result = true };
        typedef U ReferredType;
    };

    template <class U> struct PToMTraits{
        enum { result = false };
    };
    
    template <class U, class V> 
    struct PToMTraits<U V::*>{
        enum { result = true };
    };

    template <class U> struct UnConst{
        typedef U Result;
        enum { isConst = 0 };
    };
    
    template <class U> struct UnConst<const U>{
        typedef U Result;
        enum { isConst = 1 };
    };

    template <class U> struct UnVolatile{
        typedef U Result;
        enum { isVolatile = 0 };
    };
    
    template <class U> struct UnVolatile<volatile U>{
        typedef U Result;
        enum { isVolatile = 1 };
    };

public:
    enum { isPointer = PointerTraits<T>::result };
    typedef typename PointerTraits<T>::PointeeType PointeeType;

    enum { isReference = ReferenceTraits<T>::result };
    typedef typename ReferenceTraits<T>::ReferredType ReferredType;
    
    enum { isMemberPointer = PToMTraits<T>::result };

    enum { isConst = UnConst<T>::isConst };
    typedef typename UnConst<T>::Result NonConstType;
    
    enum { isVolatile = UnVolatile<T>::isVolatile };
    typedef typename UnVolatile<T>::Result NonVolatileType;
    typedef typename UnVolatile<typename UnConst<T>::Result>::Result UnqualifiedType;

    enum { isStdUnsignedInt = 
        TL::IndexOf<Private::StdUnsignedInts, T>::value >= 0 };
    enum { isStdSignedInt = 
        TL::IndexOf<Private::StdSignedInts, T>::value >= 0 };
    enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
        TL::IndexOf<Private::StdOtherInts, T>::value >= 0 };
    enum { isStdFloat = TL::IndexOf<Private::StdFloats, T>::value >= 0 };

    enum { isStdArith = isStdIntegral || isStdFloat };
    enum { isStdFundamental = isStdArith || isStdFloat || 
        Conversion<T, void>::sameType };

    enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value };
    enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value };
    enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };

    enum { isFloat = isStdFloat || IsCustomFloat<T>::value };
    enum { isArith = isIntegral || isFloat };
    enum { isFundamental = isStdFundamental || isArith || isFloat };

    
};


#endif