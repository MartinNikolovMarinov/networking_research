#include "test_runner.h"
#include "nr_ansi_colors.h"
#include "nr_time.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TEST_TIME_BUFFER_SIZE 64

static const char* passedOrFailedStr(bool passed, bool useAnsiColors) {
    if (useAnsiColors) {
        return passed ? ANSI_COLOR_INFO "PASSED" ANSI_COLOR_RESET : ANSI_COLOR_ERR "FAILED" ANSI_COLOR_RESET;
    }
    return passed ? "PASSED" : "FAILED";
}

static const char* skippedStr(bool useAnsiColors) {
    return useAnsiColors ? ANSI_COLOR_WARN "SKIPPED" ANSI_COLOR_RESET : "SKIPPED";
}

static const char* elapsedTimeToStr(uint64_t elapsedNs, char* buffer, size_t bufferSize) {
    if (elapsedNs < 1000ULL) {
        (void)snprintf(buffer, bufferSize, "%lluns", (unsigned long long)elapsedNs);
        return buffer;
    }

    if (elapsedNs < 1000000ULL) {
        (void)snprintf(buffer, bufferSize, "%.3fus", (double)elapsedNs / 1000.0);
        return buffer;
    }

    if (elapsedNs < 1000000000ULL) {
        (void)snprintf(buffer, bufferSize, "%.3fms", (double)elapsedNs / 1000000.0);
        return buffer;
    }

    (void)snprintf(buffer, bufferSize, "%.3fs", (double)elapsedNs / 1000000000.0);
    return buffer;
}

static void skippedTestGroup(const TestRunner* runner, const char* groupName) {
    printf("[SUITE %s] %s\n", skippedStr(runner->useAnsiColors), groupName != NULL ? groupName : "(unnamed)");
}

static void beginTestGroup(const TestGroup* group) {
    printf("[SUITE RUNNING] %s\n", group->name != NULL ? group->name : "(unnamed)");
}

static void endTestGroup(
    const TestRunner* runner,
    const TestGroup* group,
    int returnCode,
    uint64_t startTimeNs
) {
    char elapsedBuffer[TEST_TIME_BUFFER_SIZE];
    uint64_t elapsedNs = getMonotonicTime() - startTimeNs;

    printf("[SUITE %s] %s [ time: %s ]\n",
        passedOrFailedStr(returnCode == 0, runner->useAnsiColors),
        group->name != NULL ? group->name : "(unnamed)",
        elapsedTimeToStr(elapsedNs, elapsedBuffer, sizeof(elapsedBuffer)));
}

static void skippedTest(const TestRunner* runner, const Test* test) {
    printf("\t[TEST %s] %s\n", skippedStr(runner->useAnsiColors), test->testRunParams.name);
}

static uint64_t beginTest(const Test* test) {
    printf("\t[TEST # %d RUNNING] %s\n", test->testNumber, test->testRunParams.name);
    return getMonotonicTime();
}

static void endTest(
    const TestRunner* runner,
    const Test* test,
    int returnCode,
    uint64_t startTimeNs
) {
    char elapsedBuffer[TEST_TIME_BUFFER_SIZE];
    uint64_t elapsedNs = getMonotonicTime() - startTimeNs;

    printf("\t[TEST # %d %s] %s [ time: %s ]\n",
        test->testNumber,
        passedOrFailedStr(returnCode == 0, runner->useAnsiColors),
        test->testRunParams.name,
        elapsedTimeToStr(elapsedNs, elapsedBuffer, sizeof(elapsedBuffer)));
}

static bool groupHasOnly(const TestGroup* group) {
    for (size_t i = 0; i < group->testsCount; i++) {
        if (group->tests[i].only && !group->tests[i].skip) {
            return true;
        }
    }
    return false;
}

static int runTestGroup(
    const TestRunner* runner,
    TestGroup* group,
    int* testCounter,
    int* skippedTests,
    int* passedTests,
    int* failedTests
) {
    bool hasOnly = groupHasOnly(group);
    TestGroupRunParams groupParams = {
        .groupName = group->name,
        .testsCount = (int)group->testsCount,
    };

    if (group->beforeAll != NULL) {
        group->beforeAll(&groupParams);
    }

    for (size_t i = 0; i < group->testsCount; i++) {
        Test* test = &group->tests[i];

        if (test->testFunction == NULL) {
            *failedTests += 1;
            printf("\t[TEST # %d %s] %s [ reason: missing test function ]\n",
                *testCounter,
                passedOrFailedStr(false, runner->useAnsiColors),
                test->testRunParams.name != NULL ? test->testRunParams.name : "(unnamed)");
            if (group->afterAll != NULL) {
                group->afterAll(&groupParams);
            }
            return -1;
        }

        if ((hasOnly && !test->only) || test->skip) {
            *skippedTests += 1;
            skippedTest(runner, test);
            continue;
        }

        test->testNumber = *testCounter;
        *testCounter += 1;

        if (group->beforeEach != NULL) {
            group->beforeEach(&test->testRunParams);
        }

        uint64_t startTimeNs = beginTest(test);
        int returnCode = test->testFunction(&test->testRunParams);
        endTest(runner, test, returnCode, startTimeNs);

        if (group->afterEach != NULL) {
            group->afterEach(&test->testRunParams);
        }

        if (returnCode == 0) {
            *passedTests += 1;
        } else {
            *failedTests += 1;
            if (group->afterAll != NULL) {
                group->afterAll(&groupParams);
            }
            return returnCode;
        }
    }

    if (group->afterAll != NULL) {
        group->afterAll(&groupParams);
    }

    return 0;
}

