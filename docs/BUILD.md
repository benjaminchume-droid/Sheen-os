# Build

## Stage 0.2

The build system is target-driven and reproducible within the declared host toolchain.

Canonical target: `x86_64-uefi-usb`

```sh
sudo sh ./scripts/build.sh x86_64-uefi-usb
sh ./scripts/test.sh x86_64-uefi-usb
```

Artifact:
`out/x86_64-uefi-usb/sheen-x86_64-uefi-usb.img`

Target definitions live in `build/targets/`. Outputs are isolated per target.

## Stage 1 contents

Stage 1 contains a real Linux kernel, removable UEFI bootloader, static BusyBox,
Sheen PID 1/initramfs and basic hardware enumeration. It does not yet contain the
final TV shell, media engine, tuner stack, Android compatibility layer, casting stack,
or Vision Engine.

## Build invariants

- kernel archive is checksum verified
- target is explicit
- output is target-isolated
- initramfs ordering is locale-independent
- artifact paths are deterministic
- CI uses the same scripts
