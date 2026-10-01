# Phase 1.3 — Initramfs

Phase 1.3 establishes the early-userspace image loaded by the kernel before a persistent root filesystem exists.

## Contents

- /init entrypoint
- static BusyBox and required applet links
- /dev and /dev/pts mount points
- /proc, /sys, /run and /tmp
- /etc metadata for the early-userspace layout

The initramfs is generated as a compressed newc cpio archive and loaded directly by the UEFI/GRUB boot path.

## Design

The initramfs is intentionally small and hardware-independent. It provides the minimum environment required to initialize kernel interfaces and hand control to later Sheen system stages. It does not contain the eventual TV shell, media stack, Android runtime, or persistent root filesystem.

## Validation

tools/test-initramfs.sh decompresses the actual generated archive and checks its real entry list. It does not merely validate a manifest or simulated filesystem.

## Scope

PID 1 policy and long-running service supervision are Stage 1.4. Persistent root filesystem construction is Stage 1.5.
