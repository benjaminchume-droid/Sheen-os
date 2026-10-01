# Phase 5.7 — Timeshift

Phase 5.7 establishes a persistent circular live-TV buffer independent of the UI or tuner implementation.

## Storage model

Incoming transport bytes are stored in timestamped segment files. Segments rotate at a configurable byte threshold. When retained data exceeds the configured maximum, the oldest complete segment is evicted.

## Playback model

The buffer exposes oldest/newest presentation timestamps, timestamp-based seek, sequential reads across segment boundaries, and live-edge detection. This provides the storage primitive required for pause-live/resume-live behavior.

## Durability

Active segments are fsynced after append operations. Segment files are ordinary filesystem objects and can therefore be placed on persistent storage instead of requiring a memory-only cache.

## Scope

5.7 does not yet connect the buffer to the TV Manager session lifecycle; that integration will attach pause/resume/seek commands to a live transport session. Retention policy UI and disk quota management are later layers.