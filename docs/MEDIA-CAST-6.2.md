# Phase 6.2 — Media Casting

Phase 6.2 adds a real UPnP/DLNA AVTransport media-casting backend.

## Protocol

The sender retrieves the discovered renderer's device description, locates the AVTransport service and its control URL, then sends SOAP `SetAVTransportURI` followed by `Play` to the renderer.

The media-casting session is protocol-specific internally but exposed through a small Sheen session structure so additional senders can later implement Chromecast, DIAL, or other protocols.

## Scope

TLS/authenticated renderers, content metadata injection, pause/seek/volume controls, protocol-specific capability discovery, and Chromecast/WebRTC sender implementations are later casting stages.

## Validation

`tools/test-media-cast.sh` runs a local HTTP endpoint implementing the relevant UPnP description and SOAP actions and verifies the real sender reaches the `playing` state.