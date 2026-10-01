# Stage 1.7 — First Physical Boot

Stage 1.7 is the first hardware execution milestone. It is not considered complete from source inspection or CI alone.

## Preflight

After a successful build, run:

```sh
sudo sh ./scripts/build.sh x86_64-uefi-usb
sh ./tools/preflight-first-boot.sh x86_64-uefi-usb
```

The preflight validates the actual generated USB image before it is written to removable media.

## Write the image

On Windows, use a raw-image USB writer such as Rufus or balenaEtcher and select the generated `.img` file. On Linux, a raw block write can be performed with `dd`; identify the USB device carefully before writing because the operation overwrites the selected device.

## Physical test

1. Insert the USB into the target PC or laptop.
2. Enter the firmware boot menu and select the UEFI USB device.
3. Ensure the machine is allowed to boot unsigned GRUB if Secure Boot is enabled; the current development image is not yet production-signed.
4. Verify that GRUB appears, the Linux kernel loads, the initramfs starts, native Sheen PID 1 runs, and the display shows the early Sheen boot diagnostics.
5. Record the machine CPU architecture, firmware mode, GPU, display output, network devices, USB devices, and the exact console output for the compatibility matrix.

## Completion evidence

Stage 1.7 is complete only when a real physical machine has booted the generated image through UEFI and the result is recorded against a specific hardware profile.
