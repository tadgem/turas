//
// Created by liam_ on 7/4/2024.
//

#ifndef TURAS_ALL_SERIALIZATION_H
#define TURAS_ALL_SERIALIZATION_H

#define EMPTY(...)
#define DEFER(...) __VA_ARGS__ EMPTY()
# define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
# define EXPAND(...) __VA_ARGS__

#define CEREAL_THREAD_SAFE
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>

// Include the polymorphic serialization and registration mechanisms
#include <cereal/types/polymorphic.hpp>

#endif //TURAS_ALL_SERIALIZATION_H
