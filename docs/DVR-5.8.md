# Phase 5.8 — DVR

Phase 5.8 establishes persistent recording scheduling above the recording sink and channel database.

## Schedule model

Each DVR job stores a stable recording ID, channel ID, title, output path, start/end timestamps, and lifecycle state.

Jobs are persisted in SQLite and indexed by state and time. `sheen_dvr_list_due()` returns scheduled jobs whose time window contains the requested current timestamp.

## Lifecycle

Jobs can transition between scheduled, recording, completed, failed, and cancelled states. The scheduler itself does not fabricate recordings; a later runtime worker can claim a due job, open the channel transport, and feed actual bytes to the recording engine.

## Scope

Conflict resolution, tuner resource arbitration, EPG-driven automatic recording, recurring schedules, padding, retention policy, and UI controls remain later TV/runtime features.