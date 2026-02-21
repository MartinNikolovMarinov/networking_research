#include "core_init.h"

#include <errno.h>

// #define OS_ERR_LIST(X) \
//     X(ctx, EPERM, "Operation Not Permitted")

// CORE_ENUM_DECLARE(OsErr, i32, OS_ERR_LIST);

// #undef OS_ERR_LIST

int main() {
    coreInit(core::LogLevel::L_DEBUG);
    defer { coreShutdown(); };

    AssertFmt(false, "Crash please {}", 5.412);

    return 0;
}
