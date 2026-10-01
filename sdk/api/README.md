# Sheen Public API Boundary

Public APIs exposed to native applications and SDK clients live here.

## Versioning

The current normative public contract set is sdk/api/v1/.

Each v1 contract is transport-neutral and describes operations, responses, events, capabilities, and resource lifecycle semantics.

Rules:
- APIs describe capabilities and behavior, not private implementation details.
- APIs are versioned.
- Optional hardware features are capability-discoverable.
- Long-running operations support cancellation or session lifecycle where appropriate.
- Events and structured errors are explicit and versioned.
- Unsupported capabilities must be reported, never simulated.
- Implementations must preserve the dependency direction documented by Stage 0.1.

See sdk/api/CONTRACTS.md and docs/API-CONTRACTS-0.4.md.
