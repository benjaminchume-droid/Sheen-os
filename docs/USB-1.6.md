# Phase 1.6 — USB Image

Phase 1.6 turns the kernel, initramfs, UEFI loader and root filesystem into one actual USB disk image.

## Partition layout

1. GPT partition table
2. FAT32 EFI System Partition labeled SHEEN
3. ext4 Linux root partition labeled SHEENROOT

The x86-64 target reserves a 128 MiB ESP and uses the remainder of the 512 MiB image for the Sheen root partition. The root filesystem is copied into that partition and expanded to the partition boundary.

## Boot contents

The ESP contains the removable UEFI GRUB loader, GRUB configuration, Linux kernel, and initramfs. GRUB passes the persistent root label to the kernel while `rdinit=/init` keeps the early PID-1 handoff under Stage 1 control.

## Validation

tools/test-usb.sh verifies GPT integrity, locates the real second partition, extracts that partition from the generated disk image, runs a read-only ext4 consistency check, and inspects real root filesystem entries.

## Scope

Physical firmware boot remains Stage 1.7. This stage proves construction of the complete USB artifact, not successful execution on a particular machine.
