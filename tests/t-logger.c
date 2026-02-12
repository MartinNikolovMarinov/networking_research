#include "t-index.h"
#include "nr_logger.h"

typedef struct LogCapture {
    uint64_t calls;
    NrLogLevel lastLevel;
} LogCapture;

static void captureLogHandler(
    NrLogLevel level,
    const char* tag,
    const char* fmt,
    uint64_t argCount,
    const NrLogArg* args,
    void* userData
) {
    (void)tag;
    (void)fmt;
    (void)argCount;
    (void)args;

    LogCapture* capture = (LogCapture*)userData;
    if (capture == NULL) {
        return;
    }

    capture->calls++;
    capture->lastLevel = level;
}

static int32_t tLoggerGlobalMute(TestRunParams* params) {
    (void)params;

    nrSetLoggerMuted(false);
    if (nrIsLoggerMuted()) {
        return 1;
    }

    nrSetLoggerMuted(true);
    if (!nrIsLoggerMuted()) {
        return 2;
    }

    nrSetLoggerMuted(false);
    if (nrIsLoggerMuted()) {
        return 3;
    }

    return 0;
}

static int32_t tLoggerTagMute(TestRunParams* params) {
    (void)params;

    nrClearMutedLogTags();

    if (nrIsLogTagMuted("net")) {
        return 1;
    }

    if (!nrMuteLogTag("net")) {
        return 2;
    }

    if (!nrIsLogTagMuted("net")) {
        return 3;
    }

    if (!nrUnmuteLogTag("net")) {
        return 4;
    }

    if (nrIsLogTagMuted("net")) {
        return 5;
    }

    return 0;
}

static int32_t tLoggerSmoke(TestRunParams* params) {
    (void)params;

    nrClearMutedLogTags();
    nrSetLoggerMuted(false);
    nrResetLogHandler();

    LogCapture capture = {
        .calls = 0U,
        .lastLevel = LOG_LEVEL_TRACE,
    };
    nrSetLogHandler(captureLogHandler, &capture);

    NrLogArg infoArgs[3];
    infoArgs[0] = nrLogArgChar('a');
    infoArgs[1] = nrLogArgCStr("ok");
    infoArgs[2] = nrLogArgPtr(NULL);
    nrLogMessage(LOG_LEVEL_INFO, "logger", "smoke test char={} str={} ptr={}", 3U, infoArgs);

    NrLogArg warnArgs[1];
    warnArgs[0] = nrLogArgI64(42);
    nrLogMessage(LOG_LEVEL_WARN, NULL, "smoke warning {}", 1U, warnArgs);

    nrResetLogHandler();

    if (capture.calls != 2U) {
        return 1;
    }
    if (capture.lastLevel != LOG_LEVEL_WARN) {
        return 2;
    }

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
