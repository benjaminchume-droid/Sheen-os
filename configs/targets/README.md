# Sheen Target Registry

Each supported platform has one canonical TOML target manifest in this directory.

A target manifest describes:
- architecture and firmware
- deployment medium
- kernel source/version
- image layout
- boot policy
- hardware capability baseline
- compatibility expectations

Target manifests describe hardware/platform contracts. They do not contain secrets,
developer machine paths, runtime state or user configuration.

Target IDs use lowercase ASCII and hyphens.

Examples reserved for later implementation:
- `x86_64-uefi-ssd`
- `arm64-uefi-usb`
- `arm64-sheen-box`
- `sheen-reference-board`
