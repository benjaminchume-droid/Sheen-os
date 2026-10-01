# Phase 8.7 — Android Permission Bridge

Phase 8.7 establishes persistent mapping between Android manifest permission names and Sheen runtime capabilities.

The policy engine maps network, camera, microphone, location, storage, Bluetooth, notification, wake-lock, and vibration permissions into Sheen capability bits. Grants are stored per package in SQLite and can be granted, revoked, and checked at runtime.

Unknown Android permission strings do not acquire a capability implicitly; the mapping function returns zero and grant/check operations reject unsupported permissions.

## Scope

Runtime prompt UI, permission delegation through Binder, app-op semantics, foreground/background location policy, scoped-storage-specific permissions, notification channels, and per-sandbox OS enforcement remain later security/framework stages.