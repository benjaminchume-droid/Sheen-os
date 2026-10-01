# Phase 7.3 — Scaling

Phase 7.3 provides a real CPU scaling path for common Vision frame formats.

The scaler currently supports nearest-neighbor geometry conversion for RGBA8888, BGRA8888, NV12, and P010. Output buffers are allocated and owned by the destination frame, while presentation timing and color metadata are preserved.

The implementation is intentionally backend-neutral. A future GPU scaler can implement the same transform contract without changing the Vision frame representation.

## Scope

High-quality bilinear/lanczos filtering, super-resolution, GPU shader scaling, adaptive quality selection, and hardware-specific scaling engines are subsequent Vision stages.