#pragma once

#include "core_init.h" // IWYU pragma: keep

#include "common_types.h"

namespace sp {

void reportOsErr(OsErr& osError, const char* filePath, i32 lineNumber);

} // namespace sp
