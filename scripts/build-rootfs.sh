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
mkdir -p "$MNT/bin" "$MNT/sbin" "$MNT/usr/bin" "$MNT/usr/sbin" "$MNT/usr/libexec" "$MNT/usr/lib" "$MNT/etc/sheen/services"
mkdir -p "$BUILD/media" "$MNT/usr/include/sheen" "$MNT/dev" "$MNT/dev/pts" "$MNT/proc" "$MNT/sys" "$MNT/run" "$MNT/tmp" "$MNT/var" "$MNT/home" "$MNT/root"
busybox_path="$(command -v busybox)"
cp "$busybox_path" "$MNT/bin/busybox"
chmod 0755 "$MNT/bin/busybox"
for app in sh mount umount cat echo uname basename ls dmesg ps reboot; do ln -sf /bin/busybox "$MNT/bin/$app"; done
cp "$ROOT/system/init/pid1.c" "$OUT/pid1.c"
gcc -Os -static -s "$ROOT/system/init/pid1.c" -o "$MNT/sbin/init"
gcc -std=c11 -O2 -static -s -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/discovery/device-discovery.c" -o "$MNT/usr/sbin/sheen-hw-discover"
gcc -std=c11 -O2 -static -s "$ROOT/system/hardware/device-manager.c" -o "$MNT/usr/libexec/sheen-device-manager"
chmod 0755 "$MNT/usr/sbin/sheen-hw-discover"
gcc -std=c11 -O2 -static -s "$ROOT/system/services/service-manager.c" -o "$MNT/usr/libexec/sheen-service-manager"
gcc -std=c11 -O2 -static -s "$ROOT/system/ipc/ipc-bus.c" -o "$MNT/usr/libexec/sheen-ipc-bus"
gcc -std=c11 -O2 -static -s -I"$ROOT/system/config/include" "$ROOT/system/config/config.c" "$ROOT/system/config/config-cli.c" -o "$MNT/usr/sbin/sheen-config"
gcc -std=c11 -O2 -static -s -I"$ROOT/system/logging/include" "$ROOT/system/logging/log.c" "$ROOT/system/logging/log-cli.c" -o "$MNT/usr/sbin/sheen-log"
chmod 0755 "$MNT/usr/sbin/sheen-log"
gcc -std=c11 -O2 -static -s "$ROOT/system/diagnostics/diagnose.c" -o "$MNT/usr/sbin/sheen-diag"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s "$ROOT/hardware/graphics/drm-probe.c" -o "$MNT/usr/sbin/sheen-drm-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/graphics/gpu-probe.c" -o "$MNT/usr/sbin/sheen-gpu-probe"
chmod 0755 "$MNT/usr/sbin/sheen-gpu-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/audio/alsa-probe.c" -o "$MNT/usr/sbin/sheen-alsa-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s "$ROOT/hardware/input/evdev-probe.c" -o "$MNT/usr/sbin/sheen-evdev-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/tuners/dvb-probe.c" -o "$MNT/usr/sbin/sheen-dvb-probe"
chmod 0755 "$MNT/usr/sbin/sheen-dvb-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" "$ROOT/media/demux/container.c" "$ROOT/media/demux/container-probe.c" -o "$MNT/usr/sbin/sheen-container-probe"
chmod 0755 "$MNT/usr/sbin/sheen-container-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s "$ROOT/media/codecs/v4l2-codec-probe.c" -o "$MNT/usr/sbin/sheen-v4l2-codec-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s "$ROOT/media/codecs/v4l2-decoder-session.c" -o "$MNT/usr/sbin/sheen-v4l2-decoder-session"
chmod 0755 "$MNT/usr/sbin/sheen-v4l2-decoder-session"
chmod 0755 "$MNT/usr/sbin/sheen-v4l2-codec-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" "$ROOT/media/demux/mpegts-demux.c" "$ROOT/media/demux/demux-probe.c" -o "$MNT/usr/sbin/sheen-mpegts-demux"
chmod 0755 "$MNT/usr/sbin/sheen-mpegts-demux"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/playback/include" -c "$ROOT/media/playback/decode-pipeline.c" -o "$BUILD/decode-pipeline.o"
ar rcs "$MNT/usr/lib/libsheen-decode.a" "$BUILD/decode-pipeline.o"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/audio/include" -c "$ROOT/media/audio/audio-pipeline.c" -o "$BUILD/media/audio-pipeline.o"
ar rcs "$MNT/usr/lib/libsheen-audio.a" "$BUILD/media/audio-pipeline.o"
chmod 0755 "$MNT/usr/sbin/sheen-evdev-probe"
chmod 0755 "$MNT/usr/sbin/sheen-alsa-probe"
gcc -std=c11 -O2 -Wall -Wextra -Werror -static -s "$ROOT/system/display/display-manager.c" -o "$MNT/usr/libexec/sheen-display-manager"
chmod 0755 "$MNT/usr/libexec/sheen-display-manager"
cp "$ROOT/configs/services/display-manager.conf" "$MNT/etc/sheen/services/display-manager.conf"
chmod 0755 "$MNT/usr/sbin/sheen-drm-probe"
chmod 0755 "$MNT/usr/sbin/sheen-diag"
chmod 0755 "$MNT/usr/sbin/sheen-config"
chmod 0755 "$MNT/usr/libexec/sheen-ipc-bus"
cp "$ROOT/configs/services/ipc-bus.conf" "$MNT/etc/sheen/services/ipc-bus.conf"
chmod 0755 "$MNT/usr/libexec/sheen-service-manager"
cp "$ROOT/configs/services/device-manager.conf" "$MNT/etc/sheen/services/device-manager.conf"
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
