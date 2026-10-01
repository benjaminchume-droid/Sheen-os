#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/rootfs/sheen-rootfs.ext4"
"$ROOT/tools/validate-rootfs.sh" "$TARGET"
command -v e2fsck >/dev/null 2>&1 || { echo "missing host tool: e2fsck" >&2; exit 1; }
command -v debugfs >/dev/null 2>&1 || { echo "missing host tool: debugfs" >&2; exit 1; }
e2fsck -fn "$IMAGE" >/dev/null
for path in /etc/os-release /etc/sheen-release /bin/busybox /bin/sh /sbin/init /dev /proc /sys /run /tmp /var /home /root; do
    debugfs -R "stat $path" "$IMAGE" 2>/dev/null | grep -q "Inode:" || { echo "rootfs missing: $path" >&2; exit 1; }
done
file "$IMAGE"
echo "Root filesystem artifact checks passed: $TARGET"
