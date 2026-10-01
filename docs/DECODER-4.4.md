# Phase 4.4 — Decoder Pipeline

Phase 4.4 establishes the common packet-to-frame pipeline boundary used by software and hardware decoder backends.

## Contract

The pipeline owns decoder lifecycle state and packet validation. A backend supplies open/send/receive/flush/close callbacks. The backend must consume or copy a submitted packet before returning; `EAGAIN` is propagated as backpressure and does not transition the pipeline to an error state. Other send failures transition to `SHEEN_DECODER_ERROR`.

The pipeline supports explicit `OPEN`, `DRAINING`, `EOF`, and `ERROR` states and propagates backend receive behavior without inventing frames.

## Packaging

The build produces `libsheen-decode.a` inside the real root filesystem for native Sheen services. Codec-specific backends are separate implementations and are not embedded into this generic pipeline.

## Validation

`tools/test-decoder.sh` compiles the real pipeline with strict warnings and exercises open, packet acceptance, backpressure, receive behavior, flush/drain state, and cleanup using a test backend.
