# Stage 0.2 — Reproducible Build System

Stage 0.2 defines how Sheen source becomes a target artifact.

```
target definition → host dependency check → verified source acquisition
→ kernel configuration/build → initramfs assembly → boot image assembly
→ artifact validation
```

Rules:
1. Builds are target-driven and cannot silently select another target.
2. External source archives are checksum-verified before extraction.
3. Build outputs live under `out/` and are disposable.
4. Target configuration is separate from host configuration.
5. No credentials or machine-specific paths belong in target definitions.
6. Missing required tools are hard failures.
7. Produced artifacts have deterministic expected locations.
8. CI uses the same build/test entry points as developers.
9. Build metadata records target and kernel inputs.

Stage 0.2 does not implement cross-compilation, package management or production
signing; those belong to later stages.

## Target contract

Stage 0.3 target manifests are canonical inputs to the build system. Build scripts must
select a target by ID and validate its manifest before producing artifacts.
