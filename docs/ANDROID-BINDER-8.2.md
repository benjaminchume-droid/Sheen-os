# Phase 8.2 — Android Framework Bridge Foundation

Phase 8.2 establishes the real Linux Binder IPC boundary required by an Android userspace.

The Sheen kernel now enables Android Binder IPC and binderfs. The native Binder bridge probes the actual Binder devices, queries `BINDER_VERSION`, opens an exposed Binder device, and provides a binderfs mount primitive for the Android runtime namespace.

The bridge does not fabricate framework services. If no Binder device is available on the target, the probe reports unavailable explicitly.

## Scope

Android servicemanager/framework-service registration, Binder transaction marshalling, Java framework API compatibility, graphics SurfaceFlinger integration, permissions, storage, and package management remain subsequent compatibility stages.