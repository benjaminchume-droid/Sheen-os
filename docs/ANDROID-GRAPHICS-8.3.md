# Phase 8.3 — Android Graphics Bridge Foundation

Phase 8.3 establishes the buffer handoff between Android graphics memory and the Sheen DRM/compositor layer.

The bridge accepts a DMA-BUF file descriptor plus buffer geometry and imports it into a DRM device through `DRM_IOCTL_PRIME_FD_TO_HANDLE`. Imported GEM handles can later be released through `DRM_IOCTL_GEM_CLOSE`.

This avoids forcing a CPU bitmap-copy architecture on Android applications; a future gralloc/SurfaceFlinger adapter can hand real DMA-BUF-backed buffers into Sheen display sessions.

## Scope

Android gralloc format metadata, sync fences, acquire/release timelines, SurfaceFlinger protocol adaptation, compositor surfaces, and actual Android window lifecycle remain later graphics compatibility stages.