#ifndef TURAS_STL_HASHSET_H
#define TURAS_STL_HASHSET_H
#include <unordered_set>
namespace turas
{
    template<typename T1>
    using HashSet = std::unordered_set<T1>;
}

#endif