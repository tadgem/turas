#ifndef TURAS_STL_FUNCTIONAL_H
#define TURAS_STL_FUNCTIONAL_H

#include <functional>
namespace turas
{
	template<typename T>
	using Function = std::function<T>;

	using Procedure = std::function<void()>;
}
#endif