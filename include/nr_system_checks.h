#ifndef NETWORKING_RESEARCH_SYSTEM_CHECKS_H
#define NETWORKING_RESEARCH_SYSTEM_CHECKS_H

#if defined(__clang__)
    #define COMPILER_CLANG 1
#else
    #define COMPILER_CLANG 0
#endif

#if (defined(__GNUC__) || defined(__GNUG__)) && (COMPILER_CLANG == 0)
    #define COMPILER_GCC 1
#else
    #define COMPILER_GCC 0
#endif

#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #define COMPILER_MSVC 0
#endif

#if (COMPILER_CLANG == 0) && (COMPILER_GCC == 0) && (COMPILER_MSVC == 0)
    #define COMPILER_UNKNOWN 1
#else
    #define COMPILER_UNKNOWN 0
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS 1
#else
    #define OS_WINDOWS 0
#endif

#if defined(__linux__)
    #define OS_LINUX 1
#else
    #define OS_LINUX 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
    #define OS_MACOS 1
#else
    #define OS_MACOS 0
#endif

#if (OS_WINDOWS == 0) && (OS_LINUX == 0) && (OS_MACOS == 0)
    #define OS_UNKNOWN 1
#else
    #define OS_UNKNOWN 0
#endif

#if defined(__x86_64__) || defined(_M_X64)
    #define ARCH_X86_64 1
#else
    #define ARCH_X86_64 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
    #define ARCH_ARM64 1
#else
    #define ARCH_ARM64 0
#endif

#if (ARCH_X86_64 == 0) && (ARCH_ARM64 == 0)
    #define ARCH_UNKNOWN 1
#else
    #define ARCH_UNKNOWN 0
#endif

/* Backward-compatible aliases */
#define OS_WIN OS_WINDOWS
#define OS_MAC OS_MACOS
#define CPU_ARCH_X86_64 ARCH_X86_64
#define CPU_ARCH_ARM64 ARCH_ARM64

#if ARCH_X86_64 == 1
    #define CPU_ARCH "x86_64"
#elif ARCH_ARM64 == 1
    #define CPU_ARCH "arm64"
#else
    #define CPU_ARCH "unknown"
#endif

#endif
