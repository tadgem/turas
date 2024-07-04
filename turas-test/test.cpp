#include "test.h"

BEGIN_TESTS()

TEST(
    {
        turas::log::info("hello!");
    }
)

TEST(
    {
        turas::log::info("again!");
    }
)

RUN_TESTS()

