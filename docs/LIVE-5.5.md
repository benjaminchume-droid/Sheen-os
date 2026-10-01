# Phase 5.5 — Live Playback

Phase 5.5 establishes a transport-backed live playback session abstraction.

## Session lifecycle

Sessions transition through `opening`, `ready`, `playing`, `paused`, `stopped`, and `failed`. A session owns a real `sheen_stream` source and exposes byte reads to the media pipeline.

The session ID is generated from source identity and runtime state, and session byte counters track actual bytes delivered by the transport.

This provides the lower-level session needed by the public `TvManager.watch()` and MediaEngine session contracts; channel selection and decoded-frame presentation can be attached without changing the transport boundary.

## Scope

Hardware tuner→transport wiring, elementary-stream decode, A/V synchronization, timeshift, and the TV shell are subsequent stages.
