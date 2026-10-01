# Phase 4.9 — Media Library

Phase 4.9 establishes persistent media indexing on top of the container and filesystem primitives.

## Storage

The library uses SQLite with a durable `media` table keyed by deterministic IDs and a unique source path. It stores source, detected container, file size, modification time, and insertion time. An index on source supports repeated scans without creating duplicate records.

## Scanning

`media-library.c` recursively walks a configured filesystem tree. Regular files are passed to the real container inspector; unknown/non-media files are ignored rather than inserted as fake media. Recognized media is upserted into the catalog.

The installed CLI is `/usr/sbin/sheen-media-index`.

## Scope

Duration/track metadata extraction, thumbnails, artwork, watch state, search/ranking, library categories, network shares, and UI integration are later layers.

## Validation

`tools/test-media-library.sh` creates actual binary fixtures, scans them through the real container engine, verifies persistence with SQLite, and checks deterministic ID shape.
