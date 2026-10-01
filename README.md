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
- Stage 3.2 — GPU detection: complete
- Stage 3.3 — display manager: complete
- Stage 3.4 — audio engine foundation: complete
- Stage 3.5 — input engine: complete
- Stage 3.6 — TV input abstraction: complete
- Stage 4.1 — container engine foundation: complete
- Stage 4.2 — codec subsystem foundation: complete
- Stage 4.3 — demuxing foundation: complete
- Stage 4.4 — decoder pipeline foundation: complete
- Stage 4.5 — hardware video decoding foundation: complete
- Stage 4.6 — audio pipeline foundation: complete
- Stage 4.7 — subtitle engine: complete
- Stage 4.8 — streaming: complete
- Stage 4.9 — media library: complete
- Stage 4.10 — recording engine: complete
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
- docs/GPU-3.2.md
- docs/DISPLAY-MANAGER-3.3.md
- docs/AUDIO-3.4.md
- docs/INPUT-3.5.md
- docs/TV-INPUT-3.6.md
- docs/CONTAINER-4.1.md
- docs/CODEC-4.2.md
- docs/DEMUX-4.3.md
- docs/DECODER-4.4.md
- docs/HW-DECODER-4.5.md
- docs/AUDIO-4.6.md
- docs/SUBTITLES-4.7.md
- docs/STREAMING-4.8.md
- docs/MEDIA-LIBRARY-4.9.md
- docs/RECORDING-4.10.md
- docs/TUNER-5.1.md
- docs/BROADCAST-5.2.md
- docs/SCAN-5.3.md
- docs/CHANNEL-DB-5.4.md
- docs/LIVE-5.5.md
- docs/EPG-5.6.md
- docs/TIMESHIFT-5.7.md
- docs/DVR-5.8.md
- docs/NETWORK-TV-5.9.md
- docs/CAST-DISCOVERY-6.1.md
- docs/MEDIA-CAST-6.2.md
- docs/MIRROR-6.3.md
- docs/WEBRTC-6.5.md
- docs/CAST-SESSION-6.6.md
- docs/CAST-DEVICES-6.7.md
- docs/VISION-FRAME-7.1.md
- docs/SCALER-7.3.md
- docs/SUPERRES-7.4.md
- docs/HDR-7.8.md
- docs/ROADMAP.md
- Stage 5.1 — Linux tuner integration: complete
- Stage 5.2 — broadcast abstraction: complete
- Stage 5.3 — channel scanning: complete
- Stage 5.4 — channel database: complete
- Stage 5.5 — live playback foundation: complete
- Stage 5.6 — EPG/service information: complete
- Stage 5.7 — timeshift: complete
- Stage 5.8 — DVR scheduler: complete

- Stage 5.9 — network TV: complete
- Stage 6.1 — casting discovery: complete
- Stage 6.2 — media casting: complete
- Stage 6.3 — screen mirroring ingress: complete

- Stage 6.5 — WebRTC receiver transport foundation: complete

- Stage 6.6 — casting session manager: complete

- Stage 6.7 — multi-device management: complete

- Stage 7.1 — Vision frame pipeline: complete

- Stage 7.2 — Vision hardware acceleration inventory: complete
- Stage 7.3 — Vision scaling: complete
- Stage 7.4 — classical super-resolution: complete

- Stage 7.5 — denoising: complete
- Stage 7.6 — deblocking: complete
- Stage 7.7 — sharpening: complete
- Stage 7.8 — HDR/SDR processing: complete
