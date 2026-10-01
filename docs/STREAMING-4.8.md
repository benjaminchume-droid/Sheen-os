# Phase 4.8 — Streaming

Phase 4.8 establishes the byte-stream transport layer below demuxing.

## Supported source types

- `file://` and absolute paths for local file sources
- `http://` for HTTP/1.1 over TCP with response status and Content-Length handling
- `udp://host:port` for IPv4 UDP unicast/multicast reception

All sources expose the same `sheen_stream_read()` interface. Demuxers therefore consume a transport-neutral byte stream instead of opening sockets or files themselves.

HTTP chunked transfer encoding is currently rejected explicitly rather than mis-parsed; it can be added as a stream backend extension. HTTPS/TLS is also intentionally a separate transport backend because it requires a cryptographic library and certificate policy.

## Validation

`tools/test-streaming.sh` exercises actual file I/O, a local TCP HTTP server, and a local UDP sender against the same stream abstraction.
