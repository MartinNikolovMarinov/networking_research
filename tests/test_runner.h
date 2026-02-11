#ifndef NETWORKING_RESEARCH_TEST_RUNNER_H
#define NETWORKING_RESEARCH_TEST_RUNNER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_RUNNER_MAX_TESTS_PER_GROUP 255
#define TEST_RUNNER_MAX_GROUPS 64

typedef struct TestRunParams TestRunParams;
typedef struct TestGroupRunParams TestGroupRunParams;
typedef struct TestCreateInfo TestCreateInfo;
typedef struct TestGroupCreateInfo TestGroupCreateInfo;
typedef struct TestGroup TestGroup;
typedef struct TestRunner TestRunner;

typedef int (*TestFunction)(TestRunParams* params);
typedef void (*BeforeAllTestsFunction)(const TestGroupRunParams* params);
typedef void (*AfterAllTestsFunction)(const TestGroupRunParams* params);
typedef void (*BeforeEachTestFunction)(TestRunParams* params);
typedef void (*AfterEachTestFunction)(TestRunParams* params);

struct TestRunParams {
    const char* name;
    const void* userData;
};

struct TestGroupRunParams {
    const char* groupName;
    int testsCount;
};

struct TestCreateInfo {
    const char* name;
    TestFunction testFunction;
    bool only;
    bool skip;
    const void* userData;
};

struct TestGroupCreateInfo {
    const char* name;
    bool groupOnly;
    bool groupSkip;
    BeforeAllTestsFunction beforeAll;
    AfterAllTestsFunction afterAll;
    BeforeEachTestFunction beforeEach;
    AfterEachTestFunction afterEach;
};

typedef struct Test {
    int testNumber;
    bool only;
    bool skip;
    TestRunParams testRunParams;
    TestFunction testFunction;
} Test;

struct TestGroup {
    bool groupOnly;
    bool groupSkip;
    const char* name;
    BeforeAllTestsFunction beforeAll;
    AfterAllTestsFunction afterAll;
    BeforeEachTestFunction beforeEach;
    AfterEachTestFunction afterEach;
    Test tests[TEST_RUNNER_MAX_TESTS_PER_GROUP];
    size_t testsCount;
};

struct TestRunner {
    bool useAnsiColors;
    TestGroup testGroups[TEST_RUNNER_MAX_GROUPS];
    size_t testGroupCount;
};

void testRunnerInit(TestRunner* runner, bool useAnsiColors);
TestGroup* testRunnerAddTestGroup(TestRunner* runner, const TestGroupCreateInfo* info);
int testRunnerRunAllTestGroups(TestRunner* runner);

bool testGroupAddTest(TestGroup* group, const TestCreateInfo* info);

#ifdef __cplusplus
}
#endif

#endif
