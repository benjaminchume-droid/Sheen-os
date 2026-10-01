# Sheen Interface Registry

These are Stage 0.1 stable subsystem boundaries. They are contracts, not implementations.

## HardwareManager
Enumerate devices, identify capabilities, subscribe to device changes, expose health.

## ServiceManager
Register services, start/stop/restart services, declare dependencies, expose health.

## IpcBus
Service discovery, request/response, events, cancellation and version negotiation.

## PermissionManager
Declare capabilities, grant/revoke access and authorize operations.

## SessionManager
Create sessions, attach resources, emit lifecycle events and release resources.

## DisplayManager
Enumerate displays, negotiate modes/color capabilities and create output sessions.

## AudioManager
Enumerate inputs/outputs, route audio, negotiate formats and control volume/mute.

## MediaEngine
Inspect sources, demux, decode, synchronize, seek, subtitles, streaming and recording
primitives.

## TvManager
Enumerate tuners, scan, expose channels/services, EPG, live sessions, timeshift and DVR.

## CastManager
Discovery, receiver sessions, protocol negotiation and media/display session control.

## VisionEngine
Query capabilities, negotiate frame pipelines, process frames and expose performance state.

## AndroidManager
Runtime availability, APK install/update/remove, capability discovery and application
lifecycle plus bridges into Sheen services.

## StorageManager
Enumerate volumes, mount/unmount, expose capabilities and manage persistent storage.

## InputManager
Enumerate input devices and normalize remote, keyboard, gamepad and touch events.

Concrete transport and ABI choices are finalized during implementation.
