# Phase 8.9 — APK Package Manager Foundation

Phase 8.9 establishes real APK inspection and transactional package staging.

## APK validation

The package manager reads the APK ZIP central directory, extracts `AndroidManifest.xml`, parses the binary Android XML string pool/start-element structures to obtain the package name and Leanback TV feature, and validates that `classes.dex` contains an Android DEX header.

## Installation

Installation creates a package-specific directory and copies the original APK into `base.apk` through a temporary file, `fsync`, close, and atomic rename. Package metadata is persisted in SQLite so package discovery survives reboot.

This stage deliberately stops short of executing APK code. Signature verification, split APK/APKS support, native library extraction, ABI selection, dex optimization, Android PackageManager service semantics, and application launch belong to later compatibility/security work.

## Validation

`tools/test-android-package.sh` constructs a binary Android XML/APK fixture, inspects its manifest/Dex/TV feature, stages the real ZIP, and verifies the persistent package record and installed file.