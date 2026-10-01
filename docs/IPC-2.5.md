# Phase 2.5 — IPC

Phase 2.5 establishes the concrete transport for the Sheen IpcBus contract.

## Transport

The implementation uses Unix-domain `SOCK_STREAM` sockets with newline-delimited JSON envelopes. Each frame is one complete JSON object; the payload field remains an opaque JSON object for the routed service.

## Broker responsibilities

- service registration
- service discovery
- version negotiation
- request routing
- request/response correlation by request ID
- deadline expiry
- event broadcast
- atomic socket-path setup and cleanup

The default bus endpoint is `/run/sheen/ipc/bus.sock`.

## Service model

Services register themselves on the bus connection. Client requests are forwarded to the registered service connection without the broker needing to understand the domain-specific payload. This preserves the transport-neutral API contracts established in Phase 0.4.

## Validation

`tools/test-ipc-bus.sh` performs a real end-to-end integration test using separate service and client Unix sockets. It exercises registration, discovery, negotiation, request forwarding, response correlation, and event delivery.

## Scope

Production authentication, authorization, peer credentials, capability enforcement, quotas, and crash recovery are later permission/security/runtime stages.
