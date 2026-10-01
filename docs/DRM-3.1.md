# Phase 3.1 — DRM/KMS Display

Phase 3.1 establishes the kernel-backed DRM/KMS display discovery layer.

## Hardware boundary

`hardware/graphics/drm-probe.c` opens actual DRM card devices exposed by Linux and queries DRM resources and connectors through the kernel DRM ioctls. It enumerates connector types, connection state, physical dimensions, and available modes when the hardware/driver exposes them.

No display device is invented when `/dev/dri` is absent or inaccessible. The probe reports the observed availability state and exits cleanly.

## Scope

3.1 is hardware/display discovery and resource inspection. Persistent DRM master ownership, atomic modesetting policy, framebuffer/GBM allocation, compositor integration, and user-facing display management are subsequent stages.

## Validation

`tools/test-drm.sh` compiles the real probe with strict warnings and parses its live JSON output. On machines without accessible DRM hardware, the test validates the explicit unavailable state rather than simulating a GPU.
