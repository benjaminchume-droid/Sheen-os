#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/package/include/sheen/android-package.h" ] || { echo "missing Android package API" >&2; exit 1; }
[ -f "$ROOT/android/package/package-manager.c" ] || { echo "missing APK package manager" >&2; exit 1; }
grep -q "AndroidManifest.xml" "$ROOT/android/package/package-manager.c" || { echo "manifest inspection missing" >&2; exit 1; }
grep -q 'dex\\n035' "$ROOT/android/package/package-manager.c" || { echo "DEX validation missing" >&2; exit 1; }
grep -q "copy_atomic" "$ROOT/android/package/package-manager.c" || { echo "transactional staging missing" >&2; exit 1; }
grep -q "CREATE TABLE IF NOT EXISTS packages" "$ROOT/android/package/package-manager.c" || { echo "package persistence missing" >&2; exit 1; }
echo "Android APK package manager contract valid"
