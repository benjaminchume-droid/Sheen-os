# Phase 8.6 — Android Storage Bridge

Phase 8.6 establishes a Sheen-owned storage boundary for Android app data, cache, and shared media.

Three configurable roots are exposed: private application data, cache data, and shared storage. Android paths are always resolved relative to one of those roots.

The resolver rejects absolute paths, Windows-style absolute paths, `.`/`..` traversal components, and overlong output. Directory creation uses the same validated resolver.

## Scope

Android scoped-storage semantics, MediaStore/SAF compatibility, content-provider URIs, external-volume discovery, quota enforcement, per-app ownership, and file-descriptor based policy enforcement are later storage compatibility stages.