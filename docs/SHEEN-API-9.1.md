# Phase 9.1 — Public Sheen API

Phase 9.1 exposes a public native client library above the private IPC transport.

Applications use `sheen_api_connect()` and `sheen_api_request()` with service/operation/payload values. The client handles Unix-socket setup, JSONL framing, request IDs, deadlines, response correlation, and event retrieval internally.

The API intentionally remains generic: it does not bake display, media, TV, or casting workflows into the client. Those are service contracts discovered and called through the same transport.

## Scope

Native application lifecycle, permission dispatch, sandboxing, package management, developer tooling, and language bindings are subsequent Phase 9 stages.