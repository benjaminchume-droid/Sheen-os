# Sheen System Hardware Services

Phase 2.3 provides the native device manager at `system/hardware/device-manager.c`. It owns the live hardware inventory and consumes kernel uevents to trigger real discovery rescans.
