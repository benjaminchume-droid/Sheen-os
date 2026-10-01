# Phase 3.3 — Display Manager

Phase 3.3 establishes the first domain service responsible for display enumeration.

## Service contract

The display manager registers as `display-manager` version `1.0` on the Sheen IPC bus. It exposes capability information and a `get_displays`/`probe` operation.

The manager invokes the real DRM KMS probe rather than carrying its own vendor-specific device inventory. Its responses therefore reflect the actual kernel-exposed display hardware at request time.

## Service definition

`configs/services/display-manager.conf` enables the service for the Sheen service manager. The executable is installed at `/usr/libexec/sheen-display-manager`.

## Scope

Mode setting, display ownership, framebuffer/surface allocation, compositor integration, hotplug event propagation, and user-facing display policy are later stages.
