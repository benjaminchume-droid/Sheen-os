# Stage 0.1 — Repository Architecture Foundation

Stage 0.1 defines permanent architectural boundaries before feature implementation.

## Principle

Sheen is an operating-system platform, not a launcher. Hardware and system primitives
remain usable independently of the shell. Feature implementations must use real system
primitives rather than demo-specific hardcoded paths.

## Dependency direction

```
Shell / Applications
        ↓
Public Sheen APIs
        ↓
Domain Services
        ↓
Runtime / IPC / Permissions
        ↓
Hardware Abstraction
        ↓
Linux kernel interfaces
        ↓
Hardware
```

Lower layers must never depend on the TV shell.

## Ownership

| Domain | Owns |
|---|---|
| boot | UEFI, bootloader, initramfs |
| kernel | Linux hardware primitives |
| hardware | discovery and HAL adapters |
| system | PID 1, services, IPC, permissions, power, logging |
| runtime | sessions and shared runtime state |
| media | playback, codecs, demux, streams, subtitles, recording primitives |
| tv | tuners, scanning, channels, EPG, timeshift, DVR |
| casting | discovery, receiver protocols, sessions |
| vision | frame processing and acceleration |
| android | Android runtime/framework adapters |
| applications | user-facing application behavior |
| shell | TV navigation and presentation |
| sdk | public developer contracts |
| tools | build, image, diagnostics |
| tests | verification |

## Cross-domain rules

1. Hardware access goes through the hardware/system boundary.
2. Applications never open kernel devices directly.
3. Shell code never becomes a privileged hardware controller.
4. Media owns generic media primitives; TV consumes them.
5. Casting uses media/display sessions through public services.
6. Vision operates on explicit processing pipelines.
7. Android compatibility is an adapter boundary, not the OS foundation.
8. Public APIs avoid unnecessary private implementation types.
9. Services communicate through explicit IPC contracts.
10. Optional capabilities are discoverable at runtime.
11. Missing optional hardware must not break unrelated subsystems.
12. No subsystem may fake a capability to satisfy a demo.

## Capability model

Hardware and software capabilities are queried rather than assumed. Examples include
display modes, GPU acceleration, decoder profiles, audio outputs, tuner standards,
casting protocols, Android compatibility features, HDR formats and storage capabilities.

Capabilities are reported as available, unavailable or degraded with machine-readable
reason data where appropriate.

## Service lifecycle

```
discover → initialize → ready → active → degraded/failed → recover/stop
```

Long-lived services expose health state and structured diagnostics.

## Contract locations

- `sdk/api/` — public application contracts
- `system/ipc/` — internal service contracts
- `hardware/` — hardware adapter contracts
- `runtime/` — runtime contracts
- domain directories — private implementation interfaces

## Completion criteria

- Ownership and dependency direction are documented.
- Every major boundary has a contract location.
- Initial platform is defined.
- Security and privilege boundaries are documented.
- Stage 1 build is separately documented.
- Architecture validation rejects obvious boundary violations.

Stage 0.1 does not claim a feature is implemented merely because its directory or
contract exists.
