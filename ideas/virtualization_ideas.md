# Virtualization + Networking Research Ideas

## Status Legend

- `[ACTIVE]` currently being implemented
- `[PLANNED]` candidate backlog item

## 1. [ACTIVE] VM-to-VM packet lab with Linux network namespaces (fastest path)
- Goal: build intuition for packet flow, routing, NAT, and firewall behavior.
- Scope: create 2-3 namespaces + veth pairs + bridge, run your `sandbox` client/server experiments through them.
- Why: no hypervisor dependency; fast iteration and very debuggable.
- Estimated timeframe: 2-4 days for a solid first version, 1 week with automation and repeatable scripts.
- Complexity: Low.

## 2. [ACTIVE] QEMU virtual network playground (user-mode + TAP + bridge)
- Goal: make multiple QEMU guests talk over virtual links.
- Scope: compare `-netdev user` vs TAP/bridge; measure latency and throughput.
- Why: practical foundation before full VM driver work.
- Estimated timeframe: 4-7 days to stand up and benchmark basic topologies.
- Complexity: Medium.

## 3. [ACTIVE] KVM micro-VM networking baseline
- Goal: run a minimal Linux guest with virtio-net on KVM and instrument packet path.
- Scope: host bridge, guest static IP, ping/iperf, tcpdump on both host + guest.
- Why: realistic production-like virtualization stack.
- Estimated timeframe: 1-2 weeks (depends on host setup and tooling maturity).
- Complexity: Medium to high.

## 4. [ACTIVE] Fair + deterministic networking client benchmark track (start early)
- Goal: compare different client implementations using repeatable and fair measurements.
- Scope:
  - fixed environment controls (CPU pinning, MTU/sysctls, fixed topology, warmup, fixed seeds)
  - fixed workload matrix (message sizes, connection counts, steady and burst profiles)
  - metrics: throughput, p50/p95/p99 latency, packet loss/retransmits, CPU cost
  - two scenario classes:
    - ideal baseline (deterministic)
    - impaired network profiles via `tc netem` (loss, delay, jitter)
- Why: gives useful comparative signal early, even if it is not a perfect simulation of production traffic.
- Estimated timeframe: 3-5 days for a useful MVP harness.
- Complexity: Medium.

## 5. [PLANNED] Virtio-net driver deep-dive (guest side)
- Goal: understand RX/TX virtqueues and interrupts.
- Scope: prototype a tiny userspace model first, then inspect Linux virtio-net behavior.
- Why: best way to learn virtual NIC internals before writing custom code.
- Estimated timeframe: 2-4 weeks for meaningful depth.
- Complexity: High.

## 6. [PLANNED] eBPF observability for virtual networking
- Goal: trace packet lifecycle across veth, bridge, tap, and virtio boundaries.
- Scope: tc/xdp/bpftrace probes on host; per-hop latency and drop reason collection.
- Why: gives hard evidence when tuning or debugging.
- Estimated timeframe: 1-2 weeks for useful tracing dashboards and scripts.
- Complexity: Medium to high.

## 7. [PLANNED] Container networking internals (Docker + CNI style)
- Goal: reproduce what container runtimes do under the hood.
- Scope: bridge, veth, iptables/nftables, port-mapping, service discovery mock.
- Why: directly useful if you later include k8s.
- Estimated timeframe: 1-2 weeks for a complete reproducible lab.
- Complexity: Medium.

## 8. [PLANNED] Kubernetes networking mini-lab
- Goal: compare CNI models (bridge, overlay, eBPF dataplane).
- Scope: kind/k3d cluster, network policies, service routing tests.
- Why: higher-level orchestration + networking complexity.
- Estimated timeframe: 2-3 weeks for meaningful comparisons.
- Complexity: High.

## 9. [PLANNED] SR-IOV and passthrough (advanced)
- Goal: compare paravirtualized virtio-net vs direct VF passthrough.
- Scope: baseline throughput/latency and CPU cost.
- Why: performance-focused direction for serious networking research.
- Estimated timeframe: 2-4 weeks (hardware/platform dependent).
- Complexity: Very high.

## 10. [PLANNED] DPDK-based packet processing in VMs/containers
- Goal: bypass kernel networking path for high PPS workloads.
- Scope: run testpmd in guest/container, compare with kernel sockets.
- Why: prepares ground for NFV-style experiments.
- Estimated timeframe: 2-4 weeks for setup + first reliable benchmarks.
- Complexity: Very high.

## 11. [PLANNED] SPDK-inspired control/data plane split for network I/O
- Goal: explore polled-mode, lockless queue design patterns for networking.
- Scope: design a small prototype queue engine with fixed buffers.
- Why: architectural learning from SPDK ideas without full NVMe scope.
- Estimated timeframe: 2-3 weeks for a focused prototype.
- Complexity: High.

## 12. [PLANNED] Build a tiny L2/L3 userspace stack for experimentation
- Goal: implement ARP + IPv4 + ICMP + UDP subset.
- Scope: run over TAP device and validate with tcpdump/wireshark.
- Why: excellent systems exercise and directly relevant to virtual NIC work.
- Estimated timeframe: 3-6 weeks depending on protocol depth and test quality.
- Complexity: Very high.

## 13. [PLANNED] Virtual switch prototype
- Goal: implement a minimal software switch with MAC learning.
- Scope: forwarding table, flooding, VLAN tagging support (optional).
- Why: foundational for understanding OVS/bridge behavior.
- Estimated timeframe: 2-4 weeks for a useful and testable implementation.
- Complexity: High.

## 14. [PLANNED] Fault-injection framework for virtual networks
- Goal: evaluate resilience under packet loss, reordering, duplication, jitter.
- Scope: tc netem profiles + automated scenario runner.
- Why: helps compare architecture choices under realistic failures.
- Estimated timeframe: 1-2 weeks to build and integrate with your test workloads.
- Complexity: Medium.

## 15. [PLANNED] Security-focused virtualization networking
- Goal: harden east-west traffic between VMs/containers.
- Scope: microsegmentation policies, service identity, mTLS experiments.
- Why: practical and high-value research topic.
- Estimated timeframe: 2-4 weeks for baseline policy and threat-model-driven validation.
- Complexity: High.

## 16. [PLANNED] Reproducible benchmark harness
- Goal: standardize measurements across namespaces, VMs, and containers.
- Scope: automate ping, iperf3, netperf, pcap capture, CPU/mem metrics, report generation.
- Why: converts experiments into publishable, comparable data.
- Estimated timeframe: 1-2 weeks for MVP, ongoing refinement afterward.
- Complexity: Medium.

## Suggested execution order (pragmatic)
1. Namespace packet lab
2. QEMU VM-to-VM connectivity
3. KVM + virtio-net baseline
4. eBPF observability
5. Tiny userspace L2/L3 stack
6. Container + k8s networking
7. DPDK / SR-IOV / advanced performance work

## First concrete project idea
- Build a "virtual network testbed" script set:
  - mode A: namespaces
  - mode B: QEMU guests
  - common: topology definition, automatic capture, latency/throughput report
- Then integrate your C transport client/server as workload generators.
