# Phase 3.2 — GPU Detection

Phase 3.2 specializes the hardware layer around actual PCI display-class devices.

The GPU probe reads `/sys/bus/pci/devices`, selects PCI class `0x03` display controllers, and reports vendor/device IDs and the bound kernel driver through the Sheen HAL. It does not assume Intel, AMD, NVIDIA, or any particular GPU model.

The output is JSON and reflects only devices exposed by the running kernel. An empty GPU list is a valid result on a machine without accessible PCI display hardware.

The probe is installed as `/usr/sbin/sheen-gpu-probe` in the Sheen root filesystem.
