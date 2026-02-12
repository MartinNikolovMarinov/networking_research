#include "t-index.h"

#include <stddef.h>

void registerAllTests(TestRunner* runner) {
    if (runner == NULL) {
        return;
    }

    registerTBitsTests(runner);
    registerTLoggerTests(runner);
}