void testRunnerInit(TestRunner* runner, bool useAnsiColors) {
    if (runner == NULL) {
        return;
    }

    memset(runner, 0, sizeof(*runner));
    runner->useAnsiColors = useAnsiColors;
}

TestGroup* testRunnerAddTestGroup(TestRunner* runner, const TestGroupCreateInfo* info) {
    if (runner == NULL || info == NULL) {
        return NULL;
    }

    if (runner->testGroupCount >= TEST_RUNNER_MAX_GROUPS) {
        return NULL;
    }

    TestGroup* group = &runner->testGroups[runner->testGroupCount];
    memset(group, 0, sizeof(*group));

    group->groupOnly = info->groupOnly;
    group->groupSkip = info->groupSkip;
    group->name = info->name;
    group->beforeAll = info->beforeAll;
    group->afterAll = info->afterAll;
    group->beforeEach = info->beforeEach;
    group->afterEach = info->afterEach;

    runner->testGroupCount += 1;
    return group;
}

bool testGroupAddTest(TestGroup* group, const TestCreateInfo* info) {
    if (group == NULL || info == NULL || info->testFunction == NULL) {
        return false;
    }

    if (group->testsCount >= TEST_RUNNER_MAX_TESTS_PER_GROUP) {
        return false;
    }

    Test* test = &group->tests[group->testsCount];
    memset(test, 0, sizeof(*test));

    test->only = info->only;
    test->skip = info->skip;
    test->testRunParams.name = info->name;
    test->testRunParams.userData = info->userData;
    test->testFunction = info->testFunction;

    group->testsCount += 1;
    return true;
}

int testRunnerRunAllTestGroups(TestRunner* runner) {
    if (runner == NULL) {
        return -1;
    }

    bool hasOnlyGroup = false;
    for (size_t i = 0; i < runner->testGroupCount; i++) {
        if (runner->testGroups[i].groupOnly && !runner->testGroups[i].groupSkip) {
            hasOnlyGroup = true;
            break;
        }
    }

    int testCounter = 1;
    int passedTests = 0;
    int failedTests = 0;
    int skippedTests = 0;
    int skippedGroups = 0;

    for (size_t i = 0; i < runner->testGroupCount; i++) {
        TestGroup* group = &runner->testGroups[i];

        if ((hasOnlyGroup && !group->groupOnly) || group->groupSkip) {
            skippedTestGroup(runner, group->name);
            skippedGroups += 1;
            continue;
        }

        uint64_t startTimeNs = getMonotonicTime();
        beginTestGroup(group);

        int result = runTestGroup(runner, group, &testCounter, &skippedTests, &passedTests, &failedTests);
        endTestGroup(runner, group, result, startTimeNs);

        if (result != 0) {
            printf("\n%s %d; %s %d; %s %d; %s %d\n",
                runner->useAnsiColors ? ANSI_COLOR_INFO "Passed Tests:" ANSI_COLOR_RESET : "Passed Tests:",
                passedTests,
                runner->useAnsiColors ? ANSI_COLOR_ERR "Failed Tests:" ANSI_COLOR_RESET : "Failed Tests:",
                failedTests,
                runner->useAnsiColors ? ANSI_COLOR_WARN "Skipped Tests:" ANSI_COLOR_RESET : "Skipped Tests:",
                skippedTests,
                runner->useAnsiColors ? ANSI_COLOR_WARN "Skipped Groups:" ANSI_COLOR_RESET : "Skipped Groups:",
                skippedGroups);
            return result;
        }
    }

    printf("\n%s %d",
        runner->useAnsiColors ? ANSI_COLOR_INFO "Passed Tests:" ANSI_COLOR_RESET : "Passed Tests:",
        passedTests);

    if (failedTests > 0) {
        printf("; %s %d",
            runner->useAnsiColors ? ANSI_COLOR_ERR "Failed Tests:" ANSI_COLOR_RESET : "Failed Tests:",
            failedTests);
    }

    if (skippedGroups > 0) {
        printf("; %s %d",
            runner->useAnsiColors ? ANSI_COLOR_WARN "Skipped Groups:" ANSI_COLOR_RESET : "Skipped Groups:",
            skippedGroups);
    }

    if (skippedTests > 0) {
        printf("; %s %d",
            runner->useAnsiColors ? ANSI_COLOR_WARN "Skipped Tests:" ANSI_COLOR_RESET : "Skipped Tests:",
            skippedTests);
    }

    printf("\n");
    return 0;
}
