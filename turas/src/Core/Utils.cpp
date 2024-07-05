//
// Created by liam_ on 7/4/2024.
//

#include "Core/Utils.h"


uint64_t turas::Utils::Hash(const String &string) {
    uint64_t ret = 0;
    for(auto& c : string)
    {
        ret ^= 2305 * c;
    }
    return ret;
}

turas::HashString::HashString(const turas::String &input) : m_Value(Utils::Hash(input))
{
#ifdef TURAS_TRACK_HASHSTRINGS
    Utils::s_OriginalStrings.emplace(*this, input);
#endif
}

turas::HashString::HashString(uint64_t input) : m_Value(input)
{

}
