# Phase 2.7 — Logging & Diagnostics

Phase 2.7 establishes early-observable logging and a machine-readable diagnostics primitive.

## Logging

The logger writes newline-delimited JSON records to an append-only file. Each record contains a UTC timestamp, severity, component, and message. Values are escaped before being written, and each record is emitted with a single write operation.

Default log path: `/run/sheen/logs/system.jsonl`. The logger is usable without the IPC bus.

## Diagnostics

`sheen-diag` reports live kernel/runtime facts including kernel release, kernel command line, OS release data, sysfs availability, and whether the current Sheen hardware inventory has been published. It reports presence rather than fabricating absent subsystems.

## Integration

The root filesystem installs `/usr/sbin/sheen-log` and `/usr/sbin/sheen-diag`. Later runtime stages can route service lifecycle and hardware events into the same logging primitive and expose diagnostics through IPC.

## Validation

`tools/test-logging.sh` compiles the real logger and diagnostics binaries, validates JSON log records, and parses the diagnostics snapshot as JSON.
