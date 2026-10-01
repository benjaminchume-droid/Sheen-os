# Stage 0.5 — Testing Infrastructure

Stage 0.5 establishes tests that protect the architecture and contracts before the runtime implementation becomes large.

## Test layers

1. Architecture tests — verify required boundaries exist and privileged kernel/device access does not leak into shell or applications.
2. Target tests — validate target manifests and target/build invariants.
3. API contract tests — parse every v1 contract and verify structural invariants.
4. Artifact tests — verify kernel, initramfs, image, and build metadata after a build.
5. CI integration — execute lightweight tests before the expensive boot-image build.

## Local entrypoints

- sh tools/test-architecture.sh
- sh tools/test-contracts.sh
- sh scripts/test.sh x86_64-uefi-usb after a build

The tests validate real repository state. They do not fabricate hardware, boot success, Android compatibility, tuner availability, or media playback.

## Future test layers

Later stages add boot-in-QEMU tests, hardware-in-the-loop tests, media fixture tests, tuner fixtures, casting protocol tests, vision pipeline correctness/performance tests, and Android compatibility tests.
