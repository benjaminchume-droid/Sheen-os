# Phase 6.4 — Wireless Display

Phase 6.4 establishes the RTSP negotiation endpoint used by Wi-Fi Display/Miracast-style receivers.

The WFD service handles OPTIONS, GET_PARAMETER, SET_PARAMETER, SETUP, PLAY, and TEARDOWN, advertises video/audio capabilities, and negotiates the RTP server port that feeds the Phase 6.3 RTP/H.264 ingress.

Wi-Fi Direct group formation and source discovery remain platform/network responsibilities; this stage establishes the receiver-side RTSP contract once a source connects.

Production codec capability negotiation, audio transport, HDCP policy, and Wi-Fi Direct integration are later stages.