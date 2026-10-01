# Phase 4.6 — Audio Pipeline

Phase 4.6 establishes the generic PCM audio pipeline between decoded media and output backends.

## Contract

The pipeline carries actual PCM buffers with sample rate, channel count, sample format, frame count, and presentation timestamp. Output is provided through a sink backend with open/write/flush/close callbacks.

The sink owns or copies each submitted buffer before `write()` returns. `EAGAIN` is propagated as output backpressure; other write failures transition the pipeline to error.

This boundary is intentionally independent of ALSA, HDMI, Bluetooth, or any particular hardware output. The Linux audio probe from Phase 3.4 remains responsible for discovering available sound hardware.

## Scope

Resampling, channel mixing, volume policy, synchronization, device selection, and concrete ALSA/HDMI/Bluetooth sinks remain separate backend or policy layers.

## Validation

`tools/test-audio-pipeline.sh` exercises the real pipeline lifecycle and backpressure behavior with a contract test sink.
