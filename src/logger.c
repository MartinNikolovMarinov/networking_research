#include "logger.h"
#include "ansiColors.h"

#include <stdio.h>
#include <time.h>

static const char* level_name(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_TRACE: return "TRACE";
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARN: return "WARN";
        case LOG_LEVEL_ERR: return "ERROR";
        case LOG_LEVEL_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

static const char* level_color(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_TRACE: return ANSI_COLOR_TRACE;
        case LOG_LEVEL_INFO: return ANSI_COLOR_INFO;
        case LOG_LEVEL_WARN: return ANSI_COLOR_WARN;
        case LOG_LEVEL_ERR: return ANSI_COLOR_ERR;
        case LOG_LEVEL_FATAL: return ANSI_COLOR_FATAL;
        default: return ANSI_COLOR_RESET;
    }
}

static void print_arg(FILE* out, LogArg arg) {
    switch (arg.type) {
        case LOG_ARG_CHAR:
            fprintf(out, "%c", arg.data.char_value);
            break;
        case LOG_ARG_I64:
            fprintf(out, "%lld", arg.data.i64_value);
            break;
        case LOG_ARG_U64:
            fprintf(out, "%llu", arg.data.u64_value);
            break;
        case LOG_ARG_DOUBLE:
            fprintf(out, "%g", arg.data.double_value);
            break;
        case LOG_ARG_CSTR:
            fprintf(out, "%s", arg.data.cstr_value != NULL ? arg.data.cstr_value : "(null)");
            break;
        case LOG_ARG_PTR:
            fprintf(out, "%p", arg.data.ptr_value);
            break;
        case LOG_ARG_BOOL:
            fprintf(out, "%s", arg.data.bool_value ? "true" : "false");
            break;
        default:
            fprintf(out, "<invalid>");
            break;
    }
}

static void print_formatted_message(FILE* out, const char* fmt, size_t arg_count, const LogArg* args) {
    size_t arg_index = 0;
    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '{' && fmt[i + 1] == '{') {
            fputc('{', out);
            i++;
            continue;
        }

        if (fmt[i] == '}' && fmt[i + 1] == '}') {
            fputc('}', out);
            i++;
            continue;
        }

        if (fmt[i] == '{' && fmt[i + 1] == '}') {
            if (arg_index < arg_count) {
                print_arg(out, args[arg_index++]);
            } else {
                fputs("{}", out);
            }
            i++;
            continue;
        }

        fputc(fmt[i], out);
    }
}

void log_message(LogLevel level, const char* fmt, size_t arg_count, const LogArg* args) {
    FILE* out = (level >= LOG_LEVEL_ERR) ? stderr : stdout;
    const char* name = level_name(level);
    const char* color = level_color(level);
    time_t now = time(NULL);
    struct tm local;
#if defined(_WIN32)
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif

    fprintf(out, "%s[%02d:%02d:%02d] [%s] ", color, local.tm_hour, local.tm_min, local.tm_sec, name);
    print_formatted_message(out, fmt, arg_count, args);
    fprintf(out, "%s\n", ANSI_COLOR_RESET);
}

LogArg log_arg_char(char value) {
    return (LogArg){ .type = LOG_ARG_CHAR, .data.char_value = value };
}

LogArg log_arg_i64(long long value) {
    return (LogArg){ .type = LOG_ARG_I64, .data.i64_value = value };
}

LogArg log_arg_u64(unsigned long long value) {
    return (LogArg){ .type = LOG_ARG_U64, .data.u64_value = value };
}

LogArg log_arg_double(double value) {
    return (LogArg){ .type = LOG_ARG_DOUBLE, .data.double_value = value };
}

LogArg log_arg_cstr(const char* value) {
    return (LogArg){ .type = LOG_ARG_CSTR, .data.cstr_value = value };
}

LogArg log_arg_ptr(const void* value) {
    return (LogArg){ .type = LOG_ARG_PTR, .data.ptr_value = value };
}

LogArg log_arg_bool(int value) {
    return (LogArg){ .type = LOG_ARG_BOOL, .data.bool_value = value };
}
