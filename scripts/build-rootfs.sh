#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
[ "$(id -u)" -eq 0 ] || { echo "rootfs build requires root privileges" >&2; exit 1; }
BUILD="$ROOT/out/$TARGET"
OUT="$BUILD/rootfs"
IMAGE="$OUT/sheen-rootfs.ext4"
MNT="$OUT/mnt"
[ -n "$SHEEN_ROOTFS_SIZE_MB" ] || { echo "missing SHEEN_ROOTFS_SIZE_MB" >&2; exit 1; }
mkdir -p "$OUT" "$MNT"
rm -f "$IMAGE"
dd if=/dev/zero of="$IMAGE" bs=1M count="$SHEEN_ROOTFS_SIZE_MB" status=none
mkfs.ext4 -F -L SHEENROOT "$IMAGE" >/dev/null
cleanup() { set +e; umount "$MNT" 2>/dev/null || true; }
trap cleanup EXIT INT TERM
mount -o loop "$IMAGE" "$MNT"
mkdir -p "$MNT/bin" "$MNT/sbin" "$MNT/usr/bin" "$MNT/usr/sbin" "$MNT/etc" "$MNT/usr/libexec" "$MNT/dev" "$MNT/dev/pts" "$MNT/proc" "$MNT/sys" "$MNT/run" "$MNT/tmp" "$MNT/var" "$MNT/home" "$MNT/root"
busybox_path="$(command -v busybox)"
cp "$busybox_path" "$MNT/bin/busybox"
chmod 0755 "$MNT/bin/busybox"
for app in sh mount umount cat echo uname basename ls dmesg ps reboot; do ln -sf /bin/busybox "$MNT/bin/$app"; done
cp "$ROOT/system/init/pid1.c" "$OUT/pid1.c"
gcc -Os -static -s "$ROOT/system/init/pid1.c" -o "$MNT/sbin/init"
gcc -std=c11 -O2 -static -s "$ROOT/hardware/discovery/device-discovery.c" -o "$MNT/usr/sbin/sheen-hw-discover"
chmod 0755 "$MNT/usr/sbin/sheen-hw-discover"
chmod 0755 "$MNT/sbin/init"
cat > "$MNT/etc/os-release" <<EOF
NAME="Sheen OS"
ID=sheen
PRETTY_NAME="Sheen OS"
VERSION_ID="0.1-dev"
EOF
cat > "$MNT/etc/sheen-release" <<EOF
Sheen OS root filesystem target=$TARGET
EOF
chmod 1777 "$MNT/tmp"
chmod 0700 "$MNT/root"
touch "$MNT/var/.keep"
sync
echo "Built root filesystem: $IMAGE"
