#include "tests_index.h"

void registerAllTests(TestRunner* runner) {
    if (runner == NULL) {
        return;
    }

    registerTLoggerTests(runner);
}
