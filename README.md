# Sheen OS

Linux-native, USB-first TV operating system for PCs, laptops and future dedicated hardware.

Core targets:
- bootable USB
- media playback
- live TV
- casting
- display processing/upscaling
- ordinary Android APK compatibility
- Android TV APK compatibility

## Architecture

Linux kernel → hardware/runtime services → media/TV/cast/vision/Android subsystems → applications → Sheen shell.

The shell is a client of the operating system, not the operating system itself.

## Engineering rule

Sheen is built from real underlying primitives and general-purpose subsystem contracts. A demo path must not become a hidden architectural dependency.

## Current development state

- Stage 0.1 — repository architecture foundation: complete
- Stage 0.2 — build system: complete
- Stage 0.3 — target definitions: complete
- Stage 0.4 — API contracts: complete
- Stage 0.5 — testing infrastructure: complete
- Stage 1.1 — Linux kernel foundation: complete
- Stage 1.2 — UEFI boot foundation: complete
- Stage 1.3 — initramfs foundation: complete
- Stage 1.4 — native PID 1 foundation: complete
- Stage 1.5 — root filesystem foundation: complete
- Stage 1.6 — USB image foundation: complete
- Stage 2.1 — device discovery: complete
- Stage 2.2 — hardware abstraction layer: complete
- Stage 2.3 — device manager: complete
- Stage 2.4 — service manager: complete
- Stage 2.5 — IPC bus: complete
- Stage 2.6 — configuration system: complete
- Stage 2.7 — logging and diagnostics: complete
- Stage 3.1 — DRM/KMS display foundation: complete
- Stage 1 — executable x86-64 UEFI boot foundation: in progress

See:
- docs/STAGE-0.1.md
- docs/INTERFACES.md
- docs/API-CONTRACTS-0.4.md
- docs/TARGETS.md
- docs/BUILD.md
- docs/KERNEL-1.1.md
- docs/UEFI-1.2.md
- docs/INITRAMFS-1.3.md
- docs/PID1-1.4.md
- docs/ROOTFS-1.5.md
- docs/USB-1.6.md
- docs/FIRST-BOOT-1.7.md
- docs/DEVICE-DISCOVERY-2.1.md
- hardware/hal/README.md
- docs/DEVICE-MANAGER-2.3.md
- docs/SERVICE-MANAGER-2.4.md
- docs/IPC-2.5.md
- docs/CONFIG-2.6.md
- docs/LOGGING-2.7.md
- docs/DRM-3.1.md
- docs/ROADMAP.md
