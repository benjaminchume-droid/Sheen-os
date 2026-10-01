# Phase 1.5 — Root Filesystem

Phase 1.5 establishes the persistent Linux filesystem image that will become Sheen OS userland.

## Contents

- ext4 filesystem labeled SHEENROOT
- BusyBox userland
- native Sheen PID 1 at `/sbin/init`
- `/etc/os-release` and Sheen release metadata
- standard `/dev`, `/proc`, `/sys`, `/run`, `/tmp`, `/var`, `/home`, and `/root` directories

The root filesystem is produced as a standalone ext4 artifact under `out/<target>/rootfs/`. It is not yet installed into the USB image; that partition layout is Stage 1.6.

## Design

This root filesystem is deliberately small and real. It provides an actual writable Linux filesystem boundary for later system services, packages, persistent configuration, logs, applications, and storage management.

## Validation

The artifact test runs an ext4 consistency check and inspects the real filesystem with `debugfs`, verifying the expected userland files and native init binary.
