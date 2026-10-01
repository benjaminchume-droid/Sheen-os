# Sheen Hardware Boundary

Hardware adapters translate Linux/kernel interfaces into stable Sheen capabilities.

Hardware code may contain architecture/device-specific logic. Upper layers consume
normalized capabilities and service contracts instead of probing kernel devices directly.

Initial target: x86-64 UEFI PCs/laptops.
