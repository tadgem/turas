//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "ThirdParty/ctti/type_id.hpp"

namespace turas {
    template<typename T>
    String GetTypeName() {
        return ctti::type_id<T>().name();
    }

    template<typename T>
    uint64_t GetTypeHash() {
        return ctti::type_id<T>().hash();
    }

    struct HashString {
        HashString() { m_Value = 0; }

        HashString(const String &input);
        HashString(uint64_t value);

        template<typename T>
        HashString() : m_Value(GetTypeHash<T>())
        {
#ifdef TURAS_TRACK_HASHSTRINGS
            Utils::s_OriginalStrings.emplace(*this, GetTypeName<T>());
#endif
        }

        uint64_t m_Value;

        bool operator==(HashString const &rhs) const { return m_Value == rhs.m_Value; }
        bool operator<(const HashString &o) const { return m_Value < o.m_Value;} ;
        operator uint64_t() const { return m_Value; };
    };
}

/* required to hash a container */
template<> struct std::hash<turas::HashString> {
    std::size_t operator()(const turas::HashString& h) const {
        return std::hash<uint64_t>()(h.m_Value) ^ std::hash<uint64_t>()(h.m_Value);
    }
};


namespace turas
{
    class Utils {
    public:
#define TURAS_TRACK_HASHSTRINGS
#ifdef TURAS_TRACK_HASHSTRINGS
inline static HashMap<HashString, String> s_OriginalStrings = {};
#endif
        static uint64_t Hash(const String& string);
    };
}