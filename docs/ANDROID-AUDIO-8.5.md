# Phase 8.5 — Android Audio Bridge

Phase 8.5 establishes Android PCM output as an adapter over the existing Sheen audio pipeline.

Android PCM16 and float streams are translated to Sheen sample formats while preserving sample rate, channel count, frame count, and presentation timestamps. The bridge forwards backpressure and flush semantics from the underlying Sheen sink instead of implementing a separate mixer/output stack.

## Scope

AudioTrack/AAudio API compatibility, Android mixer semantics, device routing, spatial audio, Bluetooth routing, audio focus, and clock synchronization remain later framework/audio stages.