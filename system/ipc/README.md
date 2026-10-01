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
