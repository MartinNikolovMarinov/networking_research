#include "nr_virtual_interface.h"
#include "nr_logger.h"

#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/veth.h>
#include <sys/socket.h>
#include <unistd.h>

#define NR_NETLINK_MSG_BUFFER_SIZE 2048U
#define NR_NETLINK_ACK_BUFFER_SIZE 4096U

typedef struct NrNetlinkRequest {
    struct nlmsghdr nlh;
    struct ifinfomsg ifi;
    uint8_t payload[NR_NETLINK_MSG_BUFFER_SIZE];
} NrNetlinkRequest;

static int32_t appendRawData(struct nlmsghdr* nlh, uint32_t maxLen, const void* data, uint32_t dataLen) {
    uint32_t oldLen = NLMSG_ALIGN((uint32_t)nlh->nlmsg_len);
    uint32_t newLen = oldLen + NLMSG_ALIGN(dataLen);
    if (newLen > maxLen) {
        errno = ENOBUFS;
        return -1;
    }

    void* dst = ((uint8_t*)nlh) + oldLen;
    memset(dst, 0, (size_t)NLMSG_ALIGN(dataLen));
    memcpy(dst, data, (size_t)dataLen);
    nlh->nlmsg_len = newLen;
    return 0;
}

static int32_t appendRtAttr(
    struct nlmsghdr* nlh,
    uint32_t maxLen,
    uint16_t type,
    const void* data,
    uint32_t dataLen
) {
    uint32_t oldLen = NLMSG_ALIGN((uint32_t)nlh->nlmsg_len);
    uint32_t attrLen = RTA_LENGTH(dataLen);
    uint32_t newLen = oldLen + RTA_ALIGN(attrLen);
    if (newLen > maxLen) {
        errno = ENOBUFS;
        return -1;
    }
    if (attrLen > UINT16_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    uintptr_t baseAddr = (uintptr_t)(void*)nlh;
    uintptr_t attrAddr = baseAddr + (uintptr_t)oldLen;
    struct rtattr* rta = (struct rtattr*)(void*)attrAddr;
    rta->rta_type = type;
    rta->rta_len = (uint16_t)attrLen;

    if (dataLen > 0U && data != NULL) {
        memcpy(RTA_DATA(rta), data, (size_t)dataLen);
    }

    nlh->nlmsg_len = newLen;
    return 0;
}

static struct rtattr* beginRtAttrNest(struct nlmsghdr* nlh, uint32_t maxLen, uint16_t type) {
    uint32_t oldLen = NLMSG_ALIGN((uint32_t)nlh->nlmsg_len);
    uint32_t newLen = oldLen + RTA_ALIGN(RTA_LENGTH(0U));
    if (newLen > maxLen) {
        errno = ENOBUFS;
        return NULL;
    }

    uintptr_t baseAddr = (uintptr_t)(void*)nlh;
    uintptr_t nestAddr = baseAddr + (uintptr_t)oldLen;
    struct rtattr* nest = (struct rtattr*)(void*)nestAddr;
    nest->rta_type = type;
    nest->rta_len = RTA_LENGTH(0U);
    nlh->nlmsg_len = newLen;
    return nest;
}

static void endRtAttrNest(struct nlmsghdr* nlh, struct rtattr* nest) {
    uint8_t* base = (uint8_t*)nest;
    uint8_t* end = ((uint8_t*)nlh) + NLMSG_ALIGN((uint32_t)nlh->nlmsg_len);
    uint32_t len = (uint32_t)(end - base);
    if (len > UINT16_MAX) {
        errno = EOVERFLOW;
        return;
    }
    nest->rta_len = (uint16_t)len;
}

static int32_t parseNetlinkAck(int32_t fd) {
    union {
        struct nlmsghdr align;
        uint8_t bytes[NR_NETLINK_ACK_BUFFER_SIZE];
    } ackBuffer;

    int64_t received = (int64_t)recv(fd, ackBuffer.bytes, sizeof(ackBuffer.bytes), 0);
    if (received < 0) {
        return -1;
    }
    if ((uint64_t)received < sizeof(struct nlmsghdr)) {
        errno = EPROTO;
        return -1;
    }

    struct nlmsghdr* nlh = &ackBuffer.align;
    if (nlh->nlmsg_type != NLMSG_ERROR) {
        errno = EPROTO;
        return -1;
    }
    if ((uint64_t)nlh->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
        errno = EPROTO;
        return -1;
    }

    struct nlmsgerr* err = (struct nlmsgerr*)NLMSG_DATA(nlh);
    if (err->error == 0) {
        return 0;
    }

    errno = -err->error;
    return -1;
}

static int32_t sendCreateLinkMessage(struct nlmsghdr* nlh) {
    int32_t fd = (int32_t)socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        return -1;
    }

    struct sockaddr_nl kernelAddr;
    memset(&kernelAddr, 0, sizeof(kernelAddr));
    kernelAddr.nl_family = AF_NETLINK;

    ssize_t sent = sendto(
        (int)fd,
        nlh,
        (size_t)nlh->nlmsg_len,
        0,
        (const struct sockaddr*)&kernelAddr,
        sizeof(kernelAddr)
    );

    if (sent < 0) {
        int32_t savedErrno = errno;
        close((int)fd);
        errno = savedErrno;
        return -1;
    }

    int32_t ackRet = parseNetlinkAck(fd);
    int32_t savedErrno = errno;
    close((int)fd);
    errno = savedErrno;
    return ackRet;
}

