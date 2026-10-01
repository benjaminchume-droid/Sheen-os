# Phase 2.6 — Configuration System

Phase 2.6 establishes a reusable configuration store for Sheen system and runtime components.

## Format

Configuration files use sections (`[section]`) and `key=value` entries. Values remain strings at the storage layer so later services can define their own typed interpretation without coupling the core store to domain policy.

## Operations

- load an existing configuration
- lookup a section/key pair
- insert or update a value
- dump the current normalized configuration
- persist changes through an atomic temporary-file + `rename()` replacement

The storage layer does not execute commands, interpolate shell syntax, or embed application-specific settings.

## API

The C interface lives under `system/config/include/sheen/config.h`. A small CLI is provided as `/usr/sbin/sheen-config` in the Sheen root filesystem for diagnostics and development.

## Validation

`tools/test-config.sh` compiles the real implementation with warnings treated as errors, loads configuration, performs repeated updates, reloads the file, and verifies the final persisted values.
