# Build

Initial target: x86-64 UEFI USB image.

The build system is deliberately staged: host tools → kernel/config → root filesystem → initramfs → Sheen services → applications → image assembly.

No graphical shell is required for the first boot milestone.
