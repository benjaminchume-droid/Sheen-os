# Phase 7.1 — Vision Frame Pipeline

Phase 7.1 establishes the common frame representation and processing graph beneath all Vision Engine algorithms.

## Frame model

A frame carries width/height, pixel format, up to four planes, byte size/stride, presentation timestamp, duration, and color metadata. Supported base formats include NV12, YUV420P, P010, RGBA8888, and BGRA8888.

## Pipeline

Processing stages are registered as callbacks and applied in order. The pipeline validates the frame before processing and after every stage, so a stage cannot silently return malformed plane geometry.

The current boundary allows a stage to mutate a caller-owned frame in place. A later allocator/backend layer can add explicitly owned or zero-copy surfaces without changing the graph contract.

## Scope

Scaling, super-resolution, denoising, deblocking, sharpening, HDR/SDR conversion, frame interpolation, GPU acceleration, and adaptive quality selection are subsequent Vision stages.

## Validation

`tools/test-vision-frame.sh` validates a real NV12 frame, executes a registered processing stage, and verifies that the frame remains valid after processing.