//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "ThirdParty/ctti/type_id.hpp"
#include "Core/Types.h"

namespace turas {
    template<typename T>
    String GetTypeName() {
        return ctti::type_id<T>().name();
    }

    template<typename T>
    u64 GetTypeHash() {
        return ctti::type_id<T>().hash();
    }

    struct HashString {
        HashString() { m_Value = 0; }

        HashString(const String &input);
        HashString(u64 value);

        template<typename T>
        HashString() : m_Value(GetTypeHash<T>())
        {
#ifdef TURAS_TRACK_HASHSTRINGS
            Utils::s_OriginalStrings.emplace(*this, GetTypeName<T>());
#endif
        }

        u64 m_Value;

        bool operator==(HashString const &rhs) const { return m_Value == rhs.m_Value; }
        bool operator<(const HashString &o) const { return m_Value < o.m_Value;} ;
        operator u64() const { return m_Value; };
    };
}

/* required to hash a container */
template<> struct std::hash<turas::HashString> {
    std::size_t operator()(const turas::HashString& h) const {
        return std::hash<turas::u64>()(h.m_Value) ^ std::hash<turas::u64>()(h.m_Value);
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
        static u64 Hash(const String& string);

        static Vector<u8> LoadBinaryFromPath(const String& path);
    };
}