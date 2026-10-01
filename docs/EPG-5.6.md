# Phase 5.6 — Electronic Program Guide

Phase 5.6 establishes real EPG/service-information ingestion from DVB SI EIT alongside the existing XMLTV provider input.

## DVB EIT

The parser consumes MPEG transport-stream PID 0x12, reconstructs EIT sections, extracts service ID/event ID, UTC start time, duration, running status, free/CA state, and DVB short-event title/description metadata.

DVB timestamps use Modified Julian Date plus BCD time fields. Event records are normalized into the existing sheen_epg_event model, with service ID represented as the channel identifier when an external channel mapping is not yet available.

## Provider input

The existing XMLTV parser remains available through the same EPG document model, allowing DVB-broadcast EIT and network/provider XMLTV feeds to converge before the TV UI/database layer.

## Validation

tools/test-epg.sh builds the real parser and feeds it a standards-shaped DVB EIT transport packet, then validates timestamp, duration, service/event IDs, and short-event metadata.

## Scope

Full DVB descriptor coverage, extended-event text, content classification, parental ratings, schedule segmentation, EIT version/update handling, and persistent EPG database storage are subsequent enhancements.
