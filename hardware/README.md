# Sheen Hardware Boundary

Hardware adapters translate Linux/kernel interfaces into stable Sheen capabilities.

Hardware code may contain architecture/device-specific logic. Upper layers consume
normalized capabilities and service contracts instead of probing kernel devices directly.

Initial target: x86-64 UEFI PCs/laptops.


## Device discovery

Phase 2.1 provides `hardware/discovery/device-discovery.c`, a Linux sysfs-driven discovery primitive installed in the real root filesystem as `/usr/sbin/sheen-hw-discover`. Upper layers consume its normalized records; applications never probe kernel device nodes directly.
