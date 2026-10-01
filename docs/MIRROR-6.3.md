# Phase 6.3 — Screen Mirroring

Phase 6.3 establishes the network media ingress used by screen-mirroring receivers.

## RTP/H.264

The receiver accepts RTP payload types 96 and 97, validates the RTP header/CSRC/extension layout, handles H.264 single-NAL packets and FU-A fragmented NAL units, enforces fragment sequence/SSRC/timestamp continuity, and reconstructs Annex-B NAL output.

This is a media transport primitive rather than a complete phone-mirroring protocol. It can be driven by Miracast/Wi-Fi Display or another signaling layer that delivers RTP/H.264 to the receiver.

## Scope

Wi-Fi Direct negotiation, RTSP/Wi-Fi Display capability negotiation, source authentication, HDCP/content-protection policy, audio RTP, synchronized A/V presentation, and platform-specific phone discovery are later casting stages.

## Validation

`tools/test-mirror.sh` sends actual UDP RTP packets to a loopback receiver and verifies both a single H.264 NAL and a FU-A fragmented NAL are reconstructed.