#include "t-index.h"

int main(void) {
    TestRunner runner;
    testRunnerInit(&runner, true);
    registerAllTests(&runner);
    return testRunnerRunAllTestGroups(&runner);
}
