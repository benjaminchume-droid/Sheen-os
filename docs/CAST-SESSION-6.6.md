# Phase 6.6 — Casting Session Manager

Phase 6.6 establishes the common lifecycle layer over casting protocol backends.

Sessions have explicit created, connecting, active, stopping, stopped, failed, and unsupported states. A backend supplies `open`, `stop`, and `close` callbacks; the manager owns lifecycle transitions and bounded session slots.

The manager does not claim support for a mode when no backend is supplied, and it propagates backend `ENOTSUP` as an explicit unsupported state.

## Scope

Persistence, peer permission checks, recovery after source loss, session telemetry, multi-stream A/V synchronization, and integration with the public CastManager IPC service are later stages.

## Validation

`tools/test-cast-session.sh` compiles the real session manager with a contract backend and verifies activation, state inspection, counting, stop, and cleanup.