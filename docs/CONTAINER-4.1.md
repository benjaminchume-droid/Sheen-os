# Phase 4.1 — Container Engine Foundation

Phase 4.1 establishes a real container inspection layer beneath demuxing and playback.

## Architecture

`media/demux/container.c` owns a registry of container probes. Each probe receives the actual bytes from a local source and either recognizes its format or declines. This keeps format handling modular and allows later demuxers to attach to a common container identity without forcing the Media Engine to know every format.

The current registry recognizes ISO Base Media File Format, Matroska/WebM, MPEG Transport Stream, Ogg, FLV, RIFF AVI/WAVE, MPEG Program Stream, and ASF signatures.

## Output

`sheen-container-probe` reports source, recognized container, MIME type, source size, inspected bytes, and seekability. Unknown formats are explicitly reported as unknown.

## Scope

4.1 is container identification and byte-source foundation. Full track parsing, timestamp extraction, demux packet production, codec parsing, and streaming demuxers are subsequent media stages.
