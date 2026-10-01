# Phase 5.2 — Broadcast Abstraction

Phase 5.2 defines the common broadcast model above physical tuner controls and MPEG-TS demuxing.

## Model

`sheen_broadcast_source` carries source type, delivery system, frequency, and bandwidth. `sheen_broadcast_multiplex` contains normalized program records with service/program IDs, PMT/PCR PIDs, and elementary streams.

The implementation currently derives program and stream entities from the real MPEG-TS demuxer. DVB service names/provider metadata will be added when DVB SI tables are parsed in the EPG/service-information stages.

The abstraction also supports a `network` source type so the same program model can represent IPTV/network transport without coupling TV application logic to tuner devices.

## Scope

5.2 does not yet tune hardware, scan frequency plans, parse service descriptors, or populate EPG data. Those are subsequent stages.
