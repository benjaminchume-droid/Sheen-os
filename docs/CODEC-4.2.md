# Phase 4.2 — Codec Subsystem Foundation

Phase 4.2 establishes hardware codec capability discovery beneath decoder selection.

The V4L2 probe queries real `/dev/video*` devices with `VIDIOC_QUERYCAP` and records capture, output, and memory-to-memory capabilities plus driver/card/bus identity. This gives later decoder selection a real hardware capability source.

The probe is installed as `/usr/sbin/sheen-v4l2-codec-probe`.

Software codec implementations and format-specific decoder pipelines remain later Media Engine stages; absence of a V4L2 codec device is reported as absence, not simulated support.
