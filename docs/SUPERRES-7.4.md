# Phase 7.4 — Classical Super-Resolution

Phase 7.4 adds a real CPU detail-enhancing upscale path for RGBA/BGRA frames.

The implementation reconstructs the enlarged image with bilinear sampling and adds a locally measured high-frequency residual. Scaling factors of 2× through 4× are supported.

This is explicitly a classical image-processing method, not an ML inference engine. A future model-backed super-resolution backend can implement the same frame contract without changing the Vision pipeline.

Output frames own their allocated buffers and preserve input timing/color metadata.