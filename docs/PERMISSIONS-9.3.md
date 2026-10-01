# Phase 9.3 — Application Permissions

Phase 9.3 establishes generic persistent permission policy for native Sheen applications.

Applications are identified by `app_id` and receive explicit permission records for display, audio, input, network, storage, camera, microphone, tuner, casting, and power operations.

Grant, revoke, and check operations are stored in SQLite and default to denied when no grant exists. This gives the runtime a real policy source for later resource enforcement.

Android-specific permission names from Phase 8.7 remain an adapter layer; both can ultimately converge on the same runtime permission enforcement boundary.

## Scope

Namespace enforcement, device-node mediation, capability dropping, IPC authorization, UI prompts, and policy inheritance are later security/platform stages.