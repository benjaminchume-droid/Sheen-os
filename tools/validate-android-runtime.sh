#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/runtime/include/sheen/android-runtime.h" ] || { echo "missing Android runtime API" >&2; exit 1; }
[ -f "$ROOT/android/runtime/runtime.c" ] || { echo "missing Android runtime implementation" >&2; exit 1; }
grep -q "CLONE_NEWPID" "$ROOT/android/runtime/runtime.c" || { echo "PID namespace isolation missing" >&2; exit 1; }
grep -q "CLONE_NEWNS" "$ROOT/android/runtime/runtime.c" || { echo "mount namespace isolation missing" >&2; exit 1; }
grep -q "chroot" "$ROOT/android/runtime/runtime.c" || { echo "Android rootfs boundary missing" >&2; exit 1; }
grep -q '"system"' "$ROOT/android/runtime/runtime.c" || { echo "Android system partition validation missing" >&2; exit 1; }
grep -q '"vendor"' "$ROOT/android/runtime/runtime.c" || { echo "Android vendor partition validation missing" >&2; exit 1; }
echo "Android runtime isolation contract valid"
