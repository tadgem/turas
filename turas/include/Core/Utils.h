//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/String.h"

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

        operator uint64_t() const { return m_Value; };
    };

    class Utils {
    public:

        static uint64_t Hash(const String& string);

    };
}