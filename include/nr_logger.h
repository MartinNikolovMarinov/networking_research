#ifndef NETWORKING_RESEARCH_LOGGER_H
#define NETWORKING_RESEARCH_LOGGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum NrLogLevel {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR,
    LOG_LEVEL_FATAL,
} NrLogLevel;

typedef enum NrLogArgType {
    LOG_ARG_CHAR = 0,
    LOG_ARG_I64,
    LOG_ARG_U64,
    LOG_ARG_DOUBLE,
    LOG_ARG_CSTR,
    LOG_ARG_PTR,
    LOG_ARG_BOOL,
} NrLogArgType;

typedef struct NrLogArg {
    NrLogArgType type;
    union {
        char charValue;
        int64_t i64Value;
        uint64_t u64Value;
        double doubleValue;
        const char* cstrValue;
        const void* ptrValue;
        bool boolValue;
    } data;
} NrLogArg;

void nrLogMessage(NrLogLevel level, const char* tag, const char* fmt, uint64_t argCount, const NrLogArg* args);

void nrSetLoggerMuted(bool muted);
bool nrIsLoggerMuted(void);
bool nrMuteLogTag(const char* tag);
bool nrUnmuteLogTag(const char* tag);
void nrClearMutedLogTags(void);
bool nrIsLogTagMuted(const char* tag);

NrLogArg nrLogArgChar(char value);
NrLogArg nrLogArgI64(int64_t value);
NrLogArg nrLogArgU64(uint64_t value);
NrLogArg nrLogArgDouble(double value);
NrLogArg nrLogArgCStr(const char* value);
NrLogArg nrLogArgPtr(const void* value);
NrLogArg nrLogArgBool(bool value);

static inline NrLogArg nrLogArgCharToken(const char* token, char value) {
    (void)token;
    return nrLogArgChar(value);
}

static inline NrLogArg nrLogArgI64Token(const char* token, int64_t value) {
    if (token != NULL && token[0] == '\'' && token[1] != '\0') {
        if (token[1] != '\\' && token[2] == '\'' && token[3] == '\0') {
            return nrLogArgChar(token[1]);
        }
        if (token[1] == '\\' && token[3] == '\'' && token[4] == '\0') {
            switch (token[2]) {
                case 'n': return nrLogArgChar('\n');
                case 'r': return nrLogArgChar('\r');
                case 't': return nrLogArgChar('\t');
                case '\\': return nrLogArgChar('\\');
                case '\'': return nrLogArgChar('\'');
                case '0': return nrLogArgChar('\0');
                default: break;
            }
        }
    }
    return nrLogArgI64(value);
}

static inline NrLogArg nrLogArgU64Token(const char* token, uint64_t value) {
    (void)token;
    return nrLogArgU64(value);
}

static inline NrLogArg nrLogArgDoubleToken(const char* token, double value) {
    (void)token;
    return nrLogArgDouble(value);
}

static inline NrLogArg nrLogArgCStrToken(const char* token, const char* value) {
    (void)token;
    return nrLogArgCStr(value);
}

static inline NrLogArg nrLogArgPtrToken(const char* token, const void* value) {
    (void)token;
    return nrLogArgPtr(value);
}

static inline NrLogArg nrLogArgBoolToken(const char* token, bool value) {
    (void)token;
    return nrLogArgBool(value);
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
    char: nrLogArgCharToken, \
    int8_t: nrLogArgI64Token, \
    uint8_t: nrLogArgU64Token, \
    int16_t: nrLogArgI64Token, \
    uint16_t: nrLogArgU64Token, \
    int32_t: nrLogArgI64Token, \
    uint32_t: nrLogArgU64Token, \
    int64_t: nrLogArgI64Token, \
    uint64_t: nrLogArgU64Token, \
    float: nrLogArgDoubleToken, \
    double: nrLogArgDoubleToken, \
    long double: nrLogArgDoubleToken, \
    char*: nrLogArgCStrToken, \
    const char*: nrLogArgCStrToken, \
    void*: nrLogArgPtrToken, \
    const void*: nrLogArgPtrToken, \
    _Bool: nrLogArgBoolToken, \
    default: nrLogArgPtrToken \
)(#x, (x))

#define LOG_ARGS_ARRAY(...) ((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) })
#define LOG_ARGS_COUNT(...) ((uint64_t)(sizeof((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) }) / sizeof(LogArg)))

#define LOG_PP_CAT_(a, b) a##b
#define LOG_PP_CAT(a, b) LOG_PP_CAT_(a, b)
#define LOG_PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define LOG_PP_NARG_(...) LOG_PP_ARG_N(__VA_ARGS__)
#define LOG_PP_NARG(...) LOG_PP_NARG_(_0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define LOG_CALL_0(level, fmt) nrLogMessage((level), NULL, (fmt), 0, NULL)
#define LOG_CALL_VA(level, fmt, ...) nrLogMessage((level), NULL, (fmt), LOG_ARGS_COUNT(__VA_ARGS__), LOG_ARGS_ARRAY(__VA_ARGS__))
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

#define LOG_CALL_TAG_0(level, tag, fmt) nrLogMessage((level), (tag), (fmt), 0, NULL)
#define LOG_CALL_TAG_VA(level, tag, fmt, ...) nrLogMessage((level), (tag), (fmt), LOG_ARGS_COUNT(__VA_ARGS__), LOG_ARGS_ARRAY(__VA_ARGS__))
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
