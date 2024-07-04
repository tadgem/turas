#include "STL/Memory.h"
#include "Core/Engine.h"
#include "STL/HashMap.h"
#include "STL/String.h"

int main(int argc, char** argv)
{
    turas::Engine app;
    app.Init();

    app.Shutdown();
    return 0;
}