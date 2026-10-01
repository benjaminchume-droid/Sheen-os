# Phase 4.5 — Hardware Video Decoding

Phase 4.5 establishes the hardware decoder selection/session boundary for Linux V4L2 memory-to-memory decoders.

The selector scans real `/dev/video*` devices, queries their V4L2 capabilities, and enumerates OUTPUT formats to find a device exposing the requested coded format. Supported codec identifiers currently map to standard V4L2 compressed formats such as H.264, HEVC, VP8, VP9, AV1, and MPEG-2.

This stage deliberately separates device selection from frame-buffer queueing and decoded-frame scheduling. The result is a real kernel decoder endpoint selection primitive; codec-specific buffer negotiation and output-frame handling can be attached through the generic decoder backend in later implementation work.

No hardware decoder is claimed when the kernel exposes no compatible V4L2 device.
