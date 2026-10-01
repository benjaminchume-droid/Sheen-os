# Sheen Hardware Boundary

Hardware adapters translate Linux/kernel interfaces into stable Sheen capabilities.

Hardware code may contain architecture/device-specific logic. Upper layers consume
normalized capabilities and service contracts instead of probing kernel devices directly.

Initial target: x86-64 UEFI PCs/laptops.


## Device discovery

Phase 2.1 provides `hardware/discovery/device-discovery.c`, a Linux sysfs-driven discovery primitive installed in the real root filesystem as `/usr/sbin/sheen-hw-discover`. Upper layers consume its normalized records; applications never probe kernel device nodes directly.


## Hardware abstraction layer

`hardware/hal/` provides the reusable low-level interface for sysfs access, device identity, and normalized device state. Domain services consume this boundary instead of implementing their own Linux path handling.


## DRM/KMS

Phase 3.1 provides `hardware/graphics/drm-probe.c`, the Linux DRM/KMS discovery primitive used by later display management. It accesses DRM through the hardware boundary rather than application code.
