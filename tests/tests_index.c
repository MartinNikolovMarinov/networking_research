#include "tests_index.h"

#include <stddef.h>

void registerAllTests(TestRunner* runner) {
    if (runner == NULL) {
        return;
    }

    registerTLoggerTests(runner);
}
