#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
FILE="$ROOT/configs/targets/$TARGET.toml"

[ -f "$FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }

grep -q '^schema_version = 1$' "$FILE" || { echo "invalid target schema" >&2; exit 1; }
grep -q "^id = \"$TARGET\"$" "$FILE" || { echo "target id mismatch" >&2; exit 1; }
grep -q '^\[platform\]$' "$FILE" || { echo "missing platform section" >&2; exit 1; }
grep -q '^\[kernel\]$' "$FILE" || { echo "missing kernel section" >&2; exit 1; }
grep -q '^\[image\]$' "$FILE" || { echo "missing image section" >&2; exit 1; }
grep -q '^\[boot\]$' "$FILE" || { echo "missing boot section" >&2; exit 1; }
grep -q '^\[capabilities\]$' "$FILE" || { echo "missing capabilities section" >&2; exit 1; }

arch="$(sed -n 's/^architecture = "\([^"]*\)"$/\1/p' "$FILE" | head -n1)"
firmware="$(sed -n 's/^firmware = "\([^"]*\)"$/\1/p' "$FILE" | head -n1)"
deployment="$(sed -n 's/^deployment = "\([^"]*\)"$/\1/p' "$FILE" | head -n1)"
[ -n "$arch" ] || { echo "missing architecture" >&2; exit 1; }
[ -n "$firmware" ] || { echo "missing firmware" >&2; exit 1; }
[ -n "$deployment" ] || { echo "missing deployment" >&2; exit 1; }

case "$arch" in x86_64|arm64) ;; *) echo "unsupported architecture: $arch" >&2; exit 1;; esac
case "$firmware" in uefi|none) ;; *) echo "unsupported firmware: $firmware" >&2; exit 1;; esac
case "$deployment" in usb|ssd|emmc|nand) ;; *) echo "unsupported deployment: $deployment" >&2; exit 1;; esac

echo "target valid: $TARGET ($arch/$firmware/$deployment)"
