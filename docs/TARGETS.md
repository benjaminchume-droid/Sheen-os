# Sheen Platform Targets

Stage 0.3 establishes a canonical target registry.

## Target model

A Sheen target is a platform contract containing:

1. architecture
2. firmware/boot environment
3. deployment medium
4. kernel source and configuration
5. image layout
6. boot policy
7. baseline hardware capabilities
8. compatibility capabilities

Canonical manifests live under `configs/targets/*.toml`.

## Current target

### x86_64-uefi-usb

Generic x86-64 UEFI USB target for PCs and laptops.

- x86-64
- UEFI
- USB
- GPT/FAT32 ESP
- Linux 6.12.51
- GRUB removable boot
- DRM/KMS baseline
- USB/storage/PCI baseline
- audio/network baseline

Its manifest is `configs/targets/x86_64-uefi-usb.toml`.

## Capability semantics

A target capability describes the platform baseline, not a promise that every physical
machine has that hardware. Runtime hardware discovery remains authoritative.

Capabilities can be:
- required for the target
- supported by the target profile
- optional at runtime

The target profile must never pretend optional hardware exists.

## Portability

Architecture-specific code belongs below stable system/runtime contracts. Adding a new
architecture or board should require kernel/hardware adapter work rather than application
rewrites.

Future targets:
- x86_64-uefi-ssd
- arm64-uefi-usb
- arm64-sheen-box
- sheen-reference-board
