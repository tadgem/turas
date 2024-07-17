//
// Created by liam_ on 7/4/2024.
//

#include "Core/Utils.h"
#include "Debug/Profile.h"
#include <fstream>

turas::u64 turas::Utils::Hash(const String &string) {
    ZoneScoped;
    u64 ret = 0;
    for (auto &c: string) {
        ret ^= 2305 * c;
    }
    return ret;
}

turas::Vector<turas::u8> turas::Utils::LoadBinaryFromPath(const turas::String &path) {
    ZoneScoped;
    std::ifstream input(path, std::ios::binary);

    std::vector<u8> bytes(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));

    input.close();
    return bytes;
}

turas::String turas::Utils::GetDirectoryFromFilename(const turas::String &fname) {
    size_t pos = fname.find_last_of("\\/");
    return (String::npos == pos)
           ? ""
           : fname.substr(0, pos);
}

turas::Vector<turas::String> turas::Utils::GetFilesInDirectory(const turas::String &path) {
    auto ret = turas::Vector<turas::String>();
    for (const auto &entry: fs::directory_iterator("./" + path)) {
        ret.push_back(entry.path().string());
    }
    return ret;
}

turas::String turas::Utils::GetFilenameFromPath(const turas::String &fname) {
    size_t pos = fname.find_last_of("\\/");
    return (String::npos == pos)
           ? ""
           : fname.substr(pos + 1, fname.size() - 1);
}


turas::HashString::HashString(const turas::String &input) : m_Value(Utils::Hash(input)) {
    ZoneScoped;
#ifdef TURAS_TRACK_HASHSTRINGS
    Utils::s_OriginalStrings.emplace(*this, input);
#endif
}

turas::HashString::HashString(u64 input) : m_Value(input) {
    ZoneScoped;
}
