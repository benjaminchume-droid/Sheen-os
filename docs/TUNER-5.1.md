# Phase 5.1 — Linux Tuner Integration

Phase 5.1 establishes the real Linux DVB frontend control layer.

## Operations

The tuner API exposes frontend information, delivery-system selection, frequency/bandwidth tuning, and lock-status reads. The implementation uses Linux DVB frontend ioctls rather than shelling out to external tuning programs.

Supported delivery-system identifiers include DVB-T/T2, DVB-C, DVB-S/S2, ATSC, and ISDB-T. Frequencies and bandwidths are request inputs, not hard-coded region policy.

## Scope

Frequency plans, channel scanning, multiplex parsing, service databases, EPG, DVR, and live playback are later TV platform stages.
