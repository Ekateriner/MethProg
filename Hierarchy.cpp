#include <iostream>

class Nulltype {};

template <typename ...Args>
struct TypeList
{
    using Head = Nulltype;
    using Tail = Nulltype;
};

template <typename H, typename ...T>
struct TypeList<H, T...>
{
    using Head = H;
    using Tail = TypeList<T...>;
};

typedef TypeList<> EmptyTypeList;

template <typename H>
struct TypeList<H>
{
    using Head = H;
    using Tail = EmptyTypeList;
};

//...................................................................//
//............................LECTURE...............................//
//...................................................................//

template <class TList, template <class> class Unit>
struct GenScatterHierarchy;

template <class ...TList, template <class> class Unit>
struct GenScatterHierarchy<TypeList<TList...>, Unit> :
    public Unit<typename TypeList<TList...>::Head>,
    public GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>
{
    using TL = TypeList<TList...>;
    using LeftBase = Unit<typename TypeList<TList...>::Head>;
    using RightBase = GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

template <class T, template <class> class Unit>
struct GenScatterHierarchy<TypeList<T>, Unit> :
    public Unit<T>
{
    using LeftBase = T;
    using RightBase = Nulltype;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

template <template <class> class Unit>
struct GenScatterHierarchy<TypeList<Nulltype>, Unit>
{
    using LeftBase = Nulltype;
    using RightBase = Nulltype;
    template <typename T> struct Rebind
    {
        typedef Unit<T> Result;
    };
};

template<class TList, template <class T, class Base> class Unit, class Root = Nulltype>
struct GenLinearHierarchy;

template<class ...TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList...>, Unit, Root> :
    public Unit< typename TypeList<TList...>::Head, GenLinearHierarchy<typename TypeList<TList...>::Tail, Unit, Root>>
{

};

template<class TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList>, Unit, Root> :
    public Unit<TList, Root>
{

};

//...................................................................//
//............................HOMEWORK...............................//
//...................................................................//

template <size_t value>
struct Power3 
{
    static const size_t res = 3 * Power3<value-1>::res;
};

template <>
struct Power3<0> 
{
    static const size_t res = 1;
};

template <size_t ind, typename ...T>
struct TypeAt {
    using res = Nulltype;
};

template <size_t ind, typename ...T>
struct TypeAt<ind, TypeList<T...>>
{
    using res = typename TypeAt<ind - 1, typename TypeList<T...>::Tail>::res;
};

template <size_t ind>
struct TypeAt<ind, Nulltype>
{
    using res = Nulltype;
};

template <typename ...T>
struct TypeAt<0, TypeList<T...>>
{
    using res = typename TypeList<T...>::Head;
};

template <size_t ind, typename ...T>
struct TailAt {
    using res = Nulltype;
};

template <size_t ind, typename ...T>
struct TailAt<ind, TypeList<T...>>
{
    using res = typename TailAt<ind - 1, typename TypeList<T...>::Tail>::res;
};

template <typename ...T>
struct TailAt<0, TypeList<T...>>
{
    using res = TypeList<T...>;
};

template <size_t ind>
struct TailAt<ind, Nulltype>
{
    using res = Nulltype;
};

template<class TList, template <class T, class Base> class Unit, size_t lay, size_t pos, bool odd, class Root = Nulltype>
struct GenHelper;

template<class ...TList, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<TList...>, Unit, lay, pos, true, Root>:
    public Unit<typename TypeAt<0, TypeList<TList...>>::res, \
                GenHelper<typename TailAt<Power3<lay / 2 + 1>::res, TypeList<TList...>>::res, \
                          Unit, lay + 1, 3*pos, false, Root>>,
    public Unit<typename TypeAt<1, TypeList<TList...>>::res, \
                GenHelper<typename TailAt<Power3<lay / 2 + 1>::res + 1, TypeList<TList...>>::res, \
                          Unit, lay + 1, 3*pos + 1, false, Root>>,
    public Unit<typename TypeAt<2, TypeList<TList...>>::res, \
                GenHelper<typename TailAt<Power3<lay / 2 + 1>::res + 2, TypeList<TList...>>::res, \
                          Unit, lay + 1, 3*pos + 2, false, Root>>
{};

template<class T1, class T2, class T3, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<T1, T2, T3>, Unit, lay, pos, true, Root> :
    public Unit<T1, Root> ,
    public Unit<T2, Root> ,
    public Unit<T3, Root>
{};

template<class T1, class T2, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<T1, T2>, Unit, lay, pos, true, Root> :
    public Unit<T1, Root>,
    public Unit<T2, Root>
{};

template<class T1, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<T1, Unit, lay, pos, true, Root> :
    public Unit<T1, Root>
{};

template<template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<Nulltype>, Unit, lay, pos, true, Root>
{};

template<class ...TList, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<TList...>, Unit, lay, pos, false, Root> :
    public Unit<typename TypeList<TList...>::Head, \
                GenHelper<typename TailAt<2*pos + Power3<(lay + 1)/ 2>::res, TypeList<TList...>>::res, \
                          Unit, lay + 1, pos, true, Root>>
{};

template<class TList, template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TList, Unit, lay, pos, false, Root> :
    public Unit<TList, Root>
{};

template<template <class T, class Base> class Unit, size_t lay, size_t pos, class Root>
struct GenHelper<TypeList<Nulltype>, Unit, lay, pos, false, Root> {};

template<class TList, template <class T, class Base> class Unit, class Root = Nulltype>
struct GenCustomHierarchy;

template<class ...TList, template <class T, class Base> class Unit, class Root>
struct GenCustomHierarchy<TypeList<TList...>, Unit, Root> :
    public Unit<typename TypeList<TList...>::Head, GenHelper<typename TypeList<TList...>::Tail, Unit, 0, 0, true, Root>>
{};

template<typename T, typename Base>
struct Handler:
    public Base 
{
    T _val;
};

/*template<typename T>
struct Handler<T, Nulltype>
{
    T _val;
};*/


int main() {
    GenCustomHierarchy<TypeList<char, int, float, double, int, float, double, char, size_t>, Handler> hier;

    return 0;
}