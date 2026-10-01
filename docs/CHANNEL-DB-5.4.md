# Phase 5.4 — Channel Database

Phase 5.4 establishes the persistent catalog for discovered TV services.

## Storage model

SQLite stores service/program identity, PMT/PCR PIDs, delivery system, scan frequency/bandwidth, display name/provider, and enabled state. A uniqueness constraint on service ID + frequency + delivery prevents repeated scans from producing duplicate channel records.

The database is designed to be fed by later service-information parsers; 5.4 does not assume a broadcast provider or regional naming policy.

The diagnostic CLI is installed as `/usr/sbin/sheen-channel-db`.

## Scope

Service descriptor parsing, EPG metadata, favorites/groups, playback state, and UI are subsequent TV stages.
