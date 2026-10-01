# Sheen Platform Targets

## Initial target

- Architecture: x86-64
- Firmware: UEFI
- Boot medium: USB
- Deployment: PC/laptop

Baseline capabilities include EFI boot, PCI/PCIe, USB, storage, DRM/KMS display,
basic audio, networking where available and Linux device discovery.

## Future targets

### x86-64 installed
Internal SSD/NVMe deployment using the same upper-layer contracts.

### ARM64
Dedicated TV/box hardware using the same upper-layer APIs.

### Dedicated Sheen hardware
Controlled hardware profiles with known GPU/media acceleration, HDMI, audio, networking
and optional tuner capabilities.

## Portability rule

Architecture-specific code belongs below stable system/runtime contracts. Adding a CPU
architecture or board should primarily require kernel and hardware adapter work, not
application rewrites.
