# Phase 7.9 — Frame Processing

Phase 7.9 establishes timestamp-driven frame cadence control below the Vision pipeline.

The cadence processor converts a target frame-rate ratio into a nanosecond interval and classifies incoming frame timestamps as KEEP, DROP, or DUPLICATE. Duplicate counts are bounded to prevent an abnormal timestamp gap from causing unbounded frame generation.

This is a timing primitive, not a motion-interpolation algorithm. It does not invent image content; DUPLICATE means the downstream scheduler may present the same decoded frame for additional target intervals.

## Scope

Frame interpolation, optical-flow motion estimation, cadence-aware audio synchronization, presentation-vsync integration, and display clock correction are later stages.