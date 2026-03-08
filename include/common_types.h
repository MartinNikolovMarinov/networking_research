#pragma once

#include "core_init.h" // IWYU pragma: keep

#include <errno.h>

namespace sp {

//======================================================================================================================
// OS ERROR
//======================================================================================================================

#define OS_ERR_CODE_LIST(X, ctx) \
    X(ctx, Undefined,       "Undefined")                        \
                                                                \
    X(ctx, NotPermitted,    "Operation Not Permitted")          \
    X(ctx, ResourceLimit,   "Resource Limit Reached")           \
    X(ctx, TryAgain,        "Try Again")                        \
    X(ctx, Timedout,        "Timed Out")                        \
    X(ctx, UsageError,      "Invaid API Usage")                 \
                                                                \
    X(ctx, SENTINEL,        "Sentinel")

CORE_ENUM_DECLARE(OsErrCode, i32, OS_ERR_CODE_LIST)
CORE_ENUM_DECLARE_INT_OPS(sp::OsErrCode, i32)

// Serves as a grouping for system errors that might be actionable.
#define ERRNO_TO_OS_ERR_CODE_LIST(X) \
    X(EPERM,                sp::OsErrCode::NotPermitted)    \
    X(EACCES,               sp::OsErrCode::NotPermitted)    \
    X(EROFS,                sp::OsErrCode::NotPermitted)    \
    X(ENOKEY,               sp::OsErrCode::NotPermitted)    \
    X(EKEYEXPIRED,          sp::OsErrCode::NotPermitted)    \
    X(EKEYREVOKED,          sp::OsErrCode::NotPermitted)    \
    X(EKEYREJECTED,         sp::OsErrCode::NotPermitted)    \
    X(ERFKILL,              sp::OsErrCode::NotPermitted)    \
                                                            \
    X(ENOMEM,               sp::OsErrCode::ResourceLimit)   \
    X(ENOSPC,               sp::OsErrCode::ResourceLimit)   \
    X(ENFILE,               sp::OsErrCode::ResourceLimit)   \
    X(EMFILE,               sp::OsErrCode::ResourceLimit)   \
    X(EUSERS,               sp::OsErrCode::ResourceLimit)   \
    X(ENOBUFS,              sp::OsErrCode::ResourceLimit)   \
    X(EDQUOT,               sp::OsErrCode::ResourceLimit)   \
    X(ENOLCK,               sp::OsErrCode::ResourceLimit)   \
    X(ENOSR,                sp::OsErrCode::ResourceLimit)   \
    X(EXFULL,               sp::OsErrCode::ResourceLimit)   \
    X(E2BIG,                sp::OsErrCode::ResourceLimit)   \
    X(EFBIG,                sp::OsErrCode::ResourceLimit)   \
    X(EOVERFLOW,            sp::OsErrCode::ResourceLimit)   \
                                                            \
    X(EAGAIN,               sp::OsErrCode::TryAgain)        \
    X(EBUSY,                sp::OsErrCode::TryAgain)        \
    X(ERESTART,             sp::OsErrCode::TryAgain)        \
    X(EINTR,                sp::OsErrCode::TryAgain)        \
                                                            \
    X(ETIMEDOUT,            sp::OsErrCode::Timedout)        \
                                                            \
    X(EINVAL,               sp::OsErrCode::UsageError)      \
    X(EFAULT,               sp::OsErrCode::UsageError)      \
    X(ENOEXEC,              sp::OsErrCode::UsageError)      \
    X(EDOM,                 sp::OsErrCode::UsageError)      \
    X(ERANGE,               sp::OsErrCode::UsageError)      \
    X(EILSEQ,               sp::OsErrCode::UsageError)      \
    X(EBADMSG,              sp::OsErrCode::UsageError)      \
    X(EBADR,                sp::OsErrCode::UsageError)      \
    X(EBADRQC,              sp::OsErrCode::UsageError)      \
    X(EBADSLT,              sp::OsErrCode::UsageError)      \
    X(EBADE,                sp::OsErrCode::UsageError)

CORE_DECLARE_CONVERT_FN(enumCreateOsErrCode, decltype(errno), sp::OsErrCode,  sp::OsErrCode::Undefined, ERRNO_TO_OS_ERR_CODE_LIST)

#undef OS_ERR_CODE_LIST
#undef ERRNO_TO_OS_ERR_CODE_LIST

struct OsErr {
    u64 diagnoseId = 0;
    OsErrCode code = OsErrCode::Undefined;
    i32 sysErrno = 0;
};

#define CREATE_OS_ERR(osErrno)                                                                                \
([__capturedErrno = (osErrno)]() {                                                                            \
    auto CORE_NAME_CONCAT(__errnoCpy, __LINE__) = __capturedErrno;                                            \
    sp::OsErr CORE_NAME_CONCAT(__err, __LINE__) = {};                                                         \
    CORE_NAME_CONCAT(__err, __LINE__).code = sp::enumCreateOsErrCode(CORE_NAME_CONCAT(__errnoCpy, __LINE__)); \
    CORE_NAME_CONCAT(__err, __LINE__).sysErrno = CORE_NAME_CONCAT(__errnoCpy, __LINE__);                      \
    return CORE_NAME_CONCAT(__err, __LINE__);                                                                 \
}())

#define RETURN_OS_ERR(osErrno)                                                    \
do {                                                                              \
    sp::OsErr CORE_NAME_CONCAT(__err, __LINE__) = CREATE_OS_ERR(osErrno);         \
    sp::reportOsErr(CORE_NAME_CONCAT(__err, __LINE__), __FILE__, __LINE__);       \
    return core::unexpected(std::move(CORE_NAME_CONCAT(__err, __LINE__)));        \
} while (0)

#define TRY_SYSCALL(expr)        \
do {                             \
    if (!(expr)) {               \
        RETURN_OS_ERR(errno);    \
    }                            \
} while (0)

//======================================================================================================================
// Try Expected Expressions
//======================================================================================================================

#define TRYX(expr)                                                                     \
do {                                                                                   \
    using __ExprT = decltype((expr));                                                  \
    static_assert(!std::is_lvalue_reference_v<__ExprT>, "TRY needs an rvalue");        \
                                                                                       \
    auto&& CORE_NAME_CONCAT(__expr, __LINE__) = (expr);                                \
    if (CORE_NAME_CONCAT(__expr, __LINE__).hasErr()) {                                 \
        sp::reportOsErr(CORE_NAME_CONCAT(__expr, __LINE__).err(), __FILE__, __LINE__); \
        return core::unexpected(std::move(CORE_NAME_CONCAT(__expr, __LINE__).err()));  \
    }                                                                                  \
} while (0)

#define TRYV(expr)                                                                     \
({                                                                                     \
    using __ExprT = decltype((expr));                                                  \
    static_assert(!std::is_lvalue_reference_v<__ExprT>, "TRYV needs an rvalue");       \
                                                                                       \
    auto&& CORE_NAME_CONCAT(__expr, __LINE__) = (expr);                                \
    if (CORE_NAME_CONCAT(__expr, __LINE__).hasErr()) {                                 \
        sp::reportOsErr(CORE_NAME_CONCAT(__expr, __LINE__).err(), __FILE__, __LINE__); \
        return core::unexpected(std::move(CORE_NAME_CONCAT(__expr, __LINE__).err()));  \
    }                                                                                  \
                                                                                       \
    std::move(CORE_NAME_CONCAT(__expr, __LINE__).value());                             \
})

#define TRYX_NODIAG(expr)                                                               \
do {                                                                                   \
    using __ExprT = decltype((expr));                                                  \
    static_assert(!std::is_lvalue_reference_v<__ExprT>, "TRY needs an rvalue");        \
                                                                                       \
    auto&& CORE_NAME_CONCAT(__expr, __LINE__) = (expr);                                \
    if (CORE_NAME_CONCAT(__expr, __LINE__).hasErr()) {                                 \
        return core::unexpected(std::move(CORE_NAME_CONCAT(__expr, __LINE__).err()));  \
    }                                                                                  \
} while (0)

#define TRYV_NODIAG(expr)                                                              \
({                                                                                     \
    using __ExprT = decltype((expr));                                                  \
    static_assert(!std::is_lvalue_reference_v<__ExprT>, "TRYV needs an rvalue");       \
                                                                                       \
    auto&& CORE_NAME_CONCAT(__expr, __LINE__) = (expr);                                \
    if (CORE_NAME_CONCAT(__expr, __LINE__).hasErr()) {                                 \
        return core::unexpected(std::move(CORE_NAME_CONCAT(__expr, __LINE__).err()));  \
    }                                                                                  \
                                                                                       \
    std::move(CORE_NAME_CONCAT(__expr, __LINE__).value());                             \
})

//======================================================================================================================
// Systems
//======================================================================================================================

#define SYSTEM_ID_LIST(X, ctx)                      \
    X(ctx, Undefined,       "Undefined")            \
                                                    \
    X(ctx, SENTINEL,        "Sentinel")

CORE_ENUM_DECLARE(SystemId, u8, SYSTEM_ID_LIST)
CORE_ENUM_DECLARE_INT_OPS(sp::SystemId, u8)

} // namespace sp
