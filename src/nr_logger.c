#include "nr_logger.h"
#include "nr_ansi_colors.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_MAX_MUTED_TAGS 64
#define LOG_MAX_TAG_LENGTH 63

static bool loggerMuted = false;
static char mutedTags[LOG_MAX_MUTED_TAGS][LOG_MAX_TAG_LENGTH + 1];
static uint64_t mutedTagCount = 0;
static NrLogHandler customLogHandler = NULL;
static void* customLogHandlerUserData = NULL;

static const char* levelName(NrLogLevel level) {
    switch (level) {
        case LOG_LEVEL_TRACE: return "TRACE";
        case LOG_LEVEL_INFO: return "INFO";
        case LOG_LEVEL_WARN: return "WARN";
        case LOG_LEVEL_ERR: return "ERROR";
        case LOG_LEVEL_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

static const char* levelColor(NrLogLevel level) {
    switch (level) {
        case LOG_LEVEL_TRACE: return ANSI_COLOR_TRACE;
        case LOG_LEVEL_INFO: return ANSI_COLOR_INFO;
        case LOG_LEVEL_WARN: return ANSI_COLOR_WARN;
        case LOG_LEVEL_ERR: return ANSI_COLOR_ERR;
        case LOG_LEVEL_FATAL: return ANSI_COLOR_FATAL;
        default: return ANSI_COLOR_RESET;
    }
}

static uint64_t findMutedTagIndex(const char* tag) {
    if (tag == NULL || tag[0] == '\0') {
        return mutedTagCount;
    }

    for (uint64_t i = 0; i < mutedTagCount; i++) {
        if (strcmp(mutedTags[i], tag) == 0) {
            return i;
        }
    }

    return mutedTagCount;
}

static void printArg(FILE* out, NrLogArg arg) {
    switch (arg.type) {
        case LOG_ARG_CHAR:
            fprintf(out, "%c", arg.data.charValue);
            break;
        case LOG_ARG_I64:
            fprintf(out, "%" PRId64, arg.data.i64Value);
            break;
        case LOG_ARG_U64:
            fprintf(out, "%" PRIu64, arg.data.u64Value);
            break;
        case LOG_ARG_DOUBLE:
            fprintf(out, "%g", arg.data.doubleValue);
            break;
        case LOG_ARG_CSTR:
            fprintf(out, "%s", arg.data.cstrValue != NULL ? arg.data.cstrValue : "(nil)");
            break;
        case LOG_ARG_PTR:
            fprintf(out, "%p", arg.data.ptrValue);
            break;
        case LOG_ARG_BOOL:
            fprintf(out, "%s", arg.data.boolValue ? "true" : "false");
            break;
        default:
            fprintf(out, "<invalid>");
            break;
    }
}

static void printFormattedMessage(FILE* out, const char* fmt, uint64_t argCount, const NrLogArg* args) {
    uint64_t argIndex = 0;
    for (uint64_t i = 0; fmt[i] != '\0'; i++) {
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
            if (argIndex < argCount) {
                printArg(out, args[argIndex++]);
            }
            else {
                fputs("{}", out);
            }
            i++;
            continue;
        }

        fputc(fmt[i], out);
    }
}

void nrSetLogHandler(NrLogHandler handler, void* userData) {
    customLogHandler = handler;
    customLogHandlerUserData = userData;
}

void nrResetLogHandler(void) {
    customLogHandler = NULL;
    customLogHandlerUserData = NULL;
}

void nrSetLoggerMuted(bool muted) {
    loggerMuted = muted;
}

bool nrIsLoggerMuted(void) {
    return loggerMuted;
}

bool nrMuteLogTag(const char* tag) {
    if (tag == NULL || tag[0] == '\0') {
        return false;
    }

    if (findMutedTagIndex(tag) < mutedTagCount) {
        return true;
    }

    if (mutedTagCount >= LOG_MAX_MUTED_TAGS) {
        return false;
    }

    strncpy(mutedTags[mutedTagCount], tag, LOG_MAX_TAG_LENGTH);
    mutedTags[mutedTagCount][LOG_MAX_TAG_LENGTH] = '\0';
    mutedTagCount++;
    return true;
}

bool nrUnmuteLogTag(const char* tag) {
    uint64_t index = findMutedTagIndex(tag);
    if (index >= mutedTagCount) {
        return false;
    }

    for (uint64_t i = index; i + 1 < mutedTagCount; i++) {
        memcpy(mutedTags[i], mutedTags[i + 1], sizeof(mutedTags[i]));
    }

    mutedTagCount--;
    mutedTags[mutedTagCount][0] = '\0';
    return true;
}

void nrClearMutedLogTags(void) {
    mutedTagCount = 0;
}

bool nrIsLogTagMuted(const char* tag) {
    return findMutedTagIndex(tag) < mutedTagCount;
}

void nrLogMessage(NrLogLevel level, const char* tag, const char* fmt, uint64_t argCount, const NrLogArg* args) {
    if (loggerMuted) {
        return;
    }

    if (nrIsLogTagMuted(tag)) {
        return;
    }

    if (customLogHandler != NULL) {
        customLogHandler(level, tag, fmt, argCount, args, customLogHandlerUserData);
        return;
    }

    FILE* out = (level >= LOG_LEVEL_ERR) ? stderr : stdout;
    const char* name = levelName(level);
    const char* color = levelColor(level);
    const char* safeFmt = (fmt != NULL) ? fmt : "";
    time_t now = time(NULL);
    struct tm localTime;
#if defined(_WIN32)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    fprintf(out, "%s[%02d:%02d:%02d] [%s]", color, localTime.tm_hour, localTime.tm_min, localTime.tm_sec, name);
    if (tag != NULL && tag[0] != '\0') {
        fprintf(out, " [%s]", tag);
    }
    fputc(' ', out);
    printFormattedMessage(out, safeFmt, argCount, args);
    fprintf(out, "%s\n", ANSI_COLOR_RESET);
}

NrLogArg nrLogArgChar(char value) {
    return (NrLogArg){ .type = LOG_ARG_CHAR, .data.charValue = value };
}

NrLogArg nrLogArgI64(int64_t value) {
    return (NrLogArg){ .type = LOG_ARG_I64, .data.i64Value = value };
}

NrLogArg nrLogArgU64(uint64_t value) {
    return (NrLogArg){ .type = LOG_ARG_U64, .data.u64Value = value };
}

NrLogArg nrLogArgDouble(double value) {
    return (NrLogArg){ .type = LOG_ARG_DOUBLE, .data.doubleValue = value };
}

NrLogArg nrLogArgCStr(const char* value) {
    return (NrLogArg){ .type = LOG_ARG_CSTR, .data.cstrValue = value };
}

NrLogArg nrLogArgPtr(const void* value) {
    return (NrLogArg){ .type = LOG_ARG_PTR, .data.ptrValue = value };
}

NrLogArg nrLogArgBool(bool value) {
    return (NrLogArg){ .type = LOG_ARG_BOOL, .data.boolValue = value };
}
