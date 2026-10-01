#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
KERNEL="$BUILD/kernel"
INITRAMFS="$BUILD/initramfs"
IMAGE="$BUILD/sheen-$TARGET.img"

. "$ROOT/build/config.env"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
sh "$ROOT/tools/validate-kernel.sh" "$TARGET"

need() {
    command -v "$1" >/dev/null 2>&1 || { echo "missing host tool: $1" >&2; exit 1; }
}
for tool in curl sha256sum xz tar make gcc ld cpio grub-install mkfs.vfat sgdisk dd mount umount losetup udevadm; do
    need "$tool"
done

rm -rf "$BUILD"
mkdir -p "$KERNEL" "$INITRAMFS" "$BUILD/src"

archive="$BUILD/src/linux-$SHEEN_KERNEL_VERSION.tar.xz"
src="$BUILD/src/linux-$SHEEN_KERNEL_VERSION"

curl -L --fail --retry 3 "$SHEEN_KERNEL_URL" -o "$archive"
checksum_file="$BUILD/src/sha256sums.asc"
curl -L --fail --retry 3 https://cdn.kernel.org/pub/linux/kernel/v6.x/sha256sums.asc -o "$checksum_file"
grep "linux-$SHEEN_KERNEL_VERSION.tar.xz$" "$checksum_file" | sed "s#linux-$SHEEN_KERNEL_VERSION.tar.xz#$archive#" | sha256sum -c -

tar -xJf "$archive" -C "$BUILD/src"

cd "$src"
make mrproper
cp "$ROOT/$SHEEN_KERNEL_DEFCONFIG" .config
cat "$ROOT/$SHEEN_KERNEL_CONFIG_FRAGMENT" >> .config
make olddefconfig
make -j"$(getconf _NPROCESSORS_ONLN)" bzImage
cp arch/x86/boot/bzImage "$KERNEL/bzImage"
cp .config "$KERNEL/.config"
make -s kernelversion > "$KERNEL/kernel.release"
sha256sum .config | cut -d" " -f1 > "$KERNEL/config.sha256"

cd "$ROOT"
sh "$ROOT/scripts/build-rootfs.sh" "$TARGET"
mkdir -p "$INITRAMFS/root/bin" "$INITRAMFS/root/sbin" "$INITRAMFS/root/usr/bin" "$INITRAMFS/root/usr/sbin"
mkdir -p "$INITRAMFS/root/dev/pts" "$INITRAMFS/root/proc" "$INITRAMFS/root/sys" "$INITRAMFS/root/run" "$INITRAMFS/root/tmp" "$INITRAMFS/root/etc"
busybox_path="$(command -v busybox)"
cp "$busybox_path" "$INITRAMFS/root/bin/busybox"
chmod 0755 "$INITRAMFS/root/bin/busybox"

for app in sh mount umount cat echo uname basename; do
    ln -s /bin/busybox "$INITRAMFS/root/bin/$app"
done
gcc -Os -static -s "$ROOT/system/init/pid1.c" -o "$INITRAMFS/root/init"
chmod 0755 "$INITRAMFS/root/init"
printf '%s\n' /dev /dev/pts /proc /sys /run /tmp /bin /sbin /usr/bin /usr/sbin /etc > "$INITRAMFS/root/etc/initramfs.dirs"

(
    cd "$INITRAMFS/root"
    find . -print | cpio -H newc -o
) | gzip -9 > "$INITRAMFS/initramfs.img"

rm -f "$IMAGE"
dd if=/dev/zero of="$IMAGE" bs=1M count="$SHEEN_IMAGE_SIZE_MB" status=none
sgdisk --clear \
    --new=1:2048:+${SHEEN_ESP_SIZE_MB}M --typecode=1:ef00 --change-name=1:SHEEN-ESP \
    --new=2:0:0 --typecode="$SHEEN_ROOT_PARTITION_TYPE" --change-name=2:SHEEN-ROOT \
    "$IMAGE"

loop="$(losetup --find --show --partscan "$IMAGE")"
cleanup() {
    set +e
    umount "$BUILD/mnt/efi" 2>/dev/null || true
    losetup -d "$loop" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

udevadm settle 2>/dev/null || true
esp_part="${loop}p1"
root_part="${loop}p2"
mkfs.vfat -F 32 -n SHEEN "$esp_part" >/dev/null

mkdir -p "$BUILD/mnt/efi"
mount "$esp_part" "$BUILD/mnt/efi"
mkdir -p "$BUILD/mnt/efi/sheen"

grub-install --target="$SHEEN_GRUB_TARGET" --efi-directory="$BUILD/mnt/efi"     --boot-directory="$BUILD/mnt/efi/boot"     --removable --no-nvram --recheck

cp "$KERNEL/bzImage" "$BUILD/mnt/efi/sheen/kernel"
cp "$INITRAMFS/initramfs.img" "$BUILD/mnt/efi/sheen/initramfs.img"
mkdir -p "$BUILD/mnt/efi/boot/grub"
cp "$ROOT/boot/bootloader/grub/grub.cfg" "$BUILD/mnt/efi/boot/grub/grub.cfg"

# Install the real ext4 root filesystem into the second GPT partition and expand it.
e2fsck -fy "$BUILD/rootfs/sheen-rootfs.ext4" >/dev/null
dd if="$BUILD/rootfs/sheen-rootfs.ext4" of="$root_part" bs=4M conv=fsync status=none
e2fsck -fy "$root_part" >/dev/null
resize2fs "$root_part" >/dev/null

cat > "$BUILD/BUILD-METADATA" <<EOF
target=$TARGET
kernel=$SHEEN_KERNEL_VERSION
image=$IMAGE
EOF
sync
echo "Built: $IMAGE"
