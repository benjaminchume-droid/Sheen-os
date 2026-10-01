#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$ROOT/out"
KERNEL="$BUILD/kernel"
INITRAMFS="$BUILD/initramfs"
IMAGE="$BUILD/sheen-x86_64-uefi.img"

. "$ROOT/build/config.env"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
"$ROOT/tools/validate-kernel.sh" "$TARGET"

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
make defconfig
cat "$ROOT/$SHEEN_KERNEL_CONFIG_FRAGMENT" >> .config
make olddefconfig
make -j"$(getconf _NPROCESSORS_ONLN)" bzImage
cp arch/x86/boot/bzImage "$KERNEL/bzImage"
cp .config "$KERNEL/.config"
make -s kernelversion > "$KERNEL/kernel.release"
sha256sum .config | cut -d" " -f1 > "$KERNEL/config.sha256"

cd "$ROOT"
mkdir -p "$INITRAMFS/root"/{bin,sbin,usr/bin,usr/sbin,dev,proc,sys,run,tmp}
busybox_path="$(command -v busybox)"
cp "$busybox_path" "$INITRAMFS/root/bin/busybox"
chmod 0755 "$INITRAMFS/root/bin/busybox"

for app in sh mount umount cat echo uname basename; do
    ln -s /bin/busybox "$INITRAMFS/root/bin/$app"
done
cp boot/initramfs/init "$INITRAMFS/root/init"
chmod 0755 "$INITRAMFS/root/init"

(
    cd "$INITRAMFS/root"
    find . -print | cpio -H newc -o
) | gzip -9 > "$INITRAMFS/initramfs.img"

rm -f "$IMAGE"
dd if=/dev/zero of="$IMAGE" bs=1M count="$SHEEN_IMAGE_SIZE_MB" status=none
sgdisk --clear --new=1:2048:0 --typecode=1:ef00 --change-name=1:SHEEN-ESP "$IMAGE"

loop="$(losetup --find --show --partscan "$IMAGE")"
cleanup() {
    set +e
    umount "$BUILD/mnt/efi" 2>/dev/null || true
    losetup -d "$loop" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

udevadm settle 2>/dev/null || true
part="\${loop}p1"
mkfs.vfat -F 32 -n SHEEN "$part" >/dev/null

mkdir -p "$BUILD/mnt/efi"
mount "$part" "$BUILD/mnt/efi"
mkdir -p "$BUILD/mnt/efi/sheen"

grub-install --target=x86_64-efi     --efi-directory="$BUILD/mnt/efi"     --boot-directory="$BUILD/mnt/efi/boot"     --removable --no-nvram --recheck

cp "$KERNEL/bzImage" "$BUILD/mnt/efi/sheen/kernel"
cp "$INITRAMFS/initramfs.img" "$BUILD/mnt/efi/sheen/initramfs.img"
mkdir -p "$BUILD/mnt/efi/boot/grub"
cp "$ROOT/boot/bootloader/grub/grub.cfg" "$BUILD/mnt/efi/boot/grub/grub.cfg"

sync
echo "Built: $IMAGE"
