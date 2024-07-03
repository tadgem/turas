#ifndef TURAS_STL_IO_H
#define TURAS_STL_IO_H
#include <fstream>
#include <ostream>
namespace turas
{
    using OStream = std::ostream;
    using IStream = std::istream;
    using OfStream = std::ofstream;
    using IfStream = std::ifstream;

    template<typename T>
    using IStreamBufIterator = std::istreambuf_iterator<T>;

    using Ios = std::ios;
}

#endif