# Phase 4.7 — Subtitle Engine

Phase 4.7 establishes a common subtitle cue representation and real text-subtitle parsers.

## Formats

- SRT: comma-millisecond timestamps and numeric cue sequence blocks
- WebVTT: `WEBVTT` headers with dot-millisecond timestamps

Both formats produce the same `sheen_subtitle_cue` representation with start/end timestamps and multiline text. `sheen_subtitle_find_active()` can locate all cues active at a playback position, allowing later renderers to handle overlap according to policy.

## Scope

ASS/SSA styling, bitmap subtitles, font selection, positioning/layout, subtitle rendering, and renderer synchronization are later presentation layers.

## Validation

`tools/test-subtitles.sh` parses real SRT and WebVTT fixtures and validates cue counts, timestamps, and multiline text.
