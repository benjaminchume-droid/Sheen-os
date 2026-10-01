# Phase 2.1 — Device Discovery

Phase 2.1 establishes runtime hardware discovery as a real primitive below the HardwareManager API.

## Source of truth

Discovery reads the Linux kernel sysfs hierarchy at runtime. It currently inspects common `/sys/class` device classes plus PCI and USB bus device trees. No device inventory is compiled into the binary.

## Output

`sheen-hw-discover` emits one JSON object per line with:
- `device_id` — stable sysfs-derived identity for the current machine
- `class` — normalized discovery class
- `name` — kernel-exposed device name
- `state` — current presence state
- `properties` — discovered vendor/model/product/subsystem/sysfs metadata when available

JSON Lines keeps the primitive streamable and lets the later HardwareManager choose whether to expose snapshots, events, or IPC without changing the discovery source.

## Integration

The binary is installed at `/usr/sbin/sheen-hw-discover` in the real Sheen root filesystem. The future DeviceManager and Hardware Abstraction Layer consume discovery results; applications do not read sysfs directly.

## Validation

`tools/test-hardware-discovery.sh` compiles the implementation with warnings treated as errors, executes it against the host kernel sysfs, and validates every produced record with Python JSON parsing.

## Scope

2.1 is discovery only. Device initialization, driver ownership, normalized hardware adapters, service lifecycle, and IPC exposure are subsequent stages.


## Hardware HAL boundary

Discovery does not own raw sysfs access helpers. Path joining, attribute reads, symlink resolution, and normalized device identity are provided by `hardware/hal/`; this keeps Linux-specific access mechanics behind one reusable boundary.
