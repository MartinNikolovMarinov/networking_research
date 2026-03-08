#include "core_init.h" // IWYU pragma: keep

#include "common_types.h"
#include "diagnostics/diagnostics.h" // IWYU pragma: keep

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>

// core::expected<sp::OsErr> checkNetlinkHeader(const nlmsghdr* nh) {
//     if (nh->nlmsg_type == NLMSG_ERROR)
//     {
//         const nlmsgerr* nherr = reinterpret_cast<const nlmsgerr*>(NLMSG_DATA(nh));
//         if (nherr->error == 0) {
//             return {}; // Is this actually something possible ?
//         }
//         i32 code = -nherr->error;
//         RETURN_OS_ERR(code);
//     }

//     return {};
// }

// struct GetInfoNetInterfaceMessage {
//     struct nlmsghdr nlh;
//     struct ifinfomsg ifm;
// };

// struct Netlink {
//     core::AtomicU32 seq;
//     i32 fd;

//     Netlink() : seq(0), fd(-1) {}
//     Netlink(core::AtomicU32 _seq, i32 _fd) : seq(_seq.load()), fd(_fd) {}
//     Netlink(Netlink&& other) {
//         this->seq = other.seq.load();
//         this->fd = other.fd;

//         other.seq = 0;
//         other.fd = -1;
//     }

//     // core::expected<sp::OsErr> getNetworkInterfaceInfo() {
//     //     GetInfoNetInterfaceMessage payload = {};

//     //     payload.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
//     //     payload.nlh.nlmsg_type = RTM_GETLINK;
//     //     payload.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
//     //     payload.nlh.nlmsg_seq = seq.fetch_add(1);

//     //     payload.ifm.ifi_family = AF_PACKET;

//     //     addr_off sendN = send(fd, &payload, payload.nlh.nlmsg_len, 0);
//     //     TRY_SYSCALL(sendN > 0);

//     //     msghdr msghdr = {};
//     //     iovec iov;
//     //     // char buf[8192];
//     //     char buf[100];

//     //     iov.iov_base = buf;
//     //     iov.iov_len  = sizeof(buf);

//     //     msghdr.msg_iov = &iov;
//     //     msghdr.msg_iovlen = 1;

//     //     bool done = false;
//     //     while (!done) {
//     //         addr_off recvMsgN = recvmsg(fd, &msghdr, 0);
//     //         TRY_SYSCALL(recvMsgN >= 0);

//     //         nlmsghdr *nh = reinterpret_cast<nlmsghdr*>(buf);

//     //         while(NLMSG_OK(nh, recvMsgN)) {
//     //             if (nh->nlmsg_type == NLMSG_DONE) break;
//     //             TRY_EXPR(checkNetlinkHeader(nh));

//     //             ifinfomsg* ifi = reinterpret_cast<ifinfomsg*>(NLMSG_DATA(nh));

//     //             rtattr *attr = IFLA_RTA(ifi);
//     //             u32 attrLen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi));

//     //             while (RTA_OK(attr, attrLen)) {
//     //                 if (attr->rta_type == IFLA_IFNAME) {
//     //                     logInfo("intrface: {}", reinterpret_cast<const char*>(RTA_DATA(attr)));
//     //                 }
//     //                 attr = RTA_NEXT(attr, attrLen);
//     //             }

//     //             nh = NLMSG_NEXT(nh, recvMsgN);
//     //         }
//     //     }

//     //     return {};
//     // }

//     core::expected<sp::OsErr> close() {
//         i32 closeRes = ::close(this->fd);
//         TRY_SYSCALL(closeRes >= 0);
//         seq = 0;
//         return {};
//     }

//     static core::expected<Netlink, sp::OsErr> create() {
//         i32 fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
//         TRY_SYSCALL(fd >= 0);

//         struct sockaddr_nl addr = {};
//         addr.nl_family = AF_NETLINK;
//         addr.nl_pid = u32(getpid());

//         i32 bindRes = bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
//         TRY_SYSCALL(bindRes >= 0);

//         Netlink ret (0, fd);
//         return ret;
//     }
// };

// core::expected<sp::OsErr> _main() {
//     Netlink netlink = TRY(Netlink::create());
//     defer { netlink.close(); };

//     // addr_off sendN = send(fd, &req, req.nlh.nlmsg_len, 0);
//     // TRY_SYSCALL(sendN > 0);

//     // char buff[1024] = {};

//     // addr_off recvN = recv(fd, buff, sizeof(buff), 0);
//     // TRY_SYSCALL(recvN > 0);

//     // const nlmsghdr* nh = reinterpret_cast<const nlmsghdr*>(buff);
//     // TRY_EXPR(checkNetlinkHeader(nh));

//     // while(NLMSG_OK(nh, recvN)) {
//     //     if (nh->nlmsg_type == NLMSG_DONE) break;
//     //     TRY_EXPR(checkNetlinkHeader(nh));

//     //     ifinfomsg* ifi = reinterpret_cast<ifinfomsg*>(NLMSG_DATA(nh));

//     //     rtattr *attr = IFLA_RTA(ifi);
//     //     u32 attrLen = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi));

//     //     while (RTA_OK(attr, attrLen)) {
//     //         if (attr->rta_type == IFLA_IFNAME) {
//     //             logInfo("intrface: {}", reinterpret_cast<const char*>(RTA_DATA(attr)));
//     //         }
//     //         attr = RTA_NEXT(attr, attrLen);
//     //     }

//     //     nh = NLMSG_NEXT(nh, recvN);
//     // }

//     // i32 closeRes = close(fd);
//     // TRY_SYSCALL(closeRes >= 0);

//     return {};
// }

core::expected<i32, sp::OsErr> ex1_createOsErrorNoDiag(bool hasErr) {
    if (hasErr) {
        [[maybe_unused]] sp::OsErr a = CREATE_OS_ERR(-1);
        sp::OsErr b = CREATE_OS_ERR(-2);
        return core::unexpected(std::move(b));
    }
    return 1;
}

core::expected<i32, sp::OsErr> ex2_l2_returnOsErrWithDiag(bool hasErr) {
    if (hasErr) {
        RETURN_OS_ERR(10);
    }
    return 1;
}

core::expected<i32, sp::OsErr> ex2_l1_returnOsErrWithDiag(bool hasErr) {
    TRYX(ex2_l2_returnOsErrWithDiag(hasErr));
    return 1;
}

core::expected<sp::OsErr> _main() {
    i32 x = TRYV(ex1_createOsErrorNoDiag(false));
    logInfo("x={}", x);

    TRYX(ex2_l1_returnOsErrWithDiag(true));
    logInfo("passed");

    return {};
}

int main() {
    using namespace sp;

    coreInit(core::LogLevel::L_DEBUG);
    defer { coreShutdown(); };

    Expect(_main());

    return 0;
}
