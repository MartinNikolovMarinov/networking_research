#ifndef NETWORKING_RESEARCH_COMPILER_MACROS_H
#define NETWORKING_RESEARCH_COMPILER_MACROS_H

#include "nr_system_checks.h"

#define PRAGMA_QUOTED(x) _Pragma(#x)

#if COMPILER_MSVC == 1
    #define PRAGMA_WARNING_PUSH __pragma(warning(push))
    #define PRAGMA_WARNING_POP __pragma(warning(pop))
    #define DISABLE_MSVC_WARNING(w) __pragma(warning(disable : w))
    #define PRAGMA_COMPILER_MESSAGE(x) __pragma(message(#x))
    #define PRAGMA_WARNING_SUPPRESS_ALL __pragma(warning(push, 0))
#elif COMPILER_GCC == 1
    #define PRAGMA_WARNING_PUSH PRAGMA_QUOTED(GCC diagnostic push)
    #define PRAGMA_WARNING_POP PRAGMA_QUOTED(GCC diagnostic pop)
    #define DISABLE_GCC_AND_CLANG_WARNING(w) PRAGMA_QUOTED(GCC diagnostic ignored #w)
    #define DISABLE_GCC_WARNING(w) PRAGMA_QUOTED(GCC diagnostic ignored #w)
    #define PRAGMA_COMPILER_MESSAGE(x) PRAGMA_QUOTED(message #x)
    #define PRAGMA_WARNING_SUPPRESS_ALL                     \
        PRAGMA_QUOTED(GCC diagnostic push)                 \
        PRAGMA_QUOTED(GCC diagnostic ignored "-Wall")      \
        PRAGMA_QUOTED(GCC diagnostic ignored "-Wextra")    \
        PRAGMA_QUOTED(GCC diagnostic ignored "-Wpedantic")
#elif COMPILER_CLANG == 1
    #define PRAGMA_WARNING_PUSH PRAGMA_QUOTED(clang diagnostic push)
    #define PRAGMA_WARNING_POP PRAGMA_QUOTED(clang diagnostic pop)
    #define DISABLE_GCC_AND_CLANG_WARNING(w) PRAGMA_QUOTED(clang diagnostic ignored #w)
    #define DISABLE_CLANG_WARNING(w) PRAGMA_QUOTED(clang diagnostic ignored #w)
    #define PRAGMA_COMPILER_MESSAGE(x) PRAGMA_QUOTED(message #x)
    #define PRAGMA_WARNING_SUPPRESS_ALL PRAGMA_QUOTED(clang diagnostic ignored "-Weverything")
#endif

#ifndef PRAGMA_WARNING_PUSH
    #define PRAGMA_WARNING_PUSH
#endif

#ifndef PRAGMA_WARNING_POP
    #define PRAGMA_WARNING_POP
#endif

#ifndef DISABLE_MSVC_WARNING
    #define DISABLE_MSVC_WARNING(...)
#endif

#ifndef DISABLE_GCC_AND_CLANG_WARNING
    #define DISABLE_GCC_AND_CLANG_WARNING(...)
#endif

#ifndef DISABLE_GCC_WARNING
    #define DISABLE_GCC_WARNING(...)
#endif

#ifndef DISABLE_CLANG_WARNING
    #define DISABLE_CLANG_WARNING(...)
#endif

#ifndef PRAGMA_COMPILER_MESSAGE
    #define PRAGMA_COMPILER_MESSAGE(...)
#endif

#ifndef PRAGMA_WARNING_SUPPRESS_ALL
    #define PRAGMA_WARNING_SUPPRESS_ALL
#endif

#endif
