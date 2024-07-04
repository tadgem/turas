//
// Created by liam_ on 7/4/2024.
//

#include "Core/Utils.h"


uint32_t turas::Utils::Hash(const String &string) {
    uint32_t ret = 0;
    for(auto& c : string)
    {
        ret ^= 2305 * c;
    }
    return ret;
}
