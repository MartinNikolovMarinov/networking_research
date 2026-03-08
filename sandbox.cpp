#include "core_init.h" // IWYU pragma: keep

#include "common_types.h"
#include "diagnostics/diagnostics.h" // IWYU pragma: keep

#include <fcntl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <poll.h>
#include <unistd.h>

namespace sp {

core::expected<sp::OsErr> checkNetlinkHeader(const nlmsghdr* nh) {
    if (nh->nlmsg_type == NLMSG_ERROR)
    {
        const nlmsgerr* nherr = reinterpret_cast<const nlmsgerr*>(NLMSG_DATA(nh));
        if (nherr->error == 0) {
            return {}; // Is this actually something possible ?
        }
        i32 code = -nherr->error;
        RETURN_OS_ERR(code);
    }

    return {};
}

struct GetInfoNetInterfacePayload {
    nlmsghdr nlh;
    ifinfomsg ifm;

    static GetInfoNetInterfacePayload create(u32 seq) {
        GetInfoNetInterfacePayload payload = {};

        payload.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(ifinfomsg));
        payload.nlh.nlmsg_type = RTM_GETLINK;
        payload.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
        payload.nlh.nlmsg_seq = seq;

        payload.ifm.ifi_family = AF_PACKET;

        return payload;
    }
};

struct Netlink {
    core::AtomicU32 seq;
    i32 fd;

    Netlink() : seq(0), fd(-1) {}
    Netlink(core::AtomicU32 _seq, i32 _fd) : seq(_seq.load()), fd(_fd) {}
    Netlink(Netlink&& other) {
        this->seq = other.seq.load();
        this->fd = other.fd;

        other.seq = 0;
        other.fd = -1;
    }

    core::expected<sp::OsErr> waitForReadable(i32 timeoutMs) {
        pollfd pfd = {};
        pfd.fd = fd;
        pfd.events = POLLIN;

        i32 pollRes = poll(&pfd, 1, timeoutMs);
        TRY_SYSCALL(pollRes >= 0);

        if (pollRes == 0) {
            RETURN_OS_ERR(ETIMEDOUT);
        }

        if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
            RETURN_OS_ERR(EIO);
        }

        if ((pfd.revents & POLLIN) == 0) {
            RETURN_OS_ERR(EAGAIN);
        }

        return {};
    }

    template <core::AllocatorId TAllocId = core::DEFAULT_ALLOCATOR_ID>
    core::expected<sp::OsErr> getNetworkInterfaceInfo(core::ArrList<core::StrBuilder<TAllocId>, TAllocId>& out, i32 timeoutMs) {
        u32 sequence = this->seq.fetch_add(1);
        auto payload = GetInfoNetInterfacePayload::create(sequence);

        addr_off sendN = send(fd, &payload, payload.nlh.nlmsg_len, 0);
        TRY_SYSCALL(sendN > 0);

        auto onMessage = [&out](nlmsghdr* currNh) -> core::expected<sp::OsErr> {
            ifinfomsg* ifi = reinterpret_cast<ifinfomsg*>(NLMSG_DATA(currNh));

            if (currNh->nlmsg_len < NLMSG_LENGTH(sizeof(*ifi))) {
                RETURN_OS_ERR(EBADMSG);
            }

            u8* attrCursor = reinterpret_cast<u8*>(ifi) + NLMSG_ALIGN(sizeof(ifinfomsg));
            u32 attrLen = currNh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifi));

            while (attrLen >= sizeof(rtattr)) {
                rtattr* attr = reinterpret_cast<rtattr*>(attrCursor);
                if (attr->rta_len < sizeof(rtattr) || attr->rta_len > attrLen) {
                    RETURN_OS_ERR(EBADMSG);
                }

                if (attr->rta_type == IFLA_IFNAME) {
                    const char* ifname = reinterpret_cast<const char*>(RTA_DATA(attr));
                    out.push(core::sv(ifname));
                }

                u32 step = RTA_ALIGN(attr->rta_len);
                attrCursor += step;
                attrLen -= step;
            }

            return {};
        };

        TRYX(receiveUntilDone(sequence, timeoutMs, onMessage));

        return {};
    }

    core::expected<sp::OsErr> close() {
        this->seq = 0;
        i32 closeRes = ::close(this->fd);
        TRY_SYSCALL(closeRes >= 0);
        return {};
    }

    static core::expected<Netlink, sp::OsErr> create() {
        // Create netlink socket for routing:
        i32 fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        TRY_SYSCALL(fd >= 0);

        // Configure the socket to be NON blocking.
        i32 currentFlags = fcntl(fd, F_GETFL);
        TRY_SYSCALL(currentFlags >= 0);
        i32 setFlagsRes = fcntl(fd, F_SETFL, currentFlags | O_NONBLOCK);
        TRY_SYSCALL(setFlagsRes >= 0);

        struct sockaddr_nl addr = {};
        addr.nl_family = AF_NETLINK;
        addr.nl_pid = u32(getpid());

        i32 bindRes = bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        TRY_SYSCALL(bindRes >= 0);

        Netlink ret (0, fd);
        return ret;
    }

private:

    template <typename TOnMessageFn>
    core::expected<sp::OsErr> receiveUntilDone(u32 sequence, i32 timeoutMs, TOnMessageFn&& onMessage) {
        u8 msgBuf[8192]; // TODO: [SLAB_ALLOCATOR] use slab allocator.

        iovec iov;
        iov.iov_base = msgBuf;
        iov.iov_len = sizeof(msgBuf);

        msghdr msg = {};
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        bool done = false;
        while (!done) {
            TRYX(waitForReadable(timeoutMs));

            msg.msg_flags = 0;
            addr_off recvMsgN = recvmsg(fd, &msg, 0);
            if (recvMsgN < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                RETURN_OS_ERR(errno);
            }
            if ((msg.msg_flags & MSG_TRUNC) != 0) {
                RETURN_OS_ERR(EMSGSIZE);
            }

            u32 msgLen = u32(recvMsgN);
            u8* cursor = msgBuf;

            while (msgLen >= sizeof(nlmsghdr)) {
                nlmsghdr* currNh = reinterpret_cast<nlmsghdr*>(cursor);
                if (currNh->nlmsg_len < sizeof(nlmsghdr) || currNh->nlmsg_len > msgLen) {
                    RETURN_OS_ERR(EBADMSG);
                }

                if (currNh->nlmsg_seq != sequence) {
                    u32 step = NLMSG_ALIGN(currNh->nlmsg_len);
                    cursor += step;
                    msgLen -= step;
                    continue;
                }

                if (currNh->nlmsg_type == NLMSG_DONE) {
                    done = true;
                    break;
                }

                TRYX(checkNetlinkHeader(currNh));
                TRYX(onMessage(currNh));

                u32 step = NLMSG_ALIGN(currNh->nlmsg_len);
                cursor += step;
                msgLen -= step;
            }
        }

        return {};
    }
};

} // namespace sp

core::expected<sp::OsErr> _main() {
    sp::Netlink netlink = TRYV(sp::Netlink::create());
    defer { netlink.close(); };

    core::ArrList<core::StrBuilder<>> out;
    TRYX(netlink.getNetworkInterfaceInfo(out, 3000));

    for (addr_size i = 0; i < out.len(); i++) {
        auto& x = out[i];
        logInfo("interface: {}", x.view());
    }

    return {};
}

int main() {
    using namespace sp;

    coreInit(core::LogLevel::L_DEBUG);
    defer { coreShutdown(); };

    Expect(_main());

    return 0;
}