static int32_t createDummyInterface(const NrVirtualInterfaceCreateInfo* info) {
    NrNetlinkRequest req;
    memset(&req, 0, sizeof(req));

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(req.ifi));
    req.nlh.nlmsg_type = RTM_NEWLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
    req.ifi.ifi_family = AF_UNSPEC;

    if (appendRtAttr(&req.nlh, sizeof(req), IFLA_IFNAME, info->name, (uint32_t)strlen(info->name) + 1U) != 0) {
        return -1;
    }

    struct rtattr* linkInfo = beginRtAttrNest(&req.nlh, sizeof(req), IFLA_LINKINFO);
    if (linkInfo == NULL) {
        return -1;
    }

    const char* kind = "dummy";
    if (appendRtAttr(&req.nlh, sizeof(req), IFLA_INFO_KIND, kind, (uint32_t)strlen(kind) + 1U) != 0) {
        return -1;
    }
    endRtAttrNest(&req.nlh, linkInfo);

    return sendCreateLinkMessage(&req.nlh);
}

static int32_t createVethInterface(const NrVirtualInterfaceCreateInfo* info) {
    NrNetlinkRequest req;
    memset(&req, 0, sizeof(req));

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(req.ifi));
    req.nlh.nlmsg_type = RTM_NEWLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
    req.ifi.ifi_family = AF_UNSPEC;

    if (appendRtAttr(&req.nlh, sizeof(req), IFLA_IFNAME, info->name, (uint32_t)strlen(info->name) + 1U) != 0) {
        return -1;
    }

    struct rtattr* linkInfo = beginRtAttrNest(&req.nlh, sizeof(req), IFLA_LINKINFO);
    if (linkInfo == NULL) {
        return -1;
    }

    const char* kind = "veth";
    if (appendRtAttr(&req.nlh, sizeof(req), IFLA_INFO_KIND, kind, (uint32_t)strlen(kind) + 1U) != 0) {
        return -1;
    }

    struct rtattr* infoData = beginRtAttrNest(&req.nlh, sizeof(req), IFLA_INFO_DATA);
    if (infoData == NULL) {
        return -1;
    }

    struct rtattr* peer = beginRtAttrNest(&req.nlh, sizeof(req), VETH_INFO_PEER);
    if (peer == NULL) {
        return -1;
    }

    struct ifinfomsg peerIfInfo;
    memset(&peerIfInfo, 0, sizeof(peerIfInfo));
    peerIfInfo.ifi_family = AF_UNSPEC;
    if (appendRawData(&req.nlh, sizeof(req), &peerIfInfo, sizeof(peerIfInfo)) != 0) {
        return -1;
    }

    if (appendRtAttr(&req.nlh, sizeof(req), IFLA_IFNAME, info->peerName, (uint32_t)strlen(info->peerName) + 1U) != 0) {
        return -1;
    }

    endRtAttrNest(&req.nlh, peer);
    endRtAttrNest(&req.nlh, infoData);
    endRtAttrNest(&req.nlh, linkInfo);

    return sendCreateLinkMessage(&req.nlh);
}

int32_t nrCreateVirtualInterface(
    const NrVirtualInterfaceCreateInfo* info
) {
    if (info == NULL || info->name == NULL || info->name[0] == '\0') {
        errno = EINVAL;
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "nrCreateVirtualInterface invalid create info");
        return -1;
    }

    int32_t ret = -1;
    switch (info->type) {
        case NR_VIRTUAL_INTERFACE_TYPE_INVALID:
            errno = EINVAL;
            logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "nrCreateVirtualInterface unsupported interface type");
            return -1;
        case NR_VIRTUAL_INTERFACE_TYPE_DUMMY:
            ret = createDummyInterface(info);
            break;
        case NR_VIRTUAL_INTERFACE_TYPE_VETH:
            if (info->peerName == NULL || info->peerName[0] == '\0') {
                errno = EINVAL;
                logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "nrCreateVirtualInterface veth requires peer name");
                return -1;
            }
            ret = createVethInterface(info);
            break;
        default:
            errno = EINVAL;
            logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "nrCreateVirtualInterface unsupported interface type");
            return -1;
    }

    if (ret != 0) {
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "nrCreateVirtualInterface failed errno={} reason={}", (int32_t)errno, strerror(errno));
        return -1;
    }

    switch (info->type) {
        case NR_VIRTUAL_INTERFACE_TYPE_INVALID:
            break;
        case NR_VIRTUAL_INTERFACE_TYPE_VETH:
            logInfoTag(NR_VM_TO_VM_LAB_LOG_TAG, "created veth pair {} <-> {}", info->name, info->peerName);
            break;
        case NR_VIRTUAL_INTERFACE_TYPE_DUMMY:
            logInfoTag(NR_VM_TO_VM_LAB_LOG_TAG, "created dummy interface {}", info->name);
            break;
        default:
            break;
    }

    return 0;
}
