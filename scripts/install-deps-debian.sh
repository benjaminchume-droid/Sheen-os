#!/bin/sh
set -eu

sudo apt-get update
sudo apt-get install -y     build-essential     curl     xz-utils     cpio     busybox-static     grub-efi-amd64-bin     dosfstools     gdisk     util-linux     mtools     e2fsprogs     file
