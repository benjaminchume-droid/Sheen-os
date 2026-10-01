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
- Stage 1 — executable x86-64 UEFI boot foundation: in progress

See:
- docs/STAGE-0.1.md
- docs/INTERFACES.md
- docs/API-CONTRACTS-0.4.md
- docs/TARGETS.md
- docs/BUILD.md
- docs/ROADMAP.md
