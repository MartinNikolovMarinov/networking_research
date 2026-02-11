#include "nr_logger.h"

int main(void) {
    void* ptr = NULL;

    logTraceTag("boot", "trace startup");
    logInfoTag("net", "char={} double={} string={} ptr={}", 'a', 3.2, "testing", ptr);

    muteLogTag("net");
    logInfoTag("net", "this line is muted by tag");
    logWarnTag("ui", "ui warning still visible");

    setLoggerMuted(true);
    logErrTag("db", "this line is muted globally");

    setLoggerMuted(false);
    unmuteLogTag("net");
    logInfoTag("net", "tag {} restored", "net");
    logFatal("fatal code={} message={}", 42, "demo");

    return 0;
}
