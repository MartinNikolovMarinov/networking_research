#include "tests_index.h"
#include "nr_logger.h"

static int tLoggerGlobalMute(TestRunParams* params) {
    (void)params;

    setLoggerMuted(false);
    if (isLoggerMuted()) {
        return 1;
    }

    setLoggerMuted(true);
    if (!isLoggerMuted()) {
        return 2;
    }

    setLoggerMuted(false);
    if (isLoggerMuted()) {
        return 3;
    }

    return 0;
}

static int tLoggerTagMute(TestRunParams* params) {
    (void)params;

    clearMutedLogTags();

    if (isLogTagMuted("net")) {
        return 1;
    }

    if (!muteLogTag("net")) {
        return 2;
    }

    if (!isLogTagMuted("net")) {
        return 3;
    }

    if (!unmuteLogTag("net")) {
        return 4;
    }

    if (isLogTagMuted("net")) {
        return 5;
    }

    return 0;
}

static int tLoggerSmoke(TestRunParams* params) {
    (void)params;

    clearMutedLogTags();
    setLoggerMuted(false);

    LogArg infoArgs[3];
    infoArgs[0] = logArgChar('a');
    infoArgs[1] = logArgCStr("ok");
    infoArgs[2] = logArgPtr(NULL);
    logMessage(LOG_LEVEL_INFO, "logger", "smoke test char={} str={} ptr={}", 3, infoArgs);

    LogArg warnArgs[1];
    warnArgs[0] = logArgI64(42);
    logMessage(LOG_LEVEL_WARN, NULL, "smoke warning {}", 1, warnArgs);

    return 0;
}

void registerTLoggerTests(TestRunner* runner) {
    TestGroupCreateInfo groupInfo = {
        .name = "Logger behavior",
        .groupOnly = false,
        .groupSkip = false,
        .beforeAll = NULL,
        .afterAll = NULL,
        .beforeEach = NULL,
        .afterEach = NULL,
    };

    TestGroup* group = testRunnerAddTestGroup(runner, &groupInfo);
    if (group == NULL) {
        return;
    }

    TestCreateInfo t1 = {
        .name = "Logger can be muted globally",
        .testFunction = tLoggerGlobalMute,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t1);

    TestCreateInfo t2 = {
        .name = "Logger can mute and unmute tags",
        .testFunction = tLoggerTagMute,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t2);

    TestCreateInfo t3 = {
        .name = "Logger prints smoke messages",
        .testFunction = tLoggerSmoke,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t3);
}
