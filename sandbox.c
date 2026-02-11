#include "logger.h"

int main(void) {
    void* t = NULL;

    logTrace("trace startup");
    logInfo("char={} double={} string={}", (char)'a', 3.2, "testing");
    logWarn("port {} is close to limit {}", 65000, 65535);
    logErr("socket {} failed to connect", "127.0.0.1:8080");
    logFatal("fatal code={} message={} void_ptr={}", 42, "demo", t);
    return 0;
}
