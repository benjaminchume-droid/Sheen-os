# Sheen OS

Linux-native, USB-first TV operating system for PCs, laptops and future dedicated hardware.

Core targets: bootable USB, media playback, live TV, casting, display processing/upscaling, and compatibility for ordinary Android APKs plus Android TV APKs.

## Architecture

Linux kernel → hardware/runtime services → media/TV/cast/vision/Android subsystems → applications → Sheen shell.

The repository intentionally contains real subsystem boundaries and build contracts rather than simulated demo functionality.
