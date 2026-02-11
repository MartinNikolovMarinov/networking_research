#ifndef NR_VIRTUAL_INTERFACE_H
#define NR_VIRTUAL_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NR_VM_TO_VM_LAB_LOG_TAG "VM_TO_VM_LAB"

typedef enum NrVirtualInterfaceType {
    NR_VIRTUAL_INTERFACE_TYPE_INVALID = 0,
    NR_VIRTUAL_INTERFACE_TYPE_DUMMY,
    NR_VIRTUAL_INTERFACE_TYPE_VETH,
} NrVirtualInterfaceType;

typedef struct NrVirtualInterfaceCreateInfo {
    NrVirtualInterfaceType type;
    const char* name;
    const char* peerName;
} NrVirtualInterfaceCreateInfo;

int32_t nrCreateVirtualInterface(
    const NrVirtualInterfaceCreateInfo* info
);

#ifdef __cplusplus
}
#endif

#endif
