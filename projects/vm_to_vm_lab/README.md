# vm_to_vm_lab

## Purpose

`vm_to_vm_lab` is a research project to build repeatable packet-level experiments between isolated compute nodes.
Final target is VM-to-VM networking with QEMU/KVM.

## Project phases

0. Benchmark foundation phase (start early)
- Build a deterministic benchmark harness for comparing networking client implementations.
- Standardize fairness controls: CPU pinning, fixed MTU/sysctls, fixed topology, fixed run durations, warmup, fixed seeds.
- Capture metrics: throughput, p50/p95/p99 latency, packet loss/retransmits, and CPU cost.
- Run both:
  - ideal lab conditions (deterministic baseline)
  - impaired profiles via `tc netem` (loss/latency/jitter) for real-world relevance checks
- Treat outcomes as comparative evidence between implementations, not absolute production performance.

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
- Next implementation steps:
  - add topology bootstrap code/scripts for phase 1
  - add the benchmark harness from phase 0 so all future clients are measured consistently
