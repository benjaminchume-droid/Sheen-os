# Stage 0.4 — API Contracts

Stage 0.4 converts the Stage 0.1 subsystem boundaries into versioned, transport-neutral contracts.

## Contract rules

1. APIs describe real capabilities; they never promise simulated behavior.
2. Services expose capability discovery for optional hardware and implementations.
3. Application-facing calls cross the public API and permission boundaries.
4. Service-to-service calls use IPC rather than direct kernel/device access.
5. Long-running work is represented by resources or sessions and asynchronous events.
6. Errors are structured and machine-readable.
7. API evolution is additive within a major version.
8. Unsupported operations return explicit errors.
9. Hardware-specific details remain below the hardware boundary.
10. UI behavior is not part of the v1 system contracts.

## Generic lifecycle

discover → negotiate → authorize → request → execute → response/event → release

A service may enter degraded, failed, or recovering state without taking unrelated services down.

## Transport and ABI

No C ABI, socket protocol, D-Bus dependency, protobuf format, or language binding is locked in at Stage 0.4. Runtime and IPC implementations must implement these semantic contracts rather than redefine them.

## Android boundary

Android is an optional compatibility subsystem. Its contract reports actual runtime availability and capabilities. APK compatibility must never be represented as available when the runtime is absent.

## Domain ownership

TV owns tuner, channel, EPG, timeshift, and DVR policy. Media owns generic demux, decode, playback, and recording primitives. Vision owns frame-processing pipelines. Casting owns discovery and receiver sessions.
