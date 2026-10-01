# Phase 5.3 — Channel Scanning

Phase 5.3 establishes configurable channel scanning above the tuner integration layer.

## Scan plan

Scan plans are plain text records of `frequency_hz bandwidth_hz delivery`. The scanner accepts any supported delivery-system identifier and executes the plan against a real Sheen tuner.

No frequency table is embedded into the scanner. Regional/country plans can be supplied as configuration without changing the executable.

## Runtime

For each plan step, the scanner submits the frequency/bandwidth/delivery request through the real tuner API and records the returned lock status. Discovery of actual services on a locked multiplex is handled by the following TV stages.

## Validation

`tools/test-scan.sh` compiles the scanner and validates parsing of a multi-step DVB-T2 plan. The test does not fake a tuner lock; it only validates configuration parsing. Real hardware scanning requires a compatible DVB frontend.
