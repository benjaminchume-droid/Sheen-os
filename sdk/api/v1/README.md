# Sheen API Contracts v1

Version 1 defines the stable, transport-neutral contracts between Sheen services,
the runtime, and applications.

## Contract model

Every API is expressed as:
- operations: request/response interactions
- events: asynchronous state changes
- capabilities: runtime-discoverable optional functionality
- errors: structured, stable error categories
- lifecycle: resource state transitions where applicable

These contracts do not prescribe an IPC transport, programming language, UI, codec,
driver, Android implementation, or hardware vendor.

## Compatibility

- Adding an optional field is backward-compatible.
- Removing or changing the meaning of an existing field requires a new major API version.
- Unknown fields must be ignored unless a contract explicitly marks them as required.
- Unknown capability identifiers must not cause service failure.
- Implementations must report unsupported operations rather than simulate them.

## Files

Each JSON file is the normative v1 contract for one public subsystem.
