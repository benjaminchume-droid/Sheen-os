# Phase 1.1 — Linux Kernel

Phase 1.1 establishes the Linux kernel as a real, pinned, target-driven component of Sheen OS.

## Kernel contract

The x86_64-uefi-usb target defines:
- architecture: x86-64
- kernel version: Linux 6.12.51
- source: kernel.org tarball matching that version
- configuration fragment: build/kernel.fragment
- base defconfig: kernel/config/x86_64_defconfig

The build combines the kernel upstream x86-64 defconfig with the Sheen fragment and then runs olddefconfig. This keeps the base configuration kernel-native while making Sheen required capabilities explicit.

## Required kernel capabilities

The 1.1 foundation requires UEFI/EFI stub support, initramfs support, devtmpfs, procfs/sysfs/tmpfs, PCI and USB, DRM, ALSA sound, networking, IPv4/IPv6, and loadable modules. Hardware-specific features remain target capabilities and are not simulated when hardware is absent.

## Source integrity

The build downloads the pinned kernel source and verifies its SHA-256 digest against the matching kernel.org checksum manifest before extraction.

## Build outputs

A successful kernel build produces:
- out/<target>/kernel/bzImage
- out/<target>/kernel/.config
- out/<target>/kernel/kernel.release
- out/<target>/kernel/config.sha256

The generated configuration is retained so later stages can inspect the exact kernel feature set used to build the image.

## Scope

Phase 1.1 does not claim that the machine boots successfully. UEFI boot, initramfs/PID 1, root filesystem, USB image validation, and physical boot are separate Stage 1 stages.
