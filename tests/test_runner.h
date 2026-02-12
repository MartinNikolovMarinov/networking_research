#ifndef NR_TEST_RUNNER_H
#define NR_TEST_RUNNER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NR_TEST_RUNNER_MAX_TESTS_PER_GROUP 255
#define NR_TEST_RUNNER_MAX_GROUPS 64

typedef struct NrTestRunParams NrTestRunParams;
typedef struct NrTestGroupRunParams NrTestGroupRunParams;
typedef struct NrTestCreateInfo NrTestCreateInfo;
typedef struct NrTestGroupCreateInfo NrTestGroupCreateInfo;
typedef struct NrTestGroup NrTestGroup;
typedef struct NrTestRunner NrTestRunner;

typedef int32_t (*NrTestFunction)(NrTestRunParams* params);
typedef void (*NrBeforeAllTestsFunction)(const NrTestGroupRunParams* params);
typedef void (*NrAfterAllTestsFunction)(const NrTestGroupRunParams* params);
typedef void (*NrBeforeEachTestFunction)(NrTestRunParams* params);
typedef void (*NrAfterEachTestFunction)(NrTestRunParams* params);

struct NrTestRunParams {
    const char* name;
    const void* userData;
};

struct NrTestGroupRunParams {
    const char* groupName;
    int32_t testsCount;
};

struct NrTestCreateInfo {
    const char* name;
    NrTestFunction testFunction;
    bool only;
    bool skip;
    const void* userData;
};

struct NrTestGroupCreateInfo {
    const char* name;
    bool groupOnly;
    bool groupSkip;
    NrBeforeAllTestsFunction beforeAll;
    NrAfterAllTestsFunction afterAll;
    NrBeforeEachTestFunction beforeEach;
    NrAfterEachTestFunction afterEach;
};

typedef struct NrTest {
    int32_t testNumber;
    bool only;
    bool skip;
    NrTestRunParams testRunParams;
    NrTestFunction testFunction;
} NrTest;

struct NrTestGroup {
    bool groupOnly;
    bool groupSkip;
    const char* name;
    NrBeforeAllTestsFunction beforeAll;
    NrAfterAllTestsFunction afterAll;
    NrBeforeEachTestFunction beforeEach;
    NrAfterEachTestFunction afterEach;
    NrTest tests[NR_TEST_RUNNER_MAX_TESTS_PER_GROUP];
    uint32_t testsCount;
};

struct NrTestRunner {
    bool useAnsiColors;
    NrTestGroup testGroups[NR_TEST_RUNNER_MAX_GROUPS];
    uint32_t testGroupCount;
};

void nrTestRunnerInit(NrTestRunner* runner, bool useAnsiColors);
NrTestGroup* nrTestRunnerAddTestGroup(NrTestRunner* runner, const NrTestGroupCreateInfo* info);
int32_t nrTestRunnerRunAllTestGroups(NrTestRunner* runner);

bool nrTestGroupAddTest(NrTestGroup* group, const NrTestCreateInfo* info);

#ifdef __cplusplus
}
#endif

#endif
