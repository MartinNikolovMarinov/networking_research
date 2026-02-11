# Active Research Track

## Status

- Status: `In Progress`
- Selected project: `vm_to_vm_lab`

## Scope

The active track intentionally combines multiple ideas:

- Namespace packet lab first (`ip netns`, veth, bridge) for rapid iteration
- QEMU/KVM VM-to-VM networking second (TAP/bridge + virtio-net)
- Shared observability/benchmarking scenarios across both phases

Expected timeline for meaningful end-to-end outcome: more than 1 week (typically multi-week).

## Why this naming

`vm_to_vm_lab` names the destination scope, not only the first implementation step.
Phase 1 resembles container/namespace networking by design, because it is the fastest bootstrap path.
The same scenarios are then carried into true VM-to-VM experiments.

## Related files

- `projects/vm_to_vm_lab/README.md`
- `ideas/virtualization_ideas.md`
