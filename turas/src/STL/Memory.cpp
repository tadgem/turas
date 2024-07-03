#include "STL/Memory.h"
#include "STL/HashMap.h"
#include "STL/String.h"
using namespace turas;
static HashMap<String, size_t> s_Allocs {};

#ifdef ENABLED_MEMORY_TRACKER
void *operator new (size_t size, char *file, int line, char *function)
{
    // add tracking code here...
    s_Allocs[String(function)] += size;
    return malloc (size);
}
#endif