# Phase 4.3 — Demuxing Foundation

Phase 4.3 turns a recognized container into actual stream/program entities.

## MPEG-TS implementation

`media/demux/mpegts-demux.c` reads real 188-byte transport-stream packets, parses the PAT to locate a program map table, then parses the PMT to extract PCR and elementary stream PIDs/types.

The first implementation intentionally focuses on standards-level program/stream extraction; packet delivery, timestamp normalization, descriptors, multiple-program selection, and full elementary-stream demux output are later extensions.

## Validation

`tools/test-demux.sh` creates a standards-shaped MPEG-TS fixture with PAT/PMT sections and validates the actual parser output. The fixture is test input, not runtime device simulation.
