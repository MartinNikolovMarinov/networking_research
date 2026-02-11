#ifndef NETWORKING_RESEARCH_LOGGER_H
#define NETWORKING_RESEARCH_LOGGER_H

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
        char char_value;
        long long i64_value;
        unsigned long long u64_value;
        double double_value;
        const char* cstr_value;
        const void* ptr_value;
        int bool_value;
    } data;
} LogArg;

void log_message(LogLevel level, const char* fmt, size_t arg_count, const LogArg* args);

LogArg log_arg_char(char value);
LogArg log_arg_i64(long long value);
LogArg log_arg_u64(unsigned long long value);
LogArg log_arg_double(double value);
LogArg log_arg_cstr(const char* value);
LogArg log_arg_ptr(const void* value);
LogArg log_arg_bool(int value);

static inline LogArg log_arg_char_token(const char* token, char value) {
    (void)token;
    return log_arg_char(value);
}

static inline LogArg log_arg_i64_token(const char* token, long long value) {
    if (token != NULL && token[0] == '\'' && token[1] != '\0') {
        if (token[1] != '\\' && token[2] == '\'' && token[3] == '\0') {
            return log_arg_char(token[1]);
        }
        if (token[1] == '\\' && token[3] == '\'' && token[4] == '\0') {
            switch (token[2]) {
                case 'n': return log_arg_char('\n');
                case 'r': return log_arg_char('\r');
                case 't': return log_arg_char('\t');
                case '\\': return log_arg_char('\\');
                case '\'': return log_arg_char('\'');
                case '0': return log_arg_char('\0');
                default: break;
            }
        }
    }
    return log_arg_i64(value);
}

static inline LogArg log_arg_u64_token(const char* token, unsigned long long value) {
    (void)token;
    return log_arg_u64(value);
}

static inline LogArg log_arg_double_token(const char* token, double value) {
    (void)token;
    return log_arg_double(value);
}

static inline LogArg log_arg_cstr_token(const char* token, const char* value) {
    (void)token;
    return log_arg_cstr(value);
}

static inline LogArg log_arg_ptr_token(const char* token, const void* value) {
    (void)token;
    return log_arg_ptr(value);
}

static inline LogArg log_arg_bool_token(const char* token, int value) {
    (void)token;
    return log_arg_bool(value);
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
    char: log_arg_char_token, \
    signed char: log_arg_i64_token, \
    unsigned char: log_arg_u64_token, \
    short: log_arg_i64_token, \
    unsigned short: log_arg_u64_token, \
    int: log_arg_i64_token, \
    unsigned int: log_arg_u64_token, \
    long: log_arg_i64_token, \
    unsigned long: log_arg_u64_token, \
    long long: log_arg_i64_token, \
    unsigned long long: log_arg_u64_token, \
    float: log_arg_double_token, \
    double: log_arg_double_token, \
    long double: log_arg_double_token, \
    char*: log_arg_cstr_token, \
    const char*: log_arg_cstr_token, \
    void*: log_arg_ptr_token, \
    const void*: log_arg_ptr_token, \
    _Bool: log_arg_bool_token, \
    default: log_arg_ptr_token \
)(#x, (x))

#define LOG_ARGS_ARRAY(...) ((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) })
#define LOG_ARGS_COUNT(...) ((size_t)(sizeof((LogArg[]){ LOG_PP_MAP(LOG_ARG_VALUE, __VA_ARGS__) }) / sizeof(LogArg)))

#define LOG_PP_CAT_(a, b) a##b
#define LOG_PP_CAT(a, b) LOG_PP_CAT_(a, b)
#define LOG_PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define LOG_PP_NARG_(...) LOG_PP_ARG_N(__VA_ARGS__)
#define LOG_PP_NARG(...) LOG_PP_NARG_(_0 __VA_OPT__(,) __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define LOG_CALL_0(level, fmt) log_message((level), (fmt), 0, NULL)
#define LOG_CALL_VA(level, fmt, ...) log_message((level), (fmt), LOG_ARGS_COUNT(__VA_ARGS__), LOG_ARGS_ARRAY(__VA_ARGS__))
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

#define LOG_CALL(level, fmt, ...) LOG_PP_CAT(LOG_CALL_, LOG_PP_NARG(__VA_ARGS__))((level), (fmt) __VA_OPT__(,) __VA_ARGS__)

#define logTrace(fmt, ...) LOG_CALL(LOG_LEVEL_TRACE, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logInfo(fmt, ...) LOG_CALL(LOG_LEVEL_INFO, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logWarn(fmt, ...) LOG_CALL(LOG_LEVEL_WARN, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logErr(fmt, ...) LOG_CALL(LOG_LEVEL_ERR, (fmt) __VA_OPT__(,) __VA_ARGS__)
#define logFatal(fmt, ...) LOG_CALL(LOG_LEVEL_FATAL, (fmt) __VA_OPT__(,) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
