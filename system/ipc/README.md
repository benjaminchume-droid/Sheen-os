# Sheen IPC Boundary

System services communicate through explicit IPC contracts.

Required properties:
- service discovery
- request/response
- asynchronous events
- cancellation
- version negotiation
- structured errors
- permission checks
- service health

The concrete transport is an implementation decision for the runtime/system stage.


## Phase 2.5

`ipc-bus.c` is the concrete IpcBus v1 transport. It provides a Unix-domain broker endpoint at `/run/sheen/ipc/bus.sock` with registration, discovery, negotiation, request routing, response correlation, deadlines, and event delivery.
