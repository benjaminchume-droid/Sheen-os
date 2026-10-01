# Phase 6.1 — Casting Discovery

Phase 6.1 establishes LAN discovery for casting-capable devices using the SSDP/UPnP discovery protocol.

## Transport

The discovery engine sends a real SSDP `M-SEARCH` request to the standard multicast address `239.255.255.250:1900` and parses returned HTTP-like headers such as `LOCATION`, `ST`, `USN`, and `SERVER`.

Responses are normalized into the Sheen CastManager device model with device identity, network address, and discovered protocol capability information.

The implementation does not manufacture devices when the network has no responses. A zero-device result is valid.

## Scope

mDNS/Bonjour discovery, DIAL, Chromecast-specific service discovery, capability enrichment from device descriptions, and lost-device event tracking are subsequent casting stages.