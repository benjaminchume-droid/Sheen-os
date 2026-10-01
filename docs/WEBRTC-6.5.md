# Phase 6.5 — WebRTC Receiver Foundation

Phase 6.5 establishes real WebRTC negotiation primitives for casting receivers.

## SDP

The parser extracts ICE username/password credentials, audio/video media sections, RTP codec mappings, and ICE candidates from an SDP offer.

## ICE/STUN

The native ICE endpoint listens on UDP and handles STUN Binding Requests, returning a Binding Success response with the peer's XOR-mapped address. This establishes a real connectivity-check primitive without depending on a browser/WebView runtime.

## Boundary

The implementation intentionally stops before DTLS-SRTP and RTP media decryption/decoding. Those require a cryptographic and media transport layer that must be integrated explicitly rather than represented as a fake WebRTC session.

## Validation

`tools/test-webrtc.sh` compiles the actual SDP and ICE code, validates offer parsing, sends a real STUN Binding Request over UDP, and validates the returned transaction/cookie/type fields.