# Phase 6.7 — Multi-device Management

Phase 6.7 establishes persistent device state above transient casting discovery.

## Registry

Each device is keyed by its discovered device ID and stores name, address, port, capabilities, last-seen timestamp, and lifecycle state. Rediscovery upserts the existing identity rather than creating duplicates.

A missing device can be marked removed and later restored by a subsequent discovery result. Active-device counting excludes removed entries.

## Boundary

The registry is intentionally independent from the protocol scanner. SSDP/mDNS/other discovery adapters provide observations; the registry provides durable state to the CastManager and session layers.

## Validation

`tools/test-cast-devices.sh` exercises insert, count, removal, rediscovery, and persistent state retrieval against a real SQLite database.