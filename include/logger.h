#ifndef NETWORKING_RESEARCH_LOGGER_H
#define NETWORKING_RESEARCH_LOGGER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LogLevel {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR,
    LOG_LEVEL_FATAL,
} LogLevel;

typedef enum LogArgType {
    LOG_ARG_CHAR = 0,
    LOG_ARG_I64,
    LOG_ARG_U64,
    LOG_ARG_DOUBLE,
    LOG_ARG_CSTR,
    LOG_ARG_PTR,
    LOG_ARG_BOOL,
} LogArgType;

typedef struct LogArg {
    LogArgType type;
    union {
        char charValue;
        long long i64Value;
        unsigned long long u64Value;
        double doubleValue;
        const char* cstrValue;
        const void* ptrValue;
        bool boolValue;
    } data;
} LogArg;

void logMessage(LogLevel level, const char* tag, const char* fmt, size_t argCount, const LogArg* args);

void setLoggerMuted(bool muted);
bool isLoggerMuted(void);
bool muteLogTag(const char* tag);
bool unmuteLogTag(const char* tag);
void clearMutedLogTags(void);
bool isLogTagMuted(const char* tag);

LogArg logArgChar(char value);
LogArg logArgI64(long long value);
LogArg logArgU64(unsigned long long value);
LogArg logArgDouble(double value);
LogArg logArgCStr(const char* value);
LogArg logArgPtr(const void* value);
LogArg logArgBool(bool value);

static inline LogArg logArgCharToken(const char* token, char value) {
    (void)token;
    return logArgChar(value);
}

static inline LogArg logArgI64Token(const char* token, long long value) {
    if (token != NULL && token[0] == '\'' && token[1] != '\0') {
        if (token[1] != '\\' && token[2] == '\'' && token[3] == '\0') {
            return logArgChar(token[1]);
        }
        if (token[1] == '\\' && token[3] == '\'' && token[4] == '\0') {
            switch (token[2]) {
                case 'n': return logArgChar('\n');
                case 'r': return logArgChar('\r');
                case 't': return logArgChar('\t');
                case '\\': return logArgChar('\\');
                case '\'': return logArgChar('\'');
                case '0': return logArgChar('\0');
                default: break;
            }
        }
    }
    return logArgI64(value);
}

static inline LogArg logArgU64Token(const char* token, unsigned long long value) {
    (void)token;
    return logArgU64(value);
}

static inline LogArg logArgDoubleToken(const char* token, double value) {
    (void)token;
    return logArgDouble(value);
}

static inline LogArg logArgCStrToken(const char* token, const char* value) {
    (void)token;
    return logArgCStr(value);
}

static inline LogArg logArgPtrToken(const char* token, const void* value) {
    (void)token;
    return logArgPtr(value);
}

static inline LogArg logArgBoolToken(const char* token, bool value) {
    (void)token;
    return logArgBool(value);
}

#define LOG_PP_MAP_1(m, a1) m(a1)
#define LOG_PP_MAP_2(m, a1, a2) m(a1), m(a2)
#define LOG_PP_MAP_3(m, a1, a2, a3) m(a1), m(a2), m(a3)
#define LOG_PP_MAP_4(m, a1, a2, a3, a4) m(a1), m(a2), m(a3), m(a4)
#define LOG_PP_MAP_5(m, a1, a2, a3, a4, a5) m(a1), m(a2), m(a3), m(a4), m(a5)
#define LOG_PP_MAP_6(m, a1, a2, a3, a4, a5, a6) m(a1), m(a2), m(a3), m(a4), m(a5), m(a6)
#define LOG_PP_MAP_7(m, a1, a2, a3, a4, a5, a6, a7) m(a1), m(a2), m(a3), m(a4), m(a5), m(a6), m(a7)
#define LOG_PP_MAP_8(m, a1, a2, a3, a4, a5, a6, a7, a8) m(a1), m(a2), m(a3), m(a4), m(a5), m(a6), m(a7), m(a8)
#define LOG_PP_MAP_9(m, a1, a2, a3, a4, a5, a6, a7, a8, a9) m(a1), m(a2), m(a3), m(a4), m(a5), m(a6), m(a7), m(a8), m(a9)
#define LOG_PP_MAP_10(m, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) m(a1), m(a2), m(a3), m(a4), m(a5), m(a6), m(a7), m(a8), m(a9), m(a10)
#define LOG_PP_GET_MAP(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME
#define LOG_PP_MAP(m, ...) LOG_PP_GET_MAP(__VA_ARGS__, LOG_PP_MAP_10, LOG_PP_MAP_9, LOG_PP_MAP_8, LOG_PP_MAP_7, LOG_PP_MAP_6, LOG_PP_MAP_5, LOG_PP_MAP_4, LOG_PP_MAP_3, LOG_PP_MAP_2, LOG_PP_MAP_1)(m, __VA_ARGS__)

