# Phase 1.2 — UEFI Boot

Phase 1.2 establishes a real removable-media UEFI boot contract for the x86-64 Sheen target.

## Boot contract

- firmware: UEFI
- architecture: x86-64
- deployment: USB
- bootloader: GRUB
- removable boot: required
- NVRAM registration: not required
- EFI System Partition: FAT32, label SHEEN
- EFI fallback loader: EFI/BOOT/BOOTX64.EFI
- boot configuration: boot/grub/grub.cfg

The build invokes GRUB with the removable/no-NVRAM mode so the USB image is self-contained and can use the standard x86-64 UEFI removable-media fallback path.

## GRUB handoff

GRUB loads the Sheen kernel from /sheen/kernel and the early userspace image from /sheen/initramfs.img. The kernel command line selects /init as early userspace.

## Validation

tools/validate-uefi.sh validates the target and source boot contract. tools/test-uefi.sh validates a built image structurally: GPT integrity, FAT32 ESP access, EFI fallback loader presence, GRUB configuration, kernel, and initramfs. The checks inspect the actual image contents; they do not simulate firmware behavior.

## Scope

Phase 1.2 proves the UEFI boot artifact contract. It does not claim successful physical firmware boot. Physical boot remains Stage 1.7.
