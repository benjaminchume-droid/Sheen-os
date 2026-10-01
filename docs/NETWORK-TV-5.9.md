# Phase 5.9 — Network TV

Phase 5.9 establishes network/IPTV channel management above the generic stream and live-session layers.

## Channel model

Network channels are configuration records of `channel_id|name|uri`. The catalog preserves configured IDs, validates the URI scheme, and exposes a bounded channel set.

Current supported transports are HTTP and UDP, using the existing Sheen streaming implementation. Selecting a channel opens a real `sheen_live_session` over that URI instead of introducing a second playback stack.

## Scope

HTTP Live Streaming playlist parsing, MPEG-DASH manifests, RTSP/RTP session negotiation, authentication, provider discovery, and adaptive bitrate policy are further streaming backends. The network-TV layer is intentionally transport-neutral so those can be added without changing channel APIs.

## Validation

`tools/test-network-tv.sh` starts a local HTTP server, loads a real IPTV catalog entry, lists it, opens the selected channel through the live-session layer, and verifies that bytes are actually received.