#define LOG_ARG_VALUE(x) _Generic((x), \
    char: logArgCharToken, \
    signed char: logArgI64Token, \
    unsigned char: logArgU64Token, \
    short: logArgI64Token, \
    unsigned short: logArgU64Token, \
    int: logArgI64Token, \
    unsigned int: logArgU64Token, \
    long: logArgI64Token, \
    unsigned long: logArgU64Token, \
    long long: logArgI64Token, \
    unsigned long long: logArgU64Token, \
    float: logArgDoubleToken, \
    double: logArgDoubleToken, \
    long double: logArgDoubleToken, \
    char*: logArgCStrToken, \
    const char*: logArgCStrToken, \
    void*: logArgPtrToken, \
    const void*: logArgPtrToken, \
    _Bool: logArgBoolToken, \
    default: logArgPtrToken \
)(#x, (x))

#define LOG_ARGS_ARRAY(...) ((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) })
#define LOG_ARGS_COUNT(...) ((size_t)(sizeof((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) }) / sizeof(LogArg)))

#define LOG_PP_CAT_(a, b) a##b
#define LOG_PP_CAT(a, b) LOG_PP_CAT_(a, b)
#define LOG_PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define LOG_PP_NARG_(...) LOG_PP_ARG_N(__VA_ARGS__)
#define LOG_PP_NARG(...) LOG_PP_NARG_(_0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define LOG_CALL_0(level, fmt) logMessage((level), NULL, (fmt), 0, NULL)
#define LOG_CALL_VA(level, fmt, ...) logMessage((level), NULL, (fmt), LOG_ARGS_COUNT(__VA_ARGS__), LOG_ARGS_ARRAY(__VA_ARGS__))
#define LOG_CALL_1(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_2(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_3(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_4(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_5(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_6(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_7(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_8(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_9(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)
#define LOG_CALL_10(level, fmt, ...) LOG_CALL_VA((level), (fmt), __VA_ARGS__)

#define LOG_CALL_TAG_0(level, tag, fmt) logMessage((level), (tag), (fmt), 0, NULL)
#define LOG_CALL_TAG_VA(level, tag, fmt, ...) logMessage((level), (tag), (fmt), LOG_ARGS_COUNT(__VA_ARGS__), LOG_ARGS_ARRAY(__VA_ARGS__))
#define LOG_CALL_TAG_1(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_2(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_3(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_4(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_5(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_6(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_7(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_8(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_9(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)
#define LOG_CALL_TAG_10(level, tag, fmt, ...) LOG_CALL_TAG_VA((level), (tag), (fmt), __VA_ARGS__)

#define LOG_CALL(level, fmt, ...) LOG_PP_CAT(LOG_CALL_, LOG_PP_NARG(__VA_ARGS__))((level), (fmt) __VA_OPT__(,) __VA_ARGS__)
#define LOG_CALL_TAG(level, tag, fmt, ...) LOG_PP_CAT(LOG_CALL_TAG_, LOG_PP_NARG(__VA_ARGS__))((level), (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)

#define logTrace(fmt, ...) LOG_CALL(LOG_LEVEL_TRACE, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logInfo(fmt, ...) LOG_CALL(LOG_LEVEL_INFO, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logWarn(fmt, ...) LOG_CALL(LOG_LEVEL_WARN, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logErr(fmt, ...) LOG_CALL(LOG_LEVEL_ERR, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logFatal(fmt, ...) LOG_CALL(LOG_LEVEL_FATAL, (fmt) __VA_OPT__(,) __VA_ARGS__)

#define logTraceTag(tag, fmt, ...) LOG_CALL_TAG(LOG_LEVEL_TRACE, (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logInfoTag(tag, fmt, ...) LOG_CALL_TAG(LOG_LEVEL_INFO, (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logWarnTag(tag, fmt, ...) LOG_CALL_TAG(LOG_LEVEL_WARN, (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logErrTag(tag, fmt, ...) LOG_CALL_TAG(LOG_LEVEL_ERR, (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logFatalTag(tag, fmt, ...) LOG_CALL_TAG(LOG_LEVEL_FATAL, (tag), (fmt) __VA_OPT__(,) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
