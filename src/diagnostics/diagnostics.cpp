#include "diagnostics/diagnostics.h"

#include "common_types.h"

namespace sp {

namespace {

core::AtomicU64 g_lastDiagnosedId = 1;

} // namespace

void reportOsErr(OsErr& osError, const char* filePath, i32 lineNumber) {
    if (osError.diagnoseId == 0) {
        osError.diagnoseId = g_lastDiagnosedId.fetch_add(1);
    }

    u64 diagnoseId = osError.diagnoseId;
    i32 code = +osError.code;
    const char* codeDesc = enumOsErrCodeToCstr(osError.code);
    i32 sysErrno = osError.sysErrno;

    auto pltErr = core::PltErrCode(sysErrno);
    char pltErrorDesc[core::MAX_SYSTEM_ERR_MSG_SIZE];
    core::pltErrorDescribe(pltErr, pltErrorDesc);

    u64 ts = core::getUnixTimestampNowMs();
    char tsISO8601[32];
    core::timeToIsoUtc8601CstrCached(ts, tsISO8601, 32);

    constexpr const char* logFmt = "OS Error Occured \n  diagnose_id = {}\n  ts = {}\n  sys_errno = {} ({})\n  code = {} ({})\n  file = {}:{}";

    logErr(logFmt,
        diagnoseId,
        tsISO8601,
        sysErrno, pltErrorDesc,
        code, codeDesc,
        filePath, lineNumber);
}

} // namespace sp
