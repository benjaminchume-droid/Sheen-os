# Phase 8.1 — Android Runtime Isolation Foundation

Phase 8.1 establishes the isolated userspace boundary in which a genuine Android system image can run alongside Sheen rather than becoming the base OS.

## Runtime boundary

The runtime verifies an Android root filesystem and requires an executable init plus `system`, `vendor`, and `apex` directories. A runtime instance is launched in Linux mount, PID, and UTS namespaces, then chrooted into the Android root filesystem before executing its init process.

The runtime mounts procfs and sysfs inside the guest namespace and supplies an explicit Android-runtime environment marker. Sheen remains the host OS and owns the outer kernel/services boundary.

## Current status

This stage establishes the actual isolation mechanism, but it does not claim that an Android system image is shipped yet. APK execution still depends on a compatible Android userspace/framework image and subsequent framework, graphics, input, audio, storage, permission, TV, and package-manager stages.

An incomplete Android root filesystem is rejected instead of being treated as runnable Android.

## Validation

`tools/test-android-runtime.sh` compiles the native runtime and verifies that an incomplete image is explicitly rejected.