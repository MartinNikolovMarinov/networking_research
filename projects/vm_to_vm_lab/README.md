# vm_to_vm_lab

## Purpose

`vm_to_vm_lab` is a research project to build repeatable packet-level experiments between isolated compute nodes.
Final target is VM-to-VM networking with QEMU/KVM.

## Project phases

1. Namespace phase (fast bootstrap)
- Linux namespaces + veth + bridge.
- Validate topology creation, routing, packet capture, and fault injection.
- Use this as fast feedback before hypervisor complexity.

2. VM phase (target architecture)
- QEMU/KVM guests connected via TAP/bridge and virtio-net.
- Re-run the same scenarios from phase 1 for apples-to-apples comparisons.

3. Advanced phase
- Optional: eBPF observability, virtio queue instrumentation, DPDK/SR-IOV experiments.

## Current status

- Placeholder executable exists: `vm_to_vm_lab`.
- Next implementation step: add topology bootstrap code/scripts for phase 1.
