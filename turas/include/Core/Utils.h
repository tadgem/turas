//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"

namespace typehash_internal
{
    static const unsigned int FRONT_SIZE = sizeof("typehash_internal::GetTypeNameHelper<") - 1u;
    static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

    template <typename T>
    struct GetTypeNameHelper {
        static turas::String GetTypeName() {
            static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
            turas::String typeString = turas::String(__FUNCTION__ + FRONT_SIZE, size - 1u);
            return typeString;
        }
    };
}

namespace turas {
    template <typename T>
    String GetTypeName(void) {
        return typehash_internal::GetTypeNameHelper<T>::GetTypeName();
    }

    struct HashString {
        HashString() { m_Value = 0; }

        HashString(const String &input);
        HashString(uint64_t value);

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