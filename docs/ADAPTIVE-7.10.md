# Phase 7.10 — Adaptive Pipeline

Phase 7.10 establishes a general-purpose Vision stage planner driven by actual runtime capabilities, quality targets, and latency budgets.

Each registered stage declares required capabilities, a quality contribution, estimated cost, estimated latency, and enabled state. The planner filters out incompatible stages and skips stages that would violate the configured latency budget.

The planner is intentionally generic: it does not contain a hard-coded upscale/denoise/HDR recipe. Existing Vision stages can be registered by policy, and future hardware/model stages can participate through the same descriptor contract.

## Scope

Runtime performance measurement, thermal/power feedback, dynamic quality re-planning, frame-history analysis, and hardware-specific cost calibration remain later optimization layers.