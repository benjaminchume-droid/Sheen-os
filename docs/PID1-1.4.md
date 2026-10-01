# Phase 1.4 — PID 1

Phase 1.4 establishes native process 1 for Sheen early userspace.

## Responsibilities

- verify it is actually running as PID 1
- mount procfs, sysfs, devtmpfs, devpts and tmpfs
- emit early boot diagnostics
- launch the development shell as a child process
- reap terminated children
- respond to SIGTERM, SIGINT and SIGQUIT
- terminate the child during shutdown

The implementation is a small statically linked C executable built directly into the initramfs as /init. It is intentionally not the future Sheen service manager; service supervision belongs to the later runtime/service stages.

## Validation

tools/validate-pid1.sh checks the native source contract. tools/test-pid1.sh extracts the actual /init binary from the generated initramfs and verifies that it is a native static ELF without a dynamic interpreter.

## Scope

Stage 1.4 does not yet provide the complete service manager, IPC fabric, persistent root handoff, restart policies, or production shutdown/recovery policy.
