# Phase 4.10 — Recording Engine

Phase 4.10 establishes a durable recording sink for transport-stream and future muxer outputs.

## Write model

Recordings are created as exclusive `destination.part` staging files. Media bytes are appended there through `sheen_recording_write()`. Finalization calls `fsync()`, closes the descriptor, and atomically renames the staging file to the destination.

An aborted or unfinalized recording leaves no completed destination file; the staging file is removed when the recorder is closed.

The recorder itself is container-neutral. A future MPEG-TS/MP4/MKV muxer can write properly formatted bytes into the same durable sink.

## Scope

Container muxing, segmented recording, DVR scheduling, timeshift retention, metadata/indexing, and recording UI are later TV/media layers.

## Validation

`tools/test-recording.sh` writes real bytes through the recorder, verifies exact output equality, checks staging-file cleanup, and validates the completed state.
