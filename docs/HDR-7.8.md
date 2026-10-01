# Phase 7.8 — HDR/SDR Processing

Phase 7.8 establishes a real PQ/P010 HDR-to-SDR conversion path in the Vision Engine.

The implementation accepts P010 input, decodes the SMPTE ST 2084/PQ transfer function, converts the YUV representation to RGB, applies a configurable peak-luminance tone map, and encodes the result as sRGB RGBA8888. Timing and frame color metadata are preserved where the current frame model exposes them.

Only PQ input is implemented in this stage. HLG and reverse SDR-to-HDR conversion explicitly return unsupported rather than pretending to support those transfer functions.

The output buffer is owned by the destination frame and is released through the common Vision frame ownership API.

## Scope

Static HDR metadata parsing, dynamic HDR metadata, exact BT.2020/BT.709 color-management matrices, HLG processing, HDR10+ / Dolby Vision handling, and display-specific tone-map calibration are later enhancements.