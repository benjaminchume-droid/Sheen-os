#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
[ -f "$ROOT/system/init/pid1.c" ] || { echo "missing native PID 1 source" >&2; exit 1; }
grep -q "int main(void)" "$ROOT/system/init/pid1.c" || { echo "native PID 1 source is missing main" >&2; exit 1; }
grep -q "getpid() != 1" "$ROOT/system/init/pid1.c" || { echo "PID 1 must verify its process identity" >&2; exit 1; }
grep -q "waitpid(-1" "$ROOT/system/init/pid1.c" || { echo "PID 1 must reap child processes" >&2; exit 1; }
grep -q "SIGTERM" "$ROOT/system/init/pid1.c" || { echo "PID 1 must handle termination signals" >&2; exit 1; }
echo "PID 1 contract valid: $TARGET"
