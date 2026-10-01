# Sheen Public API Boundary

Public APIs exposed to native applications and SDK clients live here.

Rules:
- APIs describe capabilities and behavior, not private implementation details.
- APIs are versioned.
- Optional hardware features are capability-discoverable.
- Long-running operations support cancellation where appropriate.
- Events and structured errors are explicit and versioned.
