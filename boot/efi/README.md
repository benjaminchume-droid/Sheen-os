EFI assets and boot configuration belong here.

## Phase 1.2

The x86-64 USB target uses a FAT32 EFI System Partition and GRUB in removable-media mode. The firmware fallback loader is installed at EFI/BOOT/BOOTX64.EFI, so the image does not depend on an NVRAM boot entry.
