#include "nr_virtual_interface.h"
#include "nr_logger.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void printUsage(const char* exeName) {
    printf("Usage:\n");
    printf("  %s netif create veth <name> <peer_name>\n", exeName);
    printf("  %s netif create dummy <name>\n", exeName);
    printf("\n");
    printf("Examples:\n");
    printf("  sudo %s netif create veth veth_a veth_b\n", exeName);
    printf("  sudo %s netif create dummy dmy0\n", exeName);
}

static int32_t runCreateNetifCommand(int argc, char** argv) {
    if (argc < 5) {
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "not enough arguments for netif create");
        return -1;
    }

    NrVirtualInterfaceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.name = argv[4];

    if (strcmp(argv[3], "veth") == 0) {
        if (argc < 6) {
            logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "veth requires a peer name");
            return -1;
        }
        createInfo.type = NR_VIRTUAL_INTERFACE_TYPE_VETH;
        createInfo.peerName = argv[5];
    }
    else if (strcmp(argv[3], "dummy") == 0) {
        createInfo.type = NR_VIRTUAL_INTERFACE_TYPE_DUMMY;
    }
    else {
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "unsupported netif type {}", argv[3]);
        return -1;
    }

    int32_t ret = nrCreateVirtualInterface(&createInfo);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "netif") != 0) {
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "unknown command {}", argv[1]);
        printUsage(argv[0]);
        return 1;
    }

    if (argc < 4) {
        logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "missing netif subcommand");
        printUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[2], "create") == 0) {
        int32_t ret = runCreateNetifCommand(argc, argv);
        return (int)ret;
    }

    logErrTag(NR_VM_TO_VM_LAB_LOG_TAG, "unknown netif subcommand {}", argv[2]);
    printUsage(argv[0]);
    return 1;
}
