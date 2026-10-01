# Phase 8.10 — Android Compatibility Capabilities

Phase 8.10 establishes the capability-discovery contract for the Android adapter stack.

The matrix separates features that Sheen implements from features that are available on the current machine. This distinction matters for hardware-dependent features such as Binder devices, DRM graphics, evdev input, and ALSA audio.

Implemented adapter features currently include runtime isolation, Binder, DMA-BUF graphics, evdev input, PCM audio, storage sandboxing, permission policy, Android TV behavior, and APK package handling. Availability is derived from actual filesystem/device state at runtime.

The Android stack can therefore reject or degrade unsupported capabilities without advertising functionality that the current machine cannot provide.

## Scope

Per-device codec capability negotiation, ABI/architecture compatibility, Android API-level reporting, vendor-specific framework behavior, and application-level feature queries remain later compatibility/platform stages.