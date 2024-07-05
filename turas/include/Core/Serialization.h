//
// Created by liam_ on 7/4/2024.
//

#ifndef TURAS_ALL_SERIALIZATION_H
#define TURAS_ALL_SERIALIZATION_H

#define EMPTY(...)
#define DEFER(...) __VA_ARGS__ EMPTY()
# define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
# define EXPAND(...) __VA_ARGS__
#define CEREAL_THREAD_SAFE 1
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/polymorphic.hpp"

namespace turas
{
    using BinaryOutputArchive = cereal::PortableBinaryOutputArchive;
    using BinaryInputArchive = cereal::PortableBinaryInputArchive;
}
#endif //TURAS_ALL_SERIALIZATION_H
