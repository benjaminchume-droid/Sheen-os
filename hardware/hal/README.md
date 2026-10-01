# Sheen Hardware Abstraction Layer

Phase 2.2 establishes a reusable C boundary between Linux hardware exposure and Sheen subsystems.

The HAL currently provides sysfs availability checks, bounded path construction, text-attribute reads, symlink resolution, normalized device identity, and device-state representation.

The HAL is intentionally backend-oriented: callers receive stable Sheen structures and helpers, while Linux sysfs mechanics remain inside the hardware layer. Later adapters can add backend implementations without changing application-level contracts.
