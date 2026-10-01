# Build

## Stage 1

The first supported target is an x86-64 UEFI machine booting from a USB image.

On Debian/Ubuntu:

```sh
./scripts/install-deps-debian.sh
sudo ./scripts/build.sh
./scripts/test.sh
```

The resulting image is `out/sheen-x86_64-uefi.img`.

Stage 1 contains a real Linux kernel, removable UEFI bootloader, static BusyBox,
Sheen PID 1/initramfs and basic hardware enumeration. It does not yet contain the
final TV shell, media engine, tuner stack, Android compatibility layer, casting stack,
or Vision Engine.
