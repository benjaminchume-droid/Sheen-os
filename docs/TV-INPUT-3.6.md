# Phase 3.6 — TV Input Abstraction

Phase 3.6 establishes the Linux TV-input boundary beneath the future Live TV platform.

The current implementation discovers DVB class devices from `/sys/class/dvb` and correlates frontend entries to real `/dev/dvb/adapter*/frontend*` nodes when present. It reports actual accessibility and kernel uevent metadata.

No tuner, channel, frequency, transport stream, or broadcast provider is invented. Machines without DVB hardware report an empty/unavailable input set.

Channel scanning, frontend tuning, demuxing, EPG, timeshift, recording, network TV, and provider abstractions belong to Phase 5.
