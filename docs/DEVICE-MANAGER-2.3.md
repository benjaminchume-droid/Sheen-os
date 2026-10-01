# Phase 2.3 — Device Manager

Phase 2.3 establishes the system component that owns the discovered hardware inventory.

## Responsibilities

- launch the real hardware discovery primitive
- publish an atomic current inventory under `/run/sheen/hardware/devices.jsonl`
- listen for Linux `NETLINK_KOBJECT_UEVENT` hotplug notifications
- rescan the inventory after relevant add/remove/change/move/bind/unbind events
- keep publication independent from application code

The manager accepts `--once`, `--output`, and `--discovery` options for testing and integration. Its default target paths are `/usr/sbin/sheen-hw-discover` and `/run/sheen/hardware/devices.jsonl`.

## State model

The manager does not invent device presence. Each snapshot is produced from the live Linux sysfs discovery primitive, and snapshots are atomically replaced with `rename()` so readers never observe a partially written inventory.

## Integration

The binary is installed at `/usr/libexec/sheen-device-manager`. Stage 2.4 will provide the general service manager responsible for starting, stopping, supervising, and exposing this service through the Sheen runtime.

## Validation

`tools/test-device-manager.sh` builds the real manager and discovery binaries, executes the manager in one-shot mode against the host kernel, and validates the published records as JSON.
