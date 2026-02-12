#include "t-index.h"
#include "nr_bits.h"
#include "nr_logger.h"

#include <string.h>

static void discardLogHandler(
    NrLogLevel level,
    const char* tag,
    const char* fmt,
    uint64_t argCount,
    const NrLogArg* args,
    void* userData
) {
    (void)level;
    (void)tag;
    (void)fmt;
    (void)argCount;
    (void)args;
    (void)userData;
}

static int32_t tSetAndGetBitRoundTrip(TestRunParams* params) {
    (void)params;

    uint32_t value = 0u;
    value = nrSetBitAt(value, 5u, 1u);
    if (value != 32u) {
        return 1;
    }

    uint8_t bitSet = nrGetBitAt(value, 5u);
    if (bitSet != 1u) {
        return 2;
    }

    value = nrSetBitAt(value, 5u, 0u);
    if (value != 0u) {
        return 3;
    }

    uint8_t bitCleared = nrGetBitAt(value, 5u);
    if (bitCleared != 0u) {
        return 4;
    }

    return 0;
}

static int32_t tBitBoundsBehavior(TestRunParams* params) {
    (void)params;

    uint32_t value = 42u;
    uint32_t sameValue = nrSetBitAt(value, 32u, 1u);
    if (sameValue != value) {
        return 1;
    }

    uint8_t outOfRangeBit = nrGetBitAt(0xFFFFFFFFu, 32u);
    if (outOfRangeBit != 0u) {
        return 2;
    }

    return 0;
}

static int32_t tReverseCharArray(TestRunParams* params) {
    (void)params;

    char text[] = "abcd";
    nrReverseCharArr(text, 4);
    if (strcmp(text, "dcba") != 0) {
        return 1;
    }

    char single[] = "z";
    nrReverseCharArr(single, 1);
    if (strcmp(single, "z") != 0) {
        return 2;
    }

    return 0;
}

static int32_t tIsPowerOf2(TestRunParams* params) {
    (void)params;

    if (nrIsPowerOf2(0u)) {
        return 1;
    }
    if (!nrIsPowerOf2(1u)) {
        return 2;
    }
    if (!nrIsPowerOf2(64u)) {
        return 3;
    }
    if (nrIsPowerOf2(3u)) {
        return 4;
    }
    if (nrIsPowerOf2(12u)) {
        return 5;
    }

    return 0;
}

static int32_t tAlign(TestRunParams* params) {
    (void)params;

    uint32_t v1 = nrAlign(8u, 4u);
    if (v1 != 8u) {
        return 1;
    }

    uint32_t v2 = nrAlign(9u, 4u);
    if (v2 != 12u) {
        return 2;
    }

    uint32_t v3 = nrAlign(1u, 8u);
    if (v3 != 8u) {
        return 3;
    }

    return 0;
}

static int32_t tLogInfoToBinarySmoke(TestRunParams* params) {
    (void)params;

    nrSetLogHandler(discardLogHandler, NULL);

    nrLogInfoToBinary(0u);
    nrLogInfoToBinary(13u);
    nrResetLogHandler();
    return 0;
}

void registerTBitsTests(TestRunner* runner) {
    TestGroupCreateInfo groupInfo = {
        .name = "Bit helpers",
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
        .name = "Set/get bit round trip",
        .testFunction = tSetAndGetBitRoundTrip,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t1);

    TestCreateInfo t2 = {
        .name = "Bit helpers handle out-of-range positions",
        .testFunction = tBitBoundsBehavior,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t2);

    TestCreateInfo t3 = {
        .name = "Reverse char array",
        .testFunction = tReverseCharArray,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t3);

    TestCreateInfo t4 = {
        .name = "Power-of-two detection",
        .testFunction = tIsPowerOf2,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t4);

    TestCreateInfo t5 = {
        .name = "Align rounds up to next boundary",
        .testFunction = tAlign,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t5);

    TestCreateInfo t6 = {
        .name = "Binary logger helper smoke",
        .testFunction = tLogInfoToBinarySmoke,
        .only = false,
        .skip = false,
        .userData = NULL,
    };
    (void)testGroupAddTest(group, &t6);
}